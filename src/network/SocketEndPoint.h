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

#include <stdint.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "network/IEndPoint.h"

namespace dfs
{
  namespace network
  {
    class SocketEndPoint : public IEndPoint
    {
    public:

      explicit SocketEndPoint(struct sockaddr * val, size_t len, int domain, int type);

      ~SocketEndPoint() override;

      struct sockaddr * Addr() const    { return reinterpret_cast<struct sockaddr *>(addr); }

      size_t Size() const               { return this->size; }

      int Domain() const                { return this->domain; }

      int Type() const                  { return this->type; }

    private:

      uint8_t * addr;

      size_t size;

      int domain;

      int type;
    };
  }
}