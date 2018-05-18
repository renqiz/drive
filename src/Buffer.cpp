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

#include <stdlib.h>
#include <string.h>
#include "Buffer.h"

namespace dfs
{
  Buffer::~Buffer()
  {
    if (this->buf)
    {
      free(this->buf);
      this->buf = nullptr;
    }
  }


  bool Buffer::Resize(size_t size, bool clean)
  {
    if (size == 0)
    {
      if (this->buf)
      {
        free(this->buf);
        this->buf = nullptr;
      }

      this->size = size;
      return true;
    }

    uint8_t * ptr = static_cast<uint8_t *>(realloc(this->buf, size));
    if (ptr)
    {
      this->buf = ptr;
      this->size = size;

      if (clean)
      {
        memset(this->buf, 0, this->size);
      }

      return true;
    }
    else
    {
      return false;
    }
  }
}
