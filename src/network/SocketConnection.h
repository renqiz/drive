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

#pragma once

#include <functional>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "network/SocketEndPoint.h"
#include "network/Connection.h"

namespace dfs
{
  namespace network
  {
    class SocketConnection : public Connection
    {
    public:

      using SendDelegate = std::function<bool(SocketConnection *, const void *, size_t)>;

    public:

      explicit SocketConnection(EndPointPtr remote, int fd, SendDelegate send);

      ~SocketConnection() override;

      bool Send(const void * data, size_t len) override;

      void Shutdown(int flag);

      void Close();

      void OnReceive(const void * data, size_t len);

      SocketEndPoint * GetRemoteSocketEndPoint() const
      {
        return dynamic_cast<SocketEndPoint *>(this->RemoteEndPoint());
      }

      int SocketFd() const
      {
        return this->fd;
      }

    private:

      void SetNonBlock();

    private:

      int fd;

      SendDelegate send;

      std::mutex mutex;

      std::atomic<bool> closed{false};
    };
  }
}
