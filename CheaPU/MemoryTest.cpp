#include "pch.h"

#include "MemoryChip.h"

namespace CheaPU {
	TEST(CPU, creation) {
		MemoryChip m;

		EXPECT_EQ(0, m[0]);
	}

	TEST(CPU, read_and_write) {
		MemoryChip m;
		m[0x45] = 45;
		EXPECT_EQ(45, m[0x45]);
	}
}

