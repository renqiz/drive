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
#include "protocol/ReadBlockResponse.h"

namespace dfs
{
  namespace protocol
  {
    ReadBlockResponse::ReadBlockResponse()
      : Instruction(OpCode::READ_BLOCK_RESPONSE)
    {
    }


   ReadBlockResponse::ReadBlockResponse(uint32_t id)
      : Instruction(OpCode::READ_BLOCK_RESPONSE, id)
    {
    }


    ReadBlockResponse::~ReadBlockResponse()
    {
      if (this->dataOwned && this->data)
      {
        delete[] this->data;
        this->data = nullptr;
      }
    }


    void ReadBlockResponse::SetData(const void * data, uint32_t size)
    {
      this->SetData(const_cast<void *>(data), size, false);
    }


    void ReadBlockResponse::SetData(void * data, uint32_t size, bool move)
    {
      if (!data && size > 0)
      {
        return;
      }

      if (this->dataOwned && this->data)
      {
        delete[] this->data;
        this->data = nullptr;
        this->dataOwned = false;
      }

      this->size = size;

      if (size > 0)
      {
        if (move)
        {
          this->data = new uint8_t[size];
          memcpy(this->data, data, size);
        }
        else
        {
          this->data = reinterpret_cast<uint8_t *>(data);
        }

        this->dataOwned = move;
      }
    }


    bool ReadBlockResponse::Serialize(IOutputStream & output) const
    {
      if (!Instruction::Serialize(output))
      {
        return false;
      }

      return output.Write(this->size) &&
             output.Write(this->data, this->size);
    }


    bool ReadBlockResponse::Deserialize(IInputStream & input)
    {
      if (!Instruction::Deserialize(input))
      {
        return false;
      }

      if (!input.Read(&this->size))
      {
        return false;
      }

      if (this->size > 0)
      {
        this->data = new uint8_t[this->size];
        this->dataOwned = true;
        return input.Read(this->data, this->size);
      }

      return true;
    }


    void ReadBlockResponse::Print() const
    {
      printf("[%" PRIu32 "][READ_BLOCK_RESPONSE] size=%" PRIu32 "\n",
          this->InstructionId(),
          this->size);
    }
  }
}
