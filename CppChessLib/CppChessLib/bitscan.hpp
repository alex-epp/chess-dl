#ifndef BITSCAN_H
#define BITSCAN_H

#include <cstdint>
#include <intrin.h>
#include <limits>

#ifdef max
#undef max
#endif

#pragma intrinsic(_BitScanForward64)

namespace bitscan {
	constexpr unsigned int END = std::numeric_limits<unsigned long>::max();

	inline unsigned long first(std::uint64_t mask) {
		unsigned long index = 0;
		if (_BitScanForward64(&index, mask))
			return index;
		else
			return END;
	}
	inline unsigned long next(unsigned long last, std::uint64_t mask) {
		if (last >= 63)
			return END;

		unsigned long index = 0;
		if (_BitScanForward64(&index, mask >> (last + 1)))
			return index + last + 1;
		else
			return END;
	}

	#define BITSCAN_FOREACH(mask, index) for (unsigned long index = bitscan::first((mask)); index != bitscan::END; index = bitscan::next(index, (mask)))
}

#endif
