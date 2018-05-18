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

#include <map>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <atomic>
#include "network/Server.h"

namespace dfs
{
  namespace network
  {
    class FileServer : public Server
    {
    public:

      ~FileServer() override;

      bool Listen(const IEndPoint * local) override;

      void Shutdown() override;

    private:

      void ThreadProc();

      void CheckConnections(const char * dirname);

      void CheckRequests(Connection * conn);

    private:

      FileEndPoint localEndPoint;

      std::map<FileEndPoint, Connection *, FileEndPoint::Compare> connections;

      std::thread thread;

      std::mutex mutex;

      std::condition_variable cond;

      std::atomic<bool> running{false};
    };
  }
}
