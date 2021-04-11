#include "pch.h"

#include "CPU.h"
#include "MemoryChip.h"

#include <functional>
#include <iostream>

namespace CheaPU {
	TEST(CPU, reset) {
		CPU c;

		c.reset();
		EXPECT_EQ(0, c.accumulator);
		EXPECT_EQ(0, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}


	TEST(CPU, nop) {
		CPU c;
		MemoryChip m;
		c.reset();

		m[0x00] = to_word(Opcode::NOP);
		c.cycle(m);
		c.cycle(m);

		EXPECT_EQ(0, c.accumulator);
		EXPECT_EQ(1, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}
	
	TEST(CPU, load) {
		CPU c;
		MemoryChip m;
		c.reset();

		m[0x00] = to_word(Opcode::LD);
		m[0x01] = 0x02;
		m[0x02] = 42;

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);

		EXPECT_EQ(42, c.accumulator);
		EXPECT_EQ(0x02, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}

	TEST(CPU, store) {
		CPU c;
		MemoryChip m;
		c.reset();
		c.accumulator = 12;

		m[0x00] = to_word(Opcode::ST);
		m[0x01] = 0x02;

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);

		EXPECT_EQ(12, m[0x02]);
		EXPECT_EQ(0x02, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}

	TEST(CPU, add) {
		CPU c;
		MemoryChip m;
		c.reset();

		m[0x00] = to_word(Opcode::ADD);
		m[0x01] = 0x30;
		m[0x02] = to_word(Opcode::ADD);
		m[0x03] = 0x30;
		m[0x30] = 42;

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);  // 1st sum (0+42)

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);  // 2nd sum (42+42)

		EXPECT_EQ(84, c.accumulator);
		EXPECT_EQ(0x04, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}

	TEST(CPU, jmp) {
		CPU c;
		MemoryChip m;
		c.reset();

		m[0x00] = to_word(Opcode::JMP);
		m[0x01] = 0x30;

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);

		EXPECT_EQ(0, c.accumulator);
		EXPECT_EQ(0x30, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}

	TEST(CPU, jze_zero) {
		CPU c;
		MemoryChip m;
		c.reset();

		m[0x00] = to_word(Opcode::JZE);
		m[0x01] = 0x30;

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);

		EXPECT_EQ(0, c.accumulator);
		EXPECT_EQ(0x30, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}

	TEST(CPU, jze_not_zero) {
		CPU c;
		MemoryChip m;
		c.reset();
		c.accumulator = 54;

		m[0x00] = to_word(Opcode::JZE);
		m[0x01] = 0x30;

		c.cycle(m);
		c.cycle(m);

		EXPECT_EQ(54, c.accumulator);
		EXPECT_EQ(0x02, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}

	TEST(CPU, sub) {
		CPU c;
		MemoryChip m;
		c.reset();

		m[0x00] = to_word(Opcode::ADD);
		m[0x01] = 0x30;
		m[0x02] = to_word(Opcode::SUB);
		m[0x03] = 0x30;
		m[0x30] = 42;

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);  // 1st sum (0+42)

		c.cycle(m);
		c.cycle(m);
		c.cycle(m);  // 2nd sub (42-42)

		EXPECT_EQ(0, c.accumulator);
		EXPECT_EQ(0x04, c.program_counter);
		EXPECT_EQ(0, c.error);
		EXPECT_EQ(0, c.overflow);
		EXPECT_EQ(0, c.zero);
	}
}
