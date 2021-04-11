#pragma once

#include <array>
#include <cstdint>

namespace CheaPU {

	/** Simulation of the memory.
	    Just a big, linear space (no segments, pages...).
		Addressable by the byte, as if an array. 
		No reactions for out-of-bound access (but expect the emulation
		to crash).*/
	class MemoryChip
	{
	public:
		MemoryChip();

		uint8_t& operator[](size_t idx);
		const uint8_t& operator[](size_t idx) const;

		/** The actual memory. 8K, for no particular reason. */
		std::array<uint8_t, 8 * 1024> storage;
	};
}

