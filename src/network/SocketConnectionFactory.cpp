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

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <fcntl.h>
#include "network/SocketEndPoint.h"
#include "network/SocketConnection.h"
#include "network/SocketMessageSender.h"
#include "network/SocketConnectionFactory.h"

namespace dfs
{
  namespace network
  {
    Connection * SocketConnectionFactory::Connect(EndPointPtr endpoint, int timeout)
    {
      if (!endpoint)
      {
        return nullptr;
      }

      SocketEndPoint * sockEp = dynamic_cast<SocketEndPoint *>(endpoint.get());
      if (!sockEp)
      {
        return nullptr;
      }

      int connectFd = socket(sockEp->Domain(), sockEp->Type(), 0);
      if (connectFd == -1)
      {
        return nullptr;
      }

      int retry = 0;
      bool inProgress = false;

      struct timeval tv = {
        .tv_sec = timeout,
        .tv_usec = 0
      };

      struct timeval * pTime = timeout >= 0 ? &tv : nullptr;

      fd_set fds;
      FD_ZERO(&fds);
      FD_SET(connectFd, &fds);

      int err = 0;
      socklen_t errLen = sizeof(err);

      SocketConnection * connection = nullptr;

      int flags = fcntl(connectFd, F_GETFL, 0);
      if (flags < 0 || fcntl(connectFd, F_SETFL, (flags | O_NONBLOCK)) < 0)
      {
        goto cleanup;
      }

      while (connect(connectFd, sockEp->Addr(), sockEp->Size()) == -1)
      {
        if (timeout >= 0 && errno == EAGAIN)
        {
          if (++retry < timeout * 10)
          {
            usleep(100 * 1000);
            continue;
          }
        }

        if (errno != EINPROGRESS)
        {
          goto cleanup;
        }
        else
        {
          inProgress = true;
          break;
        }
      }

      if (select(connectFd + 1, NULL, &fds, NULL, pTime) <= 0)
      {
        goto cleanup;
      }

      if (inProgress)
      {
        if (getsockopt(connectFd, SOL_SOCKET, SO_ERROR, &err, &errLen) != 0 || err != 0)
        {
          goto cleanup;
        }
      }

      connection = new SocketConnection(endpoint, connectFd,
        [this](Connection * conn, const void * data, size_t len) {
          return this->Sender()->Send(static_cast<SocketConnection *>(conn), data, len);
        }
      );

cleanup:

      if (!connection)
      {
        close(connectFd);
      }

      return connection;
    }


    std::unique_ptr<MessageSender> SocketConnectionFactory::CreateSender()
    {
      return std::unique_ptr<MessageSender>(new SocketMessageSender());
    }
  }
}
