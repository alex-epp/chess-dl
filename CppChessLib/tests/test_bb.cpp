#include "stdafx.h"
#include "CppUnitTest.h"

#include <cstdint>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../CppChessLib/chess.h"


namespace tests
{
	TEST_CLASS(TestBB)
	{
	public:
		const chess::BB::BitBoard bb =
			(UINT64_C(0b00011000) << (8 * 7)) +
			(UINT64_C(0b00011000) << (8 * 6)) +
			(UINT64_C(0b00100100) << (8 * 5)) +
			(UINT64_C(0b11000010) << (8 * 4)) +
			(UINT64_C(0b01000011) << (8 * 3)) +
			(UINT64_C(0b00100100) << (8 * 2)) +
			(UINT64_C(0b00011000) << (8 * 1)) +
			(UINT64_C(0b00011000) << (8 * 0));

		TEST_METHOD(TestShiftNorth)
		{
			Assert::AreEqual(
				(UINT64_C(0b00011000) << (8 * 7)) +
				(UINT64_C(0b00100100) << (8 * 6)) +
				(UINT64_C(0b11000010) << (8 * 5)) +
				(UINT64_C(0b01000011) << (8 * 4)) +
				(UINT64_C(0b00100100) << (8 * 3)) +
				(UINT64_C(0b00011000) << (8 * 2)) +
				(UINT64_C(0b00011000) << (8 * 1)) +
				(UINT64_C(0b00000000) << (8 * 0)),
				chess::BB::shift_N(bb)
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = chess::BB::shift_N(shifted_bb)) {
				Assert::AreEqual(shifted_bb, chess::BB::shift_N(bb, i));
			}
		}

		TEST_METHOD(TestShiftSouth)
		{
			Assert::AreEqual(
				(UINT64_C(0b00000000) << (8 * 7)) +
				(UINT64_C(0b00011000) << (8 * 6)) +
				(UINT64_C(0b00011000) << (8 * 5)) +
				(UINT64_C(0b00100100) << (8 * 4)) +
				(UINT64_C(0b11000010) << (8 * 3)) +
				(UINT64_C(0b01000011) << (8 * 2)) +
				(UINT64_C(0b00100100) << (8 * 1)) +
				(UINT64_C(0b00011000) << (8 * 0)),
				chess::BB::shift_S(bb)
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = chess::BB::shift_S(shifted_bb)) {
				Assert::AreEqual(shifted_bb, chess::BB::shift_S(bb, i));
			}
		}

		TEST_METHOD(TestShiftWest)
		{
			Assert::AreEqual(
				(UINT64_C(0b00110000) << (8 * 7)) +
				(UINT64_C(0b00110000) << (8 * 6)) +
				(UINT64_C(0b01001000) << (8 * 5)) +
				(UINT64_C(0b10000100) << (8 * 4)) +
				(UINT64_C(0b10000110) << (8 * 3)) +
				(UINT64_C(0b01001000) << (8 * 2)) +
				(UINT64_C(0b00110000) << (8 * 1)) +
				(UINT64_C(0b00110000) << (8 * 0)),
				chess::BB::shift_W(bb)
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = chess::BB::shift_W(shifted_bb)) {
				Assert::AreEqual(shifted_bb, chess::BB::shift_W(bb, i));
			}
		}

		TEST_METHOD(TestShiftEast)
		{
			Assert::AreEqual(
				(UINT64_C(0b00001100) << (8 * 7)) +
				(UINT64_C(0b00001100) << (8 * 6)) +
				(UINT64_C(0b00010010) << (8 * 5)) +
				(UINT64_C(0b01100001) << (8 * 4)) +
				(UINT64_C(0b00100001) << (8 * 3)) +
				(UINT64_C(0b00010010) << (8 * 2)) +
				(UINT64_C(0b00001100) << (8 * 1)) +
				(UINT64_C(0b00001100) << (8 * 0)),
				chess::BB::shift_E(bb)
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = chess::BB::shift_E(shifted_bb)) {
				Assert::AreEqual(shifted_bb, chess::BB::shift_E(bb, i));
			}
		}

		TEST_METHOD(TestShiftNE)
		{
			Assert::AreEqual(chess::BB::shift_E(chess::BB::shift_N(bb)), chess::BB::shift_NE(bb));
		}

		TEST_METHOD(TestShiftNW)
		{
			Assert::AreEqual(chess::BB::shift_W(chess::BB::shift_N(bb)), chess::BB::shift_NW(bb));
		}

		TEST_METHOD(TestShiftSE)
		{
			Assert::AreEqual(chess::BB::shift_E(chess::BB::shift_S(bb)), chess::BB::shift_SE(bb));
		}

		TEST_METHOD(TestShiftSW)
		{
			Assert::AreEqual(chess::BB::shift_W(chess::BB::shift_S(bb)), chess::BB::shift_SW(bb));
		}
	};
}