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
#include "protocol/WriteBlockResponse.h"


namespace dfs
{
  namespace protocol
  {
    WriteBlockResponse::WriteBlockResponse()
      : Instruction(OpCode::WRITE_BLOCK_RESPONSE)
    {
    }


    WriteBlockResponse::WriteBlockResponse(uint32_t id)
      : Instruction(OpCode::WRITE_BLOCK_RESPONSE, id)
    {
    }


    bool WriteBlockResponse::Serialize(IOutputStream & output) const
    {
      if (!Instruction::Serialize(output))
      {
        return false;
      }

      return output.Write(this->size);
    }


    bool WriteBlockResponse::Deserialize(IInputStream & input)
    {
      if (!Instruction::Deserialize(input))
      {
        return false;
      }

      return input.Read(&this->size);
    }


    void WriteBlockResponse::Print() const
    {
      printf("[%" PRIu32 "][WRITE_BLOCK_RESPONSE] size=%" PRIu32 "\n",
          this->InstructionId(),
          this->size);
    }
  }
}
