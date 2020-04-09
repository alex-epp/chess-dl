#include "tests.hpp"
#include <catch2/catch.hpp>
#include <cstdint>

#include <chess/backend/chess.hpp>

using namespace chess;

namespace tests {
	constexpr BitBoard bb =
		(UINT64_C(0b00011000) << (8 * 7)) +
		(UINT64_C(0b00011000) << (8 * 6)) +
		(UINT64_C(0b00100100) << (8 * 5)) +
		(UINT64_C(0b11000010) << (8 * 4)) +
		(UINT64_C(0b01000011) << (8 * 3)) +
		(UINT64_C(0b00100100) << (8 * 2)) +
		(UINT64_C(0b00011000) << (8 * 1)) +
		(UINT64_C(0b00011000) << (8 * 0));

	TEST_CASE("TestBB") {
	    SECTION("TestShiftNorth") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b00011000) << (8 * 7)) +
					(UINT64_C(0b00100100) << (8 * 6)) +
					(UINT64_C(0b11000010) << (8 * 5)) +
					(UINT64_C(0b01000011) << (8 * 4)) +
					(UINT64_C(0b00100100) << (8 * 3)) +
					(UINT64_C(0b00011000) << (8 * 2)) +
					(UINT64_C(0b00011000) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				) == bb.shift_N()
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = shifted_bb.shift_N()) {
				REQUIRE(shifted_bb == bb.shift_N(i));
			}
		}
        SECTION("TestShiftSouth") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b00011000) << (8 * 6)) +
					(UINT64_C(0b00011000) << (8 * 5)) +
					(UINT64_C(0b00100100) << (8 * 4)) +
					(UINT64_C(0b11000010) << (8 * 3)) +
					(UINT64_C(0b01000011) << (8 * 2)) +
					(UINT64_C(0b00100100) << (8 * 1)) +
					(UINT64_C(0b00011000) << (8 * 0))
				) == bb.shift_S()
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = shifted_bb.shift_S()) {
				REQUIRE(shifted_bb == bb.shift_S(i));
			}
		}
        SECTION("TestShiftEast") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b00110000) << (8 * 7)) +
					(UINT64_C(0b00110000) << (8 * 6)) +
					(UINT64_C(0b01001000) << (8 * 5)) +
					(UINT64_C(0b10000100) << (8 * 4)) +
					(UINT64_C(0b10000110) << (8 * 3)) +
					(UINT64_C(0b01001000) << (8 * 2)) +
					(UINT64_C(0b00110000) << (8 * 1)) +
					(UINT64_C(0b00110000) << (8 * 0))
				) == bb.shift_E()
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = shifted_bb.shift_E()) {
				REQUIRE(shifted_bb == bb.shift_E(i));
			}
		}
		SECTION("TestShiftWest") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b00001100) << (8 * 7)) +
					(UINT64_C(0b00001100) << (8 * 6)) +
					(UINT64_C(0b00010010) << (8 * 5)) +
					(UINT64_C(0b01100001) << (8 * 4)) +
					(UINT64_C(0b00100001) << (8 * 3)) +
					(UINT64_C(0b00010010) << (8 * 2)) +
					(UINT64_C(0b00001100) << (8 * 1)) +
					(UINT64_C(0b00001100) << (8 * 0))
				) == bb.shift_W()
			);
			auto shifted_bb = bb;
			for (int i = 0; i < 8; ++i, shifted_bb = shifted_bb.shift_W()) {
				REQUIRE(shifted_bb == bb.shift_W(i));
			}
		}
		SECTION("TestShiftNE") {
			REQUIRE(bb.shift_N().shift_E() == bb.shift_NE());
		}
		SECTION("TestShiftNW") {
			REQUIRE(bb.shift_N().shift_W() == bb.shift_NW());
		}
        SECTION("TestShiftSE") {
			REQUIRE(bb.shift_S().shift_E() == bb.shift_SE());
		}
        SECTION("TestShiftSW") {
			REQUIRE(bb.shift_S().shift_W() == bb.shift_SW());
		}
        SECTION("TestFillNorthOccluded") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b10000010) << (8 * 7)) +
					(UINT64_C(0b10000010) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b01000000) << (8 * 4)) +
					(UINT64_C(0b01000100) << (8 * 3)) +
					(UINT64_C(0b01000100) << (8 * 2)) +
					(UINT64_C(0b01000100) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				) ==
				BitBoard(
					(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b10000010) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b00000000) << (8 * 4)) +
					(UINT64_C(0b00000000) << (8 * 3)) +
					(UINT64_C(0b00000000) << (8 * 2)) +
					(UINT64_C(0b01000100) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				).fill_N_occluded(~(BB::R5 ^ BB::G5 ^ BB::G6))
			);
		}
        SECTION("TestFillSouthOccluded") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b10000010) << (8 * 6)) +
					(UINT64_C(0b10000010) << (8 * 5)) +
					(UINT64_C(0b00000010) << (8 * 4)) +
					(UINT64_C(0b00000000) << (8 * 3)) +
					(UINT64_C(0b00000000) << (8 * 2)) +
					(UINT64_C(0b01000100) << (8 * 1)) +
					(UINT64_C(0b01000100) << (8 * 0))
				) ==
				BitBoard(
					(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b10000010) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b00000000) << (8 * 4)) +
					(UINT64_C(0b00000000) << (8 * 3)) +
					(UINT64_C(0b00000000) << (8 * 2)) +
					(UINT64_C(0b01000100) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				).fill_S_occluded(~(BB::R4 ^ BB::H4 ^ BB::H5))
			);
		}
        SECTION("TestFillEastOccluded") {
			REQUIRE(
				BitBoard(
					(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b00000000) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b00110000) << (8 * 4)) +
					(UINT64_C(0b00110000) << (8 * 3)) +
					(UINT64_C(0b10000000) << (8 * 2)) +
					(UINT64_C(0b00111100) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				) ==
				BitBoard(
					(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b00000000) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b00010000) << (8 * 4)) +
					(UINT64_C(0b00010000) << (8 * 3)) +
					(UINT64_C(0b10000000) << (8 * 2)) +
					(UINT64_C(0b00000100) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				).fill_E_occluded(~BB::G)
			);
		}
        SECTION("TestFillWestOccluded") {
			REQUIRE(
				BitBoard(
				(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b00000000) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b01110000) << (8 * 4)) +
					(UINT64_C(0b00000000) << (8 * 3)) +
					(UINT64_C(0b11110000) << (8 * 2)) +
					(UINT64_C(0b00000111) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				) ==
				BitBoard(
				(UINT64_C(0b00000000) << (8 * 7)) +
					(UINT64_C(0b00000000) << (8 * 6)) +
					(UINT64_C(0b00000000) << (8 * 5)) +
					(UINT64_C(0b01000000) << (8 * 4)) +
					(UINT64_C(0b00000000) << (8 * 3)) +
					(UINT64_C(0b10000000) << (8 * 2)) +
					(UINT64_C(0b00000100) << (8 * 1)) +
					(UINT64_C(0b00000000) << (8 * 0))
				).fill_W_occluded(~BB::D)
			);
		}
        SECTION("TestFillNEOccludedNoOcclude") {
			auto constexpr start = BB::C4 | BB::G5;
			auto constexpr empty = ~BitBoard(0);
			auto constexpr filled = start.fill_NE_occluded(empty);
			auto constexpr expected = BB::C4 | BB::D5 | BB::E6 | BB::F7 | BB::G8 | BB::G5 | BB::H6;
			REQUIRE(expected == filled);
		}
        SECTION("TestFillNWOccludedNoOcclude") {
			auto constexpr start = BB::C4 | BB::G5;
			auto constexpr empty = ~BitBoard(0);
			auto constexpr filled = start.fill_NW_occluded(empty);
			auto constexpr expected = BB::A6 | BB::B5 | BB::C4 | BB::D8 | BB::E7 | BB::F6 | BB::G5;
			REQUIRE(expected == filled);
		}
        SECTION("TestFillSEOccludedNoOcclude") {
			auto constexpr start = BB::C4 | BB::G5;
			auto constexpr empty = ~BitBoard(0);
			auto constexpr filled = start.fill_SE_occluded(empty);
			auto constexpr expected = BB::C4 | BB::D3 | BB::E2 | BB::F1 | BB::G5 | BB::H4;
			REQUIRE(expected == filled);
		}
        SECTION("TestFillSWOccludedNoOcclude") {
			auto constexpr start = BB::C4 | BB::G5;
			auto constexpr empty = ~BitBoard(0);
			auto constexpr filled = start.fill_SW_occluded(empty);
			auto constexpr expected = BB::A2 | BB::B3 | BB::C4 | BB::C1 | BB::D2 | BB::E3 | BB::F4 | BB::G5;
			REQUIRE(expected == filled);
		}
        SECTION("TestFillNEOccluded") {
			auto constexpr start = BB::C4 | BB::B5;
			auto constexpr empty = ~BB::R7;
			auto constexpr filled = start.fill_NE_occluded(empty);
			auto constexpr expected = BB::C4 | BB::D5 | BB::E6 | BB::B5 | BB::C6;
			REQUIRE(expected == filled);
		}
        SECTION("TestFillNWOccluded") {
			auto constexpr start = BB::D4 | BB::G5;
			auto constexpr empty = ~BB::R7;
			auto constexpr filled = start.fill_NW_occluded(empty);
			auto constexpr expected = BB::B6 | BB::C5 | BB::D4 | BB::F6 | BB::G5;
            REQUIRE(expected == filled);
		}
        SECTION("TestFillSEOccluded") {
			auto constexpr start = BB::C4 | BB::E5;
			auto constexpr empty = ~BB::R2;
			auto constexpr filled = start.fill_SE_occluded(empty);
			auto constexpr expected = BB::C4 | BB::D3 | BB::E5 | BB::F4 | BB::G3;
            REQUIRE(expected == filled);
		}
		SECTION("TestFillSWOccluded") {
			auto constexpr start = BB::C4 | BB::G5;
			auto constexpr empty = ~BB::R2;
			auto constexpr filled = start.fill_SW_occluded(empty);
			auto constexpr expected = BB::B3 | BB::C4 | BB::E3 | BB::F4 | BB::G5;
            REQUIRE(expected == filled);
		}
		SECTION("TestPopCount") {
	        auto bb = BB::B3 | BB::C3 | BB::E4 | BB::A6 | BB::G6 | BB::E7 | BB::G7;
	        REQUIRE(bb.pop_count() == 7);
	    }
	}
}