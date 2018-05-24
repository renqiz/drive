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
#include <string>
#include <mutex>
#include <atomic>
#include "AsyncResult.h"
#include "network/IEndPoint.h"
#include "dsp/Instruction.h"

namespace dfs
{
  namespace network
  {
    class Connection;
    class ConnectionFactory;
    class MessageSender;
  }

  class DspClient
  {
  private:

    struct RequestId
    {
      network::Connection * connection;
      uint32_t id;

      struct Compare
      {
        bool operator()(const RequestId & lhs, const RequestId & rhs) const
        {
          return (lhs.connection < rhs.connection) ||
            (lhs.connection == rhs.connection && lhs.id < rhs.id);
        }
      };
    };


  public:

    explicit DspClient(network::ConnectionFactory * factory);

    ~DspClient();

    int Connect(network::EndPointPtr endpoint);

    void Disconnect(int handle);

    AsyncResultPtr CallAsync(int handle, std::unique_ptr<dsp::Instruction> instr);

  private:

    void OnDataReceived(network::Connection * conn, const void * data, size_t len);

    bool RegisterResult(network::Connection * conn, uint32_t id, AsyncResultPtr result);

    AsyncResultPtr ReleaseResult(network::Connection * conn, uint32_t id);

  private:

    std::map<int, network::Connection *> connections;

    std::mutex mutex;

    network::ConnectionFactory * factory;

    std::atomic<int> nextHandle{1};

    std::mutex resultsMutex;

    std::map<RequestId, AsyncResultPtr, RequestId::Compare> results;

    network::MessageSender * sender = nullptr;
  };
}
