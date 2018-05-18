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
#include "network/IEndPoint.h"

namespace dfs
{
  namespace network
  {
    class FileEndPoint : public IEndPoint
    {
    public:

      static FileEndPoint * this_endpoint;

    public:

      explicit FileEndPoint(const std::string & name)
        : name(name)
        , hasId(false)
      {
      }


      explicit FileEndPoint(const std::string & name, uint16_t id)
        : name(name)
        , id(id)
        , hasId(true)
      {
      }


      const std::string & Name() const
      {
        return this->name;
      }


      uint16_t Id() const
      {
        assert(this->hasId);
        return this->id;
      }


      void SetId(uint16_t val)
      {
        this->id = val;
        this->hasId = true;
      }


    private:

      std::string name;

      uint16_t id;

      bool hasId;

    public:

      struct Compare
      {
        bool operator()(const FileEndPoint & lhs, const FileEndPoint & rhs)
        {
          int val = lhs.Name().compare(rhs.Name());
          return val < 0 || (val == 0 && lhs.Id() < rhs.Id());
        }
      };
    };
  }
}
