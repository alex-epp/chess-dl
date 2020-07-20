#ifndef BITSCAN_H
#define BITSCAN_H

#include <cstdint>
#include <limits>

#ifndef __linux__
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)
#else
#include <cstdio>
#endif

#ifdef max
#undef max
#endif



namespace bitscan {
	constexpr unsigned int END = std::numeric_limits<unsigned int>::max();

#ifdef _WIN32
	inline unsigned int first(std::uint64_t mask) noexcept {
		unsigned long index = 0;
		if (_BitScanForward64(&index, mask))
		    return index;
		else
			return END;
	}
	inline unsigned int next(unsigned long last, std::uint64_t mask) noexcept {
		if (last >= 63)
			return END;

		unsigned long index = 0;
		if (_BitScanForward64(&index, mask >> (last + 1)))
			return index + last + 1;
		else
			return END;
	}
#else
    inline unsigned int first(std::uint64_t mask) noexcept {
	    if (mask == 0)
	        return END;
	    else
	        return __builtin_ctzl(mask);
    }
    inline unsigned int next(unsigned long last, std::uint64_t mask) noexcept {
	    mask >>= (last + 1);
        if (last >= 63 || mask == 0)
            return END;
        else
            return __builtin_ctzl(mask) + last + 1;
    }
#endif

	#define BITSCAN_FOREACH(mask, index) for (unsigned long index = bitscan::first((mask)); index != bitscan::END; index = bitscan::next(index, (mask)))
}

#endif
