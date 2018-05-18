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
#include <string.h>
#include "dsp/ReadBlockResponse.h"

namespace dfs
{
  namespace dsp
  {
    ReadBlockResponse::ReadBlockResponse()
      : Instruction(OpCode::READ_BLOCK_RESPONSE)
    {
    }


   ReadBlockResponse::ReadBlockResponse(uint32_t id)
      : Instruction(OpCode::READ_BLOCK_RESPONSE, id)
    {
    }


    bool ReadBlockResponse::Serialize(IOutputStream & output) const
    {
      if (!Instruction::Serialize(output))
      {
        return false;
      }

      if (!output.Write(static_cast<uint32_t>(this->buf.Size())))
      {
        return false;
      }

      if (this->buf.Size() > 0)
      {
        return output.Write(static_cast<uint8_t *>(this->buf.Buf()), this->buf.Size());
      }

      return true;
    }


    bool ReadBlockResponse::Deserialize(IInputStream & input)
    {
      if (!Instruction::Deserialize(input))
      {
        return false;
      }

      uint32_t length = 0;
      if (!input.Read(&length))
      {
        return false;
      }

      if (!this->buf.Resize(length))
      {
        return false;
      }

      if (length > 0)
      {
        return input.Read(static_cast<uint8_t *>(this->buf.Buf()), this->buf.Size());
      }

      return true;
    }


    void ReadBlockResponse::Print() const
    {
      printf("[%" PRIu32 "][READ_BLOCK_RESPONSE] size=%lu\n",
          this->InstructionId(),
          this->buf.Size());
    }
  }
}
