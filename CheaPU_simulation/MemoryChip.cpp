#include "pch.h"
#include "MemoryChip.h"

namespace CheaPU {

    MemoryChip::MemoryChip()
    {
        storage.fill(0);
    }

    uint8_t& MemoryChip::operator[](size_t idx)
    {
        return storage[idx];
    }

    const uint8_t& MemoryChip::operator[](size_t idx) const
    {
        return storage[idx];
    }

}
