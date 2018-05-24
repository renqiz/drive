/*
  Copyright (c) 2018 Drive Foundation

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/

#include <unistd.h>
#include <poll.h>
#include <string.h>
#include <map>
#include <vector>
#include "Util.h"
#include "network/SocketConnection.h"
#include "network/SocketMessageReceiver.h"


namespace dfs
{
  namespace network
  {
    SocketMessageReceiver::SocketMessageReceiver()
    {
      this->added = std::unique_ptr<LockFreeQueue<SocketConnection *>>(new LockFreeQueue<SocketConnection *>());
      this->removed = std::unique_ptr<LockFreeQueue<SocketConnection *>>(new LockFreeQueue<SocketConnection *>());

      if (socketpair(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, this->sv) == 0)
      {
        this->active = true;
        this->thread = std::thread(std::bind(&SocketMessageReceiver::ThreadProc, this));
      }
    }


    SocketMessageReceiver::~SocketMessageReceiver()
    {
      if (this->active)
      {
        this->active = false;

        close(this->sv[0]);

        if (this->thread.joinable())
        {
          this->thread.join();
        }
      }

      SocketConnection * conn = nullptr;
      while (this->added->Consume(conn) || this->removed->Consume(conn))
      {
        conn->ReleaseReference();
      }
    }


    void SocketMessageReceiver::AddConnection(SocketConnection * conn)
    {
      conn->AddReference();
      this->added->Produce(conn);

      uint8_t sig = 1;
      send(this->sv[0], &sig, sizeof(sig), 0);
    }


    void SocketMessageReceiver::RemoveConnection(SocketConnection * conn)
    {
      conn->AddReference();
      this->removed->Produce(conn);

      uint8_t sig = 2;
      send(this->sv[0], &sig, sizeof(sig), 0);
    }


    void SocketMessageReceiver::ThreadProc()
    {
      std::map<int, std::unique_ptr<ReceiverBuffer>> connections;

      while (this->active)
      {
        SocketConnection * conn = nullptr;
        while (this->added->Consume(conn))
        {
          if (connections.find(conn->SocketFd()) == connections.end())
          {
            std::unique_ptr<ReceiverBuffer> ptr = std::unique_ptr<ReceiverBuffer>(new ReceiverBuffer());
            ptr->conn = conn;

            connections[conn->SocketFd()] = std::move(ptr);
          }
          else
          {
            conn->ReleaseReference();
          }
        }

        while (this->removed->Consume(conn))
        {
          auto itr = connections.find(conn->SocketFd());
          if (itr != connections.end())
          {
            itr->second->conn->ReleaseReference();
            connections.erase(itr);
          }

          conn->ReleaseReference();
        }

        std::vector<struct pollfd> fds;

        struct pollfd sigfd = {
          .fd = this->sv[1],
          .events = POLLIN,
          .revents = 0
        };

        fds.emplace_back(std::move(sigfd));

        for (const auto & pair : connections)
        {
          struct pollfd fd = {
            .fd = pair.first,
            .events = POLLIN,
            .revents = 0
          };

          fds.emplace_back(std::move(fd));
        }

        while (this->active && poll(&fds[0], fds.size(), -1) > 0)
        {
          if (!this->active)
          {
            break;
          }

          for (size_t i = 1; i < fds.size(); ++i)
          {
            if (fds[i].revents == POLLIN)
            {
              auto itr = connections.find(fds[i].fd);
              auto rb = itr->second.get();

              char buffer[BUFSIZ];
              ssize_t bytes = 0;
              while ((bytes = read(fds[i].fd, buffer, sizeof(buffer))) > 0)
              {
                auto offset = rb->buffer.Size();
                rb->buffer.Resize(offset + bytes);
                memcpy(((uint8_t *)rb->buffer.Buf()) + offset, buffer, bytes);

                while (rb->buffer.Size() >= sizeof(uint32_t) + rb->size)
                {
                  if (rb->size == 0)
                  {
                    // New message
                    memcpy(&rb->size, rb->buffer.Buf(), sizeof(uint32_t));
                    rb->size = util::ntoh(rb->size);
                  }
                  else
                  {
                    // We have a complete message
                    rb->conn->OnReceive(((uint8_t *)rb->buffer.Buf()) + sizeof(uint32_t), rb->size);

                    size_t remaining = rb->buffer.Size() - sizeof(uint32_t) - rb->size;

                    if (remaining > 0)
                    {
                      // More than 1 message
                      memmove(rb->buffer.Buf(), ((uint8_t *)rb->buffer.Buf()) + sizeof(uint32_t) + rb->size, remaining);
                    }

                    rb->buffer.Resize(remaining);
                    rb->size = 0;
                  }
                }
              }

              if (bytes != EAGAIN && bytes != EWOULDBLOCK)
              {
                // remote connection closed or error
                rb->conn->Close();
                rb->conn->ReleaseReference();
                connections.erase(itr);
              }
            }
          }

          if (this->active && fds[0].revents == POLLIN)
          {
            char buf[BUFSIZ];
            while (read(this->sv[1], buf, sizeof(buf)) > 0)
            {
              // clean up anything on signal socket
            }

            // We have different sets of sockets. Reconstruct the fds to poll.
            break;
          }
        } // poll loop
      } // active loop

      // exiting
      for (const auto & pair : connections)
      {
        pair.second->conn->ReleaseReference();
      }
    }
  }
}
