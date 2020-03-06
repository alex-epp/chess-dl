#pragma once


#include "piece.hpp"
#include "coord.hpp"

#include <string_view>
#include <limits>
#include <cassert>

#ifdef max
#undef max
#endif


namespace chess {
	class BitBoard;
	class BitBoardIterator;

	class PositionIndex {
	public: // Construct from unsigned int
		constexpr static unsigned int EMPTY = std::numeric_limits<unsigned int>::max();

		constexpr PositionIndex(unsigned int position_index = EMPTY) : position_index(position_index) {
			assert((position_index >= 0 && position_index < 64) || position_index == EMPTY);
		}
		constexpr PositionIndex(std::string_view uci) : PositionIndex() {
			assert(uci == "-" || (uci.length() == 2 && 'a' <= util::tolower(uci[0]) && util::tolower(uci[0]) <= 'h' && '1' <= uci[1] && uci[1] <= '8'));
			if (uci != "-") {
				auto file = util::tolower(uci[0]) - 'a';
				auto rank = uci[1] - '1';
				assert(0 <= file && file < 8);
				assert(0 <= rank && rank < 8);
				this->position_index = file + (rank << 3);
				assert(0 <= this->position_index && this->position_index < 64);
			}
		}
		constexpr PositionIndex(const File file, const Rank rank) : position_index(static_cast<int>(file) + (static_cast<int>(rank) << 3)) {
			assert(0 <= position_index && position_index < 64);
		}

	public: // Data-access
		constexpr unsigned int get() const { return this->position_index; }
		constexpr Rank rank() const {
			return static_cast<Rank>(this->position_index / 8);
		}
		constexpr File file() const {
		    return static_cast<File>(this->position_index % 8);
		}

	private:
		enum Direction {
			NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST
		};
		constexpr static int COMPASS_ROSE[9] = { 8, 9, 1, -7, -8, -9, -2, 7 };

	public: // Operations
		constexpr PositionIndex north() const {
			return this->position_index + COMPASS_ROSE[NORTH];
		}
		constexpr PositionIndex east() const {
			return this->position_index + COMPASS_ROSE[EAST];
		}
		constexpr PositionIndex south() const {
			return this->position_index + COMPASS_ROSE[SOUTH];
		}
		constexpr PositionIndex west() const {
			return this->position_index + COMPASS_ROSE[WEST];
		}
		constexpr PositionIndex northeast() const {
			return this->position_index + COMPASS_ROSE[NORTHEAST];
		}
		constexpr PositionIndex southeast() const {
			return this->position_index + COMPASS_ROSE[SOUTHEAST];
		}
		constexpr PositionIndex southwest() const {
			return this->position_index + COMPASS_ROSE[SOUTHWEST];
		}
		constexpr PositionIndex northwest() const {
			return this->position_index + COMPASS_ROSE[NORTHWEST];
		}

		constexpr PositionIndex north(const int amount) const {
			return this->position_index + COMPASS_ROSE[NORTH] * amount;
		}
		constexpr PositionIndex east(const int amount) const {
			return this->position_index + COMPASS_ROSE[EAST] * amount;
		}
		constexpr PositionIndex south(const int amount) const {
			return this->position_index + COMPASS_ROSE[SOUTH] * amount;
		}
		constexpr PositionIndex west(const int amount) const {
			return this->position_index + COMPASS_ROSE[WEST] * amount;
		}
		constexpr PositionIndex northeast(const int amount) const {
			return this->position_index + COMPASS_ROSE[NORTHEAST] * amount;
		}
		constexpr PositionIndex southeast(const int amount) const {
			return this->position_index + COMPASS_ROSE[SOUTHEAST] * amount;
		}
		constexpr PositionIndex southwest(const int amount) const {
			return this->position_index + COMPASS_ROSE[SOUTHWEST] * amount;
		}
		constexpr PositionIndex northwest(const int amount) const {
			return this->position_index + COMPASS_ROSE[NORTHWEST] * amount;
		}

		constexpr PositionIndex forward(Piece::Colour colour) const {
			auto dir = (colour == Piece::WHITE) ? 1 : -1;
			return this->north(dir);
		}
		constexpr PositionIndex backward(Piece::Colour colour) const {
			auto dir = (colour == Piece::WHITE) ? 1 : -1;
			return this->south(dir);
		}

		constexpr PositionIndex flip_vertical() {
			if (this->position_index == EMPTY)
				return EMPTY;
			else
				return this->position_index ^ 56;
		}

		constexpr bool operator == (const PositionIndex rhs) const {
			return this->position_index == rhs.position_index;
		}

	private:
		unsigned int position_index;

		friend class BitBoardBitScanIterator;
	};

	namespace PI {
		constexpr PositionIndex A1 =  0, B1 =  1, C1 =  2, D1 =  3, E1 =  4, F1 =  5, G1 =  6, H1 =  7;
		constexpr PositionIndex A2 =  8, B2 =  9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15;
		constexpr PositionIndex A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23;
		constexpr PositionIndex A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31;
		constexpr PositionIndex A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39;
		constexpr PositionIndex A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47;
		constexpr PositionIndex A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55;
		constexpr PositionIndex A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63;
	};
}