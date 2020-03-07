#include "catch.hpp"

#include <cstdint>
#include <limits>
#include <climits>

#include "../bitscan.hpp"

namespace tests
{
	TEST_CASE("TestBitScan") {
		SECTION("TestFirst") {
			for (unsigned long i = 0; i < 64; ++i)
				REQUIRE(i == bitscan::first(UINT64_C(1) << i));

			REQUIRE(bitscan::END == bitscan::first(UINT64_C(0)));
		}
        SECTION("TestNext") {
			constexpr auto bits = UINT64_C(0b10011101001);
            REQUIRE(3UL == bitscan::next(0, bits));
			REQUIRE(5UL == bitscan::next(3, bits));
			REQUIRE(6UL == bitscan::next(5, bits));
			REQUIRE(7UL == bitscan::next(6, bits));
			REQUIRE(10UL == bitscan::next(7, bits));
			REQUIRE(bitscan::END == bitscan::next(10, bits));
		}
        SECTION("TestForEach") {
			constexpr auto bits = UINT64_C(0b10011101001);
			constexpr unsigned long expected_index[6] = { 0, 3, 5, 6, 7, 10 };
			int bit = 0;
			BITSCAN_FOREACH(bits, index) {
				REQUIRE(expected_index[bit++] == index);
			}
			REQUIRE(6 == bit);
		}
	};
}