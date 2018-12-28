#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../CppChessLib/chess.h"
#include "../CppChessLib/chess.cpp" // ...yeah, I should *not* have to ever do this!

using namespace chess;


namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			std::wstring ToString(const Piece::Type& piece) {
				return ToString(static_cast<int>(piece));
			}
		}
	}
}

namespace tests
{
	TEST_CLASS(TestMove)
	{
	public:
		TEST_METHOD(TestPreserveFlags)
		{
			for (unsigned int flag = 0b0000; flag <= 0b1111; ++flag) {
				Assert::AreEqual(flag, Move(0, 0, flag).flags());
			}
		}

		TEST_METHOD(TestPreservesToFrom)
		{
			for (unsigned int from = 0; from < 64; ++from) {
				for (unsigned int to = 0; to < 64; ++to) {
					auto m = Move(from, to);
					Assert::AreEqual(from, m.from());
					Assert::AreEqual(to, m.to());
				}
			}
		}

		TEST_METHOD(TestPromotionType)
		{
			auto from = PosIndex::from_uci("E2"), to = PosIndex::from_uci("E4");

			Assert::AreEqual(Piece::BISHOP, Move(from, to, Move::B_PROMOTION).promoted_type());
			Assert::AreEqual(Piece::BISHOP, Move(from, to, Move::B_PROMO_CAPTURE).promoted_type());
			Assert::AreEqual(Piece::BISHOP, Move(from, to, Move::QUIET, Piece::BISHOP).promoted_type());
			
			Assert::AreEqual(Piece::KNIGHT, Move(from, to, Move::N_PROMOTION).promoted_type());
			Assert::AreEqual(Piece::KNIGHT, Move(from, to, Move::N_PROMO_CAPTURE).promoted_type());
			Assert::AreEqual(Piece::KNIGHT, Move(from, to, Move::QUIET, Piece::KNIGHT).promoted_type());
			
			Assert::AreEqual(Piece::ROOK, Move(from, to, Move::R_PROMOTION).promoted_type());
			Assert::AreEqual(Piece::ROOK, Move(from, to, Move::R_PROMO_CAPTURE).promoted_type());
			Assert::AreEqual(Piece::ROOK, Move(from, to, Move::QUIET, Piece::ROOK).promoted_type());
			
			Assert::AreEqual(Piece::QUEEN, Move(from, to, Move::Q_PROMOTION).promoted_type());
			Assert::AreEqual(Piece::QUEEN, Move(from, to, Move::Q_PROMO_CAPTURE).promoted_type());
			Assert::AreEqual(Piece::QUEEN, Move(from, to, Move::QUIET, Piece::QUEEN).promoted_type());
		}
	};
}
