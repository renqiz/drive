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

#include "dsp/Instruction.h"

namespace dfs
{
  namespace dsp
  {
    class CreatePartitionRequest : public Instruction
    {
    public:

      CreatePartitionRequest();

      explicit CreatePartitionRequest(uint32_t id);

      uint32_t BlockSize() const        { return this->blockSize; }

      void SetBlockSize(uint32_t val)   { this->blockSize = val; }

      uint64_t BlockCount() const       { return this->blockCount; }

      void SetBlockCount(uint64_t val)  { this->blockCount = val; }

    public:

      bool Serialize(IOutputStream & output) const override;

      bool Deserialize(IInputStream & input) override;

      void Print() const override;

    private:

      uint32_t blockSize = 0;

      // TODO: use a contract to specify the total block count
      uint64_t blockCount = 0;
    };
  }
}
