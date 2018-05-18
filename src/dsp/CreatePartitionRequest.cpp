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

#include <inttypes.h>
#include "dsp/CreatePartitionRequest.h"


namespace dfs
{
  namespace dsp
  {
    CreatePartitionRequest::CreatePartitionRequest()
      : Instruction(OpCode::CREATE_PARTITION_REQUEST)
    {
    }


    CreatePartitionRequest::CreatePartitionRequest(uint32_t id)
      : Instruction(OpCode::CREATE_PARTITION_REQUEST, id)
    {
    }


    bool CreatePartitionRequest::Serialize(IOutputStream & output) const
    {
      if (!Instruction::Serialize(output))
      {
        return false;
      }

      return output.Write(this->blockSize) && output.Write(this->blockCount);
    }


    bool CreatePartitionRequest::Deserialize(IInputStream & input)
    {
      if (!Instruction::Deserialize(input))
      {
        return false;
      }

      if (input.Remainder() < sizeof(this->blockSize) + sizeof(this->blockCount))
      {
        return false;
      }

      input.Read(&this->blockSize);

      input.Read(&this->blockCount);

      return true;
    }


    void CreatePartitionRequest::Print() const
    {
      printf(
          "[%" PRIu32 "][CREATE_PARTITION_REQUEST] blockSize=%" PRIu32 " blockCount=%" PRIu64 "\n",
          this->InstructionId(),
          this->blockSize,
          this->blockCount
      );
    }
  }
}
