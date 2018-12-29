#ifndef BITSCAN_H
#define BITSCAN_H

#include <intrin.h>
#include <cstdint>

#pragma intrinsic(_BitScanForward64)

namespace bitscan {
	inline unsigned long first(std::uint64_t mask) {
		unsigned long index = 0;
		if (_BitScanForward64(&index, mask))
			return index;
		else
			return -1;
	}
	inline unsigned long next(unsigned long last, std::uint64_t mask) {
		unsigned long index = 0;
		if (_BitScanForward64(&index, mask >> (last + 1)))
			return index + last + 1;
		else
			return -1;
	}

	#define BITSCAN_FOREACH(mask, index) for (unsigned long index = bitscan::first((mask)); index != -1UL; index = bitscan::next(index, (mask)))
}

#endif
