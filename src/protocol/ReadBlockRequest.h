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
#include "protocol/Instruction.h"

namespace dfs
{
  namespace protocol
  {
    class ReadBlockRequest : public Instruction
    {
    public:

      ReadBlockRequest();

      explicit ReadBlockRequest(uint32_t id);

      const std::string & PartitionId() const       { return this->partitionId; }

      void SetPartitionId(std::string id)           { this->partitionId = std::move(id); }

      uint64_t BlockId() const                      { return this->blockId; }

      void SetBlockId(uint64_t id)                  { this->blockId = id; }

      uint32_t Offset() const                       { return this->offset; }

      void SetOffset(uint32_t val)                  { this->offset = val; }

      uint32_t Size() const                         { return this->size; }

      void SetSize(uint32_t val)                    { this->size = val; }

    public:

      bool Serialize(IOutputStream & output) const override;

      bool Deserialize(IInputStream & input) override;

      void Print() const override;

    private:

      std::string partitionId;

      uint64_t blockId = 0;

      uint32_t offset = 0;

      uint32_t size = 0;
    };
  }
}
