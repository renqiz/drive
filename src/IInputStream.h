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

#include <assert.h>
#include <string>
#include "Util.h"

namespace dfs
{
  class IInputStream
  {
  public:

    virtual ~IInputStream() = default;

    virtual const void * BufferAt(size_t offset) = 0;
    virtual size_t Length() = 0;
    virtual size_t Offset() = 0;
    virtual size_t Remainder() = 0;
    virtual void Rewind(size_t bytes) = 0;
    virtual void Skip(size_t bytes) = 0;
    virtual size_t ReadBuffer(void * buffer, size_t length) = 0;
    virtual size_t PeekBuffer(void * buffer, size_t length) = 0;
    virtual bool IsValid() = 0;

    template<typename T>
    T Peek()
    {
      T value;
      if (this->PeekBuffer(&value, sizeof(value)) == sizeof(value))
      {
        return util::ntoh(value);
      }
      else
      {
        return T();
      }
    }


    template<typename T>
    T Read()
    {
      T value;
      if (this->ReadBuffer(&value, sizeof(value)) == sizeof(value))
      {
        return util::ntoh(value);
      }
      else
      {
        return T();
      }
    }

    template<typename T>
    bool Read(T * val)
    {
      if (!val || this->Remainder() < sizeof(T))
      {
        return false;
      }

      *val = this->Read<T>();
      return true;
    }

    template<typename T>
    size_t Read(T * ary, size_t length)
    {
      size_t read = this->ReadBuffer(ary, length * sizeof(T)) / sizeof(T);
      if (read == length)
      {
        for (size_t idx = 0; idx < read; ++idx)
        {
          ary[idx] = util::ntoh(ary[idx]);
        }
      }
      return read;
    }


    bool ReadString(std::string & output)
    {
      if (!this->Remainder() < sizeof(uint32_t))
      {
        return false;
      }

      uint32_t length = this->Read<uint32_t>();
      if (length > 0)
      {
        output.resize(length);
        if (this->Read<char>((char *)output.data(), length) != length)
        {
          return false;
        }
      }

      return true;
    }
  };
}
