#pragma once

#include <cstdint>

#include "StepByStep.h"

/** \file */ // Forces Doxygen to pick up out-of-class enums (https://www.doxygen.nl/manual/commands.html#cmdfile).

namespace CheaPU {

	class MemoryChip;
	
	/** Constants for the operations that the CPU can execute. 
	    The enum values are the binary-machine language values.
		
		All instructions take 8 bits. All instructions are to be 
		followed by one operand (except where otherwise nonted).
		The operand is always 8 bit and it is the memory address
		where the value to work on resides. The second value for
		binary operations is is whatever the accumulator contains, implicitly.

		The machine is big endian.
		Example bits:
			Most significant > 1100 0000 < Least significant
			                 0xC    0

		Operations take 2 or more cycles. By and large, fetching anything in
		memory takes 1 cycle, so you need one cycle to fetch and decode, then
		whatever many cycles the operation uses. Almost always another cycle
		to load the value pointed by the operand.
		*/
	enum class Opcode : uint8_t {
		/** Does nothing. No operand. */
		NOP = 0x00,

		/** Load. Copy the value pointed by the operand in the accumulator.*/
		LD = 0x01,

		/** Store. Copy the value in the accumulator to the address in the operand.*/
		ST = 0x02,

		/** Sum the operand with the accumulator. Result goes in the accumulator. */
		ADD = 0x03,

		/** Enable the error flag to block the machine. No operand. */
		HALT = 0x04,

		/** Jump to the address in the operand. */
		JMP = 0x05,

		/** Jump to the address in the operand, if the accumulator value is 0. */
		JZE = 0x06,

		/** Subtract the value pointed by the operand from the accumulator. Result remains in the accumulator.*/
		SUB = 0x07
	};


	/** Converts the "assembly" opcode into the machine language number. */
	uint8_t to_word(const Opcode x);


	/** Emulated CPU. On the cheap, as the namespace says.
	    
		The fields represent the usual parts of a CPU, but this 
		"emulator" does not correspond to any real architecture.
		
		It reproduces the simplest CPU architecture I am aware of: no
		registers but the accumulator, 1-operand instructions, one
		addressing mode...
		
		@see Opcode for the programming details.*/
	class CPU  {
	public:
		/** Restore the CPU to the "just turned on" state. 
		    Zero the flags, put the program counter back to 0...*/
		void reset();

		/** Simulate a single machine cycle. 
		    Instructions that take more than one cycle will remain "in wait". */
		void cycle(MemoryChip& memory);

		/** @name CPU registers.
		*  Names are "obvious" (if you know the basics of CPU architectures). */
		/**@{*/
		uint8_t program_counter;
		uint8_t accumulator;
		/**@}*/


		/** @name CPU flags. */
		/**@{*/
		uint8_t overflow : 1;
		uint8_t zero : 1;
		uint8_t error : 1;
		/**@}*/

	private:
		CheaPU::StepByStep<bool> running_instruction;

		/** NOP that does not increment the program counter. 
		It immediately terminates so that the CPU can fetch the 1st real
		instruction. */
		CheaPU::StepByStep<bool> FakeInitInstruction();

		/** @name Implementations of machine instructions. */
		/**@{*/
		CheaPU::StepByStep<bool> NOP();
		CheaPU::StepByStep<bool> LD(MemoryChip& memory);
		CheaPU::StepByStep<bool> ST(MemoryChip& memory);
		CheaPU::StepByStep<bool> ADD(MemoryChip& memory);
		CheaPU::StepByStep<bool> HALT(MemoryChip& memory);
		CheaPU::StepByStep<bool> JMP(MemoryChip& memory);
		CheaPU::StepByStep<bool> JZE(MemoryChip& memory);
		CheaPU::StepByStep<bool> SUB(MemoryChip& memory);
		/**@}*/
	};

}