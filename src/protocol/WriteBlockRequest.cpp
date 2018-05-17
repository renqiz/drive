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

#include <string.h>
#include <inttypes.h>
#include "protocol/WriteBlockRequest.h"


namespace dfs
{
  namespace protocol
  {
    WriteBlockRequest::WriteBlockRequest()
      : Instruction(OpCode::WRITE_BLOCK_RESPONSE)
    {
    }


    WriteBlockRequest::WriteBlockRequest(uint32_t id)
      : Instruction(OpCode::WRITE_BLOCK_RESPONSE, id)
    {
    }


    WriteBlockRequest::~WriteBlockRequest()
    {
      if (this->dataOwned && this->data)
      {
        delete[] this->data;
        this->data = nullptr;
      }
    }


    void WriteBlockRequest::SetData(const void * data, uint32_t size)
    {
      this->SetData(const_cast<void *>(data), size, false);
    }


    void WriteBlockRequest::SetData(void * data, uint32_t size, bool move)
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


    bool WriteBlockRequest::Serialize(IOutputStream & output) const
    {
      if (!Instruction::Serialize(output))
      {
        return false;
      }

      if (!output.WriteString(this->partitionId) ||
          !output.Write(this->blockId) ||
          !output.Write(this->offset) ||
          !output.Write(this->size))
      {
        return false;
      }

      if (this->size > 0)
      {
        return output.Write(this->data, this->size);
      }

      return true;
    }

    
    bool WriteBlockRequest::Deserialize(IInputStream & input)
    {
      if (!Instruction::Deserialize(input))
      {
        return false;
      }

      if (!input.ReadString(this->partitionId) ||
          !input.Read(&this->blockId) ||
          !input.Read(&this->offset) ||
          !input.Read(&this->size))
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

    
    void WriteBlockRequest::Print() const
    {
      printf("[%" PRIu32 "][WRITE_BLOCK_REQUEST] partitionId='%s' blockId=%" PRIu64 " offset=%" PRIu32 " size=%" PRIu32 "\n",
          this->InstructionId(),
          this->partitionId.c_str(),
          this->blockId,
          this->offset,
          this->size);
    }
  }
}
