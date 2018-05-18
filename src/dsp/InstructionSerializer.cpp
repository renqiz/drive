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

#include "dsp/Protocol.h"
#include "dsp/InstructionSerializer.h"


namespace dfs
{
  namespace dsp
  {
    bool InstructionSerializer::Serialize(IOutputStream & output, const Instruction * instr)
    {
      if (!instr)
      {
        return false;
      }

      return instr->Serialize(output);
    }


    std::unique_ptr<Instruction> InstructionSerializer::Deserialize(IInputStream & input)
    {
      if (input.Remainder() < sizeof(uint16_t))
      {
        return nullptr;
      }

      uint16_t code = input.Peek<uint16_t>();
      if (code == 0 || code >= static_cast<uint16_t>(OpCode::__MAX__))
      {
        return nullptr;
      }

      auto instr = CreateInstruction(static_cast<OpCode>(code));
      if (!instr || !instr->Deserialize(input))
      {
        return nullptr;
      }

      return instr;
    }


    std::unique_ptr<Instruction> InstructionSerializer::CreateInstruction(OpCode code)
    {
      switch (code)
      {
        case OpCode::CREATE_PARTITION_REQUEST:  return std::unique_ptr<Instruction>(new CreatePartitionRequest());
        case OpCode::CREATE_PARTITION_RESPONSE: return std::unique_ptr<Instruction>(new CreatePartitionResponse());
        case OpCode::READ_BLOCK_REQUEST:        return std::unique_ptr<Instruction>(new ReadBlockRequest());
        case OpCode::READ_BLOCK_RESPONSE:       return std::unique_ptr<Instruction>(new ReadBlockResponse());
        case OpCode::WRITE_BLOCK_REQUEST:       return std::unique_ptr<Instruction>(new WriteBlockRequest());
        case OpCode::WRITE_BLOCK_RESPONSE:      return std::unique_ptr<Instruction>(new WriteBlockResponse());

        default:                                return nullptr;
      }
    }
  }
}
