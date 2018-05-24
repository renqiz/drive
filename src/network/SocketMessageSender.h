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

#include <thread>
#include <memory>
#include <atomic>
#include "Buffer.h"
#include "LockFreeQueue.h"
#include "network/MessageSender.h"

namespace dfs
{
  namespace network
  {
    class SocketConnection;

    class SocketMessageSender : public MessageSender
    {
    private:

      struct Message
      {
        Buffer buffer;
        size_t offset;
        SocketConnection * connection;
      };

      using MessageQueue = LockFreeQueue<Message *>;

    public:

      SocketMessageSender();
      ~SocketMessageSender() override;

      bool Send(Connection * conn, const void * data, size_t len) override;

    private:

      bool Send(SocketConnection * connection, const void * data, size_t len);

      void ThreadProc();

    private:

      std::thread thread;

      std::atomic<bool> active{false};

      std::unique_ptr<MessageQueue> messages;

      int sv[2] = { -1, -1 };
    };
  }
}

