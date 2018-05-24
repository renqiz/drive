/**
 *
 * MIT License
 * 
 * Copyright (c) 2018 drvcoin
 * 
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 * 
 * =============================================================================
 */

#pragma once

#include <atomic>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <utility>
#include <functional>

namespace dfs
{
  class IAsyncResult
  {
  public:

    virtual ~IAsyncResult();

    void Complete();

    bool Wait(int msTimeout = 0);

    bool IsCompleted() const    { return this->completed; }

  private:

    std::atomic<bool> completed{false};

    std::mutex mutex;

    std::condition_variable cond;
  };


  template<typename T>
  class AsyncResult : public IAsyncResult
  {
  public:
    
    using DeleteDelegate = std::function<void(AsyncResult<T> *)>;

    explicit AsyncResult(DeleteDelegate customDelete = nullptr)
      : customDelete(customDelete)
    {
    }

    ~AsyncResult() override
    {
      if (customDelete)
      {
        customDelete(this);
      }
    }

    template<typename V>
    void Complete(V && value)
    {
      this->result = std::forward<V>(value);
      static_cast<IAsyncResult *>(this)->Complete();
    }

    const T & GetResult() const
    {
      return result;
    }

    T & GetResult()
    {
      return result;
    }

  private:

    T result = {};

    DeleteDelegate customDelete;
  };


  namespace AsyncResultHelper
  {
    template<typename T>
    inline T & GetResult(const IAsyncResult * result)
    {
      auto rtn = static_cast<const AsyncResult<T> *>(result);
      return rtn->GetResult();
    }
  }


  using AsyncResultPtr = std::shared_ptr<IAsyncResult>;
}
