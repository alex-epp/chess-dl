#include <catch2/catch.hpp>

#include "../chess.hpp"

using namespace chess;


namespace tests
{
	TEST_CASE("TestMove") {
		SECTION("TestPreserveFlags") {
			for (unsigned int flag = 0b0000; flag <= 0b1111; ++flag) {
				REQUIRE(flag == Move(0, 0, flag).flags());
			}
		}
		SECTION("TestPreservesToFrom") {
			for (unsigned int from = 0; from < 64; ++from) {
				for (unsigned int to = 0; to < 64; ++to) {
					auto m = Move(from, to);
					REQUIRE(Square(from) == m.from());
					REQUIRE(Square(to) == m.to());
				}
			}
		}
        SECTION("TestPromotionType")
		{
			constexpr auto from = Square("E2"), to = Square("E4");

			REQUIRE(Piece::BISHOP == Move(from, to, Move::B_PROMOTION).promoted_type());
			REQUIRE(Piece::BISHOP == Move(from, to, Move::B_PROMO_CAPTURE).promoted_type());
			REQUIRE(Piece::BISHOP == Move(from, to, Move::QUIET, Piece::BISHOP).promoted_type());
			
			REQUIRE(Piece::KNIGHT == Move(from, to, Move::N_PROMOTION).promoted_type());
			REQUIRE(Piece::KNIGHT == Move(from, to, Move::N_PROMO_CAPTURE).promoted_type());
			REQUIRE(Piece::KNIGHT == Move(from, to, Move::QUIET, Piece::KNIGHT).promoted_type());
			
			REQUIRE(Piece::ROOK == Move(from, to, Move::R_PROMOTION).promoted_type());
			REQUIRE(Piece::ROOK == Move(from, to, Move::R_PROMO_CAPTURE).promoted_type());
			REQUIRE(Piece::ROOK == Move(from, to, Move::QUIET, Piece::ROOK).promoted_type());
			
			REQUIRE(Piece::QUEEN == Move(from, to, Move::Q_PROMOTION).promoted_type());
			REQUIRE(Piece::QUEEN == Move(from, to, Move::Q_PROMO_CAPTURE).promoted_type());
			REQUIRE(Piece::QUEEN == Move(from, to, Move::QUIET, Piece::QUEEN).promoted_type());
		}
	};
}
