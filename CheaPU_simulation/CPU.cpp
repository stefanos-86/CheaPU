#include "pch.h"
#include "CPU.h"

#include "MemoryChip.h"

#include <iostream>

namespace CheaPU {

	uint8_t to_word(const Opcode x) 
	{
		return static_cast<uint8_t>(x);
	}


	void CPU::reset()
	{
		accumulator = 0;
		program_counter = 0;
		overflow = 0;
		zero = 0;
		error = 0;

		running_instruction = FakeInitInstruction();
		running_instruction();  // CPU does nothing, but instruction is complete. 1st cycle will fetch real code.
	}

	void CPU::cycle(MemoryChip& memory) 
	{
		// Block on error: do nothing.
		if (error)
			return;

		if (running_instruction.completed()) {
			// Fetch
			uint8_t instruction = memory[program_counter];

			// Decode.
			if (instruction == to_word(Opcode::NOP)) {
				running_instruction = NOP();
			}
			else if (instruction == to_word(Opcode::LD)) {
				running_instruction = LD(memory);
			}
			else if (instruction == to_word(Opcode::ST)) {
				running_instruction = ST(memory);
			}
			else if (instruction == to_word(Opcode::ADD)) {
				running_instruction = ADD(memory);
			}
			else if (instruction == to_word(Opcode::HALT)) {
				running_instruction = HALT(memory);
			}
			else if (instruction == to_word(Opcode::JMP)) {
				running_instruction = JMP(memory);
			}
			else if (instruction == to_word(Opcode::JZE)) {
				running_instruction = JZE(memory);
			}
			else if (instruction == to_word(Opcode::SUB)) {
				running_instruction = SUB(memory);
			}
			else {
				// Illegal opcode.
				error = true;
			}

			// No execute! Access to memory to fetch used up the cycle. 
		}
		else  // Keep doing the old instruction.
			running_instruction();
	}

	CheaPU::StepByStep<bool> CPU::FakeInitInstruction()
	{
		co_return true;
	}

	CheaPU::StepByStep<bool> CPU::NOP()
	{
		program_counter++;
		co_return true;
	}

	CheaPU::StepByStep<bool> CPU::LD(MemoryChip& memory)
	{
		uint8_t source_address = memory[program_counter + 1];
		co_yield false;

		accumulator = memory[source_address];
		program_counter += 2;
		co_return true;
	}

	CheaPU::StepByStep<bool> CPU::ST(MemoryChip& memory)
	{
		uint8_t source_address = memory[program_counter + 1];
		co_yield false;

		memory[source_address] = accumulator;
		program_counter += 2;

		co_return true;
	}

	CheaPU::StepByStep<bool> CPU::ADD(MemoryChip& memory)
	{
		// TODO: overflow flag if overflow.

		uint8_t source_address = memory[program_counter + 1];
		co_yield false;

		accumulator += memory[source_address];
		program_counter += 2;
		co_return true;
	}

	CheaPU::StepByStep<bool> CPU::HALT(MemoryChip& memory)
	{
		error = true;
		co_return true;
	}


	CheaPU::StepByStep<bool> CPU::JMP(MemoryChip& memory)
	{
		uint8_t jump_to = memory[program_counter + 1];
		co_yield false;

		program_counter = jump_to;
		co_return true;
	}

	CheaPU::StepByStep<bool> CPU::JZE(MemoryChip& memory)
	{
		if (accumulator == 0) {
			uint8_t jump_to = memory[program_counter + 1];
			co_yield false;

			program_counter = jump_to;
			co_return true;
		}
		else {
			program_counter += 2;
			co_return true;
		}
	}

	CheaPU::StepByStep<bool> CPU::SUB(MemoryChip& memory)
	{
		// TODO: overflow flag if undeflow.

		uint8_t source_address = memory[program_counter + 1];
		co_yield false;

		accumulator -= memory[source_address];
		program_counter += 2;
		co_return true;
	}


}