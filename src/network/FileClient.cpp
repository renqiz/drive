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

#include "network/FileConnection.h"
#include "network/FileClient.h"

namespace dfs
{
  namespace network
  {
    std::atomic<uint16_t> FileClient::lastId{0};


    FileClient::FileClient(const FileEndPoint & local)
      : localEndPoint(local)
    {
    }


    Connection * FileClient::CreateConnection(const IEndPoint * remote)
    {
      auto remoteEndPoint = dynamic_cast<const FileEndPoint *>(remote);
      if (!remoteEndPoint)
      {
        return nullptr;
      }

      this->localEndPoint.SetId(++lastId);

      auto localPtr = std::unique_ptr<IEndPoint>(new FileEndPoint(this->localEndPoint));
      auto remotePtr = std::unique_ptr<IEndPoint>(new FileEndPoint(*remoteEndPoint));

      return new FileConnection(std::move(localPtr), std::move(remotePtr));
    }
  }
}
