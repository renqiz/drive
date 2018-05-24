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

#include <memory>
#include <functional>
#include "ReferenceCount.h"
#include "network/IEndPoint.h"

namespace dfs
{
  namespace network
  {
    class Connection : public ReferenceCount
    {
    public:

      using ReceiveEventHandler = std::function<void(Connection *, const void *, size_t)>;

      explicit Connection(EndPointPtr remote)
        : remote(remote)
      {
      }

      IEndPoint * RemoteEndPoint() const     { return this->remote.get(); }

      // Asynchronous send data and return immediately.
      // Return false if any error.
      virtual bool Send(const void * data, size_t len) = 0;

      void SetReceiveHandler(ReceiveEventHandler handler)   { this->onReceived = std::move(handler); }

    protected:

      void CallReceiveHandler(const void * data, size_t len)
      {
        if (this->onReceived)
        {
          this->onReceived(this, data, len);
        }
      }

    private:

      EndPointPtr remote;

      ReceiveEventHandler onReceived = nullptr;
    };
  }
}
