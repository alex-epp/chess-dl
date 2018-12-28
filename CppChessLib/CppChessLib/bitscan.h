#ifndef BITSCAN
#define BITSCAN

#include <intrin.h>
#include <cstdint>

#pragma intrinsic(_BitScanForward64)

namespace bitscan {
	constexpr unsigned long first(std::uint64_t mask) {
		unsigned long index = 0;
		if (_BitScanForward64(&index, mask))
			return index;
		else
			return -1;
	}
	constexpr unsigned long next(unsigned long begin, std::uint64_t mask) {
		if (_BitScanForward64(&begin, mask >> begin))
			return begin;
		else
			return -1;
	}

	#define BITSCAN_FOREACH(mask, index) for (unsigned long index = bitscan::first((mask)); index <= 64; index = bitscan::next(index, (mask)))
}

#endif
