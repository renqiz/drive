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
#include "dsp/ReadBlockRequest.h"


namespace dfs
{
  namespace dsp
  {
    ReadBlockRequest::ReadBlockRequest()
      : Instruction(OpCode::READ_BLOCK_REQUEST)
    {
    }


    ReadBlockRequest::ReadBlockRequest(uint32_t id)
      : Instruction(OpCode::READ_BLOCK_REQUEST, id)
    {
    }


    bool ReadBlockRequest::Serialize(IOutputStream & output) const
    {
      if (!Instruction::Serialize(output))
      {
        return false;
      }

      return output.WriteString(this->partitionId) &&
             output.Write(this->blockId) &&
             output.Write(this->offset) &&
             output.Write(this->size);
    }


    bool ReadBlockRequest::Deserialize(IInputStream & input)
    {
      if (!Instruction::Deserialize(input))
      {
        return false;
      }

      return input.ReadString(this->partitionId) &&
             input.Read(& this->blockId) &&
             input.Read(& this->offset) &&
             input.Read(& this->size);
    }


    void ReadBlockRequest::Print() const
    {
      printf("[%" PRIu32 "][READ_BLOCK_REQUEST] partitionId='%s' blockId=%" PRIu64 " offset=%" PRIu32 " size=%" PRIu32 "\n",
          this->InstructionId(),
          this->partitionId.c_str(),
          this->blockId,
          this->offset,
          this->size);
    }
  }
}
