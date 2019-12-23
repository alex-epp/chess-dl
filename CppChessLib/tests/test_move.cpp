#include "stdafx.h"
#include "CppUnitTest.h"

using namespace Microsoft::VisualStudio::CppUnitTestFramework;

#include "../CppChessLib/chess.h"

using namespace chess;


namespace Microsoft {
	namespace VisualStudio {
		namespace CppUnitTestFramework {
			inline std::wstring ToString(const chess::Piece::Type& piece) {
				return piece_repr(piece).data();
			}

			inline std::wstring ToString(const chess::PositionIndex& position) {
				return ToString(position.get());
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
					Assert::AreEqual(PositionIndex(from), m.from());
					Assert::AreEqual(PositionIndex(to), m.to());
				}
			}
		}

		TEST_METHOD(TestPromotionType)
		{
			constexpr auto from = PositionIndex("E2"), to = PositionIndex("E4");

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
