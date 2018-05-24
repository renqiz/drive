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

#include <assert.h>
#include "network/Connection.h"
#include "network/ConnectionFactory.h"
#include "dsp/InstructionSerializer.h"
#include "BufferedOutputStream.h"
#include "BufferedInputStream.h"
#include "AsyncResult.h"
#include "DspClient.h"

namespace dfs
{
  DspClient::DspClient(network::ConnectionFactory * factory)
    : factory(factory)
  {
    assert(factory);

    this->sender = factory->Sender();

    assert(this->sender);
  }


  DspClient::~DspClient()
  {
    {
      std::unique_lock<std::mutex> lock(this->mutex);
      for (const auto & pair : this->connections)
      {
        pair.second->ReleaseReference();
      }

      this->connections.clear();
    }

    {
      std::unique_lock<std::mutex> lock(this->resultsMutex);
      for (const auto & pair : this->results)
      {
        pair.first.connection->ReleaseReference();
      }

      this->results.clear();
    }
  }


  int DspClient::Connect(network::EndPointPtr endpoint)
  {
    network::Connection * connection = this->factory->Connect(endpoint);

    if (!connection)
    {
      return -1;
    }

    using namespace std::placeholders;

    connection->SetReceiveHandler(std::bind(&DspClient::OnDataReceived, this, _1, _2, _3));

    int handle = this->nextHandle++;

    {
      std::unique_lock<std::mutex> lock(this->mutex);
      this->connections[handle] = connection;
    }

    return handle;
  }


  void DspClient::Disconnect(int handle)
  {
    network::Connection * conn = nullptr;

    {
      std::unique_lock<std::mutex> lock(this->mutex);
      auto itr = this->connections.find(handle);
      if (itr != this->connections.end())
      {
        conn = itr->second;
        this->connections.erase(itr);
      }
    }

    if (conn)
    {
      conn->ReleaseReference();
    }
  }


  AsyncResultPtr DspClient::CallAsync(int handle, std::unique_ptr<dsp::Instruction> instr)
  {
    if (!instr)
    {
      return nullptr;
    }

    network::Connection * conn = nullptr;

    {
      std::unique_lock<std::mutex> lock(this->mutex);
      auto itr = this->connections.find(handle);
      if (itr == this->connections.end())
      {
        return nullptr;
      }

      conn = itr->second;
      conn->AddReference();
    }

    AsyncResultPtr result = nullptr;

    BufferedOutputStream output;
    if (dsp::InstructionSerializer::Serialize(output, instr.get()))
    {
      result = AsyncResultPtr(new AsyncResult<dsp::Instruction *>(
        [](AsyncResult<dsp::Instruction *> * res)
        {
          auto instr = res->GetResult();
          if (!instr)
          {
            delete instr;
          }
        }
      ));

      if (!this->RegisterResult(conn, instr->InstructionId(), result) ||
          !this->sender->Send(conn, output.Buffer(), output.Offset()))
      {
        this->ReleaseResult(conn, instr->InstructionId());
        result = nullptr;
      }
    }

cleanup:

    conn->ReleaseReference();

    return result;
  }


  void DspClient::OnDataReceived(network::Connection * conn, const void * data, size_t len)
  {
    if (!conn)
    {
      return;
    }

    BufferedInputStream bis(reinterpret_cast<const uint8_t *>(data), len);
    auto instr = dsp::InstructionSerializer::Deserialize(bis);
    if (!instr)
    {
      return;
    }

    auto result = this->ReleaseResult(conn, instr->InstructionId());
    if (result)
    {
      auto ptr = static_cast<AsyncResult<dsp::Instruction *> *>(result.get());
      ptr->Complete(instr.release());
    }
  }


  bool DspClient::RegisterResult(network::Connection * conn, uint32_t id, AsyncResultPtr result)
  {
    if (!conn)
    {
      return false;
    }

    RequestId requestId = {
      .connection = conn,
      .id = id
    };

    std::unique_lock<std::mutex> lock(this->resultsMutex);

    const auto & itr = this->results.find(requestId);
    if (itr != this->results.end())
    {
      return false;
    }

    conn->AddReference();

    this->results[requestId] = result;
    return true;
  }


  AsyncResultPtr DspClient::ReleaseResult(network::Connection * conn, uint32_t id)
  {
    if (!conn)
    {
      return nullptr;
    }

    RequestId requestId = {
      .connection = conn,
      .id = id
    };

    AsyncResultPtr rtn = nullptr;

    std::unique_lock<std::mutex> lock(this->resultsMutex);

    const auto & itr = this->results.find(requestId);

    if (itr != this->results.end())
    {
      rtn = itr->second;
      itr->first.connection->ReleaseReference();
      this->results.erase(itr);
    }

    return rtn;
  }
}
