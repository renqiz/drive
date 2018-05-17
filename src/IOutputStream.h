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

#include <string>
#include "Util.h"

namespace dfs
{
  class IOutputStream
  {
  public:

    virtual ~IOutputStream() = default;

    virtual size_t WriteBuffer(const void * buffer, size_t size) = 0;

    template<typename T>
    bool Write(T val)
    {
      val = util::hton(val);
      return this->WriteBuffer(&val, sizeof(val)) == sizeof(val);
    }

    template<typename T>
    bool Write(T * ary, size_t length)
    {
      if (sizeof(T) > 1)
      {
        for (size_t idx = 0; idx < length; ++idx)
        {
          ary[idx] = util::hton(ary[idx]);
        }
      }
      bool result = this->WriteBuffer(ary, sizeof(T) * length) == sizeof(T) * length;
      if (sizeof(T) > 1)
      {
        for (size_t idx = 0; idx < length; ++idx)
        {
          ary[idx] = util::ntoh(ary[idx]);
        }
      }
      return result;
    }


    bool WriteString(const std::string & val)
    {
      return this->Write<uint32_t>(val.size()) && this->Write((char *)val.data(), val.size());
    }
  };
}
