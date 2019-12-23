#include "stdafx.h"
#include "CppUnitTest.h"

#include <cstdint>
#include <limits>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../CppChessLib/bitscan.hpp"


namespace tests
{
	TEST_CLASS(TestBitScan)
	{
	public:

		TEST_METHOD(TestFirst)
		{
			for (unsigned long i = 0; i < 64; ++i)
				Assert::AreEqual(i, bitscan::first(UINT64_C(1) << i));

			Assert::AreEqual(ULONG_MAX, bitscan::first(UINT64_C(0)));
		}

		TEST_METHOD(TestNext)
		{
			constexpr auto bits = UINT64_C(0b10011101001);
			Assert::AreEqual(3UL, bitscan::next(0, bits));
			Assert::AreEqual(5UL, bitscan::next(3, bits));
			Assert::AreEqual(6UL, bitscan::next(5, bits));
			Assert::AreEqual(7UL, bitscan::next(6, bits));
			Assert::AreEqual(10UL, bitscan::next(7, bits));
			Assert::AreEqual(ULONG_MAX, bitscan::next(10, bits));
		}

		TEST_METHOD(TestForEach)
		{
			constexpr auto bits = UINT64_C(0b10011101001);
			constexpr unsigned long expected_index[6] = { 0, 3, 5, 6, 7, 10 };
			int bit = 0;
			BITSCAN_FOREACH(bits, index) {
				Assert::AreEqual(expected_index[bit++], index);
			}
			Assert::AreEqual(6, bit);
		}
	};
}