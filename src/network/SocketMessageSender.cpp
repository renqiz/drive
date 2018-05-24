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
#include <set>
#include <vector>
#include "Util.h"
#include "network/SocketConnection.h"
#include "network/SocketMessageSender.h"

namespace dfs
{
  namespace network
  {
    SocketMessageSender::SocketMessageSender()
    {
      this->messages = std::unique_ptr<MessageQueue>(new MessageQueue());

      if (socketpair(AF_LOCAL, SOCK_STREAM | SOCK_NONBLOCK | SOCK_CLOEXEC, 0, this->sv) == 0)
      {
        this->active = true;
        this->thread = std::thread(std::bind(&SocketMessageSender::ThreadProc, this));
      }
    }


    SocketMessageSender::~SocketMessageSender()
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

      Message * entry = nullptr;
      while (this->messages->Consume(entry))
      {
        entry->connection->ReleaseReference();
        delete entry;
      }
    }

    bool SocketMessageSender::Send(Connection * connection, const void * data, size_t len)
    {
      SocketConnection * conn = dynamic_cast<SocketConnection *>(conn);
      if (!conn)
      {
        return false;
      }

      return this->Send(conn, data, len);
    }


    bool SocketMessageSender::Send(SocketConnection * connection, const void * data, size_t len)
    {
      auto entry = std::unique_ptr<Message>(new Message());

      connection->AddReference();

      entry->connection = connection;

      entry->buffer.Resize(len + sizeof(uint32_t));

      uint32_t packageSize = util::hton(static_cast<uint32_t>(len));

      memcpy(entry->buffer.Buf(), &packageSize, sizeof(uint32_t));

      memcpy(((uint8_t *)entry->buffer.Buf()) + sizeof(uint32_t), data, len);

      entry->offset = 0;

      if (this->messages->Produce(entry.get()))
      {
        entry.release();

        uint8_t sig = 1;
        send(this->sv[0], &sig, sizeof(sig), 0);

        return true;
      }

      return false;
    }


    void SocketMessageSender::ThreadProc()
    {
      std::multimap<int, Message *> messageMap;

      while (this->active)
      {
        std::vector<struct pollfd> fds;

        struct pollfd sigfd = {
          .fd = this->sv[1],
          .events = POLLIN,
          .revents = 0
        };

        fds.emplace_back(std::move(sigfd));

        Message * entry = nullptr;
        while (this->messages->Consume(entry))
        {
          messageMap.insert(std::pair<int, Message *>(entry->connection->SocketFd(), entry));
        }

        std::set<int> fdSet;

        for (const auto & pair : messageMap)
        {
          if (fdSet.find(pair.first) == fdSet.end())
          {
            fdSet.insert(pair.first);

            struct pollfd mfd = {
              .fd = pair.first,
              .events = POLLOUT,
              .revents = 0
            };

            fds.emplace_back(std::move(mfd));
          }
        }

        while (this->active && poll(&fds[0], fds.size(), -1) > 0)
        {
          if (!this->active)
          {
            break;
          }

          for (size_t i = 1; i < fds.size(); ++i)
          {
            if (fds[i].revents == POLLOUT)
            {
              for (auto itr = messageMap.find(fds[i].fd); itr != messageMap.end(); itr = messageMap.find(fds[i].fd))
              {
                Message * message = itr->second;

                auto sent = send(fds[i].fd, ((uint8_t *)message->buffer.Buf()) + message->offset, message->buffer.Size() - message->offset, 0);
                
                if (sent == EAGAIN || sent == EWOULDBLOCK)
                {
                  break;
                }
                else if (sent > 0 && sent < message->buffer.Size() - message->offset)
                {
                  // partial sent
                  message->offset += sent;
                  break;
                }
                else
                {
                  // Completely sent or something error
                  if (sent < 0)
                  {
                    message->connection->Close();
                  }

                  message->connection->ReleaseReference();
                  delete message;
                  messageMap.erase(itr);
                }
              }
            }
          }

          if (this->active && fds[0].events == POLLIN)
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
      for (auto & pair : messageMap)
      {
        pair.second->connection->ReleaseReference();
        delete pair.second;
      }
    }
  }
}
