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

#include <arpa/inet.h>

#define return_false_if(condition) \
  if (condition) { return false; }

#define return_false_if_msg(condition, ...) \
  if (condition) { printf(__VA_ARGS__); return false; }

uint64_t htonll(uint64_t val);
uint64_t ntohll(uint64_t val);

namespace dfs
{
  namespace util
  {
    // host byte order to network byte order

    template<typename T, size_t sizeT>
    struct __hton_impl;
    
    template<typename T>
    struct __hton_impl<T, 1>
    {
      T operator()(T val) { return val; }
    };
    
    template<typename T>
    struct __hton_impl<T, 2>
    {
      T operator()(T val) { return static_cast<T>(htons(static_cast<uint16_t>(val))); }
    };
    
    template<typename T>
    struct __hton_impl<T, 4>
    {
      T operator()(T val) { return static_cast<T>(htonl(static_cast<uint32_t>(val))); }
    };

    template<typename T>
    struct __hton_impl<T, 8>
    {
      T operator()(T val) { return static_cast<T>(htonll(static_cast<uint64_t>(val))); }
    };

    template<typename T>
    T hton(T val)
    {
      return __hton_impl<T, sizeof(T)>()(val);
    }


    // network byte order to host byte order

    template<typename T, size_t sizeT>
    struct __ntoh_impl;

    template<typename T>
    struct __ntoh_impl<T, 1>
    {
      T operator()(T val) { return val; }
    };

    template<typename T>
    struct __ntoh_impl<T, 2>
    {
      T operator()(T val) { return static_cast<T>(ntohs(static_cast<uint16_t>(val))); }
    };

    template<typename T>
    struct __ntoh_impl<T, 4>
    {
      T operator()(T val) { return static_cast<T>(ntohl(static_cast<uint32_t>(val))); }
    };

    template<typename T>
    struct __ntoh_impl<T, 8>
    {
      T operator()(T val) { return static_cast<T>(ntohll(static_cast<uint64_t>(val))); }
    };

    template<typename T>
    T ntoh(T val)
    {
      return __ntoh_impl<T, sizeof(T)>()(val);
    }
  }
}
