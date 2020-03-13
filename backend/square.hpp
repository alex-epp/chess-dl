#pragma once


#include "piece.hpp"
#include "coord.hpp"

#include <string_view>
#include <limits>
#include <cassert>
#include <string>

#ifdef max
#undef max
#endif


namespace chess {
	class BitBoard;
	class BitBoardIterator;

	class Square {
	public: // Construct from unsigned int
		constexpr static unsigned int EMPTY = std::numeric_limits<unsigned int>::max();

		constexpr Square(unsigned int square = EMPTY) : square(square) {
			assert((square >= 0 && square < 64) || square == EMPTY);
		}
		constexpr Square(std::string_view uci) : Square() {
			assert(uci == "-" || (uci.length() == 2 && 'a' <= util::tolower(uci[0]) && util::tolower(uci[0]) <= 'h' && '1' <= uci[1] && uci[1] <= '8'));
			if (uci != "-") {
				auto file = util::tolower(uci[0]) - 'a';
				auto rank = uci[1] - '1';
				assert(0 <= file && file < 8);
				assert(0 <= rank && rank < 8);
				this->square = file + (rank << 3);
				assert(0 <= this->square && this->square < 64);
			}
		}
		constexpr Square(const File file, const Rank rank) : square(static_cast<int>(file) + (static_cast<int>(rank) << 3)) {
			assert(0 <= square && square < 64);
		}

	public: // Data-access
        [[nodiscard]] constexpr Rank rank() const {
			return static_cast<Rank>(this->square / 8);
		}
        [[nodiscard]] constexpr File file() const {
		    return static_cast<File>(this->square % 8);
		}
        [[nodiscard]] constexpr unsigned int get() const {
		    return this->square;
		}
        [[nodiscard]] constexpr bool is_empty() const {
		    return this->square == Square::EMPTY;
		}

		[[nodiscard]] inline const std::string uci() const {
		    return std::string(1, 'a' + static_cast<char>(this->file())) + char('1' + static_cast<char>(this->rank()));
		}

	private:
		enum Direction {
			NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST
		};
		constexpr static int COMPASS_ROSE[9] = { 8, 9, 1, -7, -8, -9, -2, 7 };

	public: // Operations
        [[nodiscard]] constexpr Square north() const {
			return this->square + COMPASS_ROSE[NORTH];
		}
        [[nodiscard]] constexpr Square east() const {
			return this->square + COMPASS_ROSE[EAST];
		}
        [[nodiscard]] constexpr Square south() const {
			return this->square + COMPASS_ROSE[SOUTH];
		}
        [[nodiscard]] constexpr Square west() const {
			return this->square + COMPASS_ROSE[WEST];
		}
        [[nodiscard]] constexpr Square northeast() const {
			return this->square + COMPASS_ROSE[NORTHEAST];
		}
        [[nodiscard]] constexpr Square southeast() const {
			return this->square + COMPASS_ROSE[SOUTHEAST];
		}
        [[nodiscard]] constexpr Square southwest() const {
			return this->square + COMPASS_ROSE[SOUTHWEST];
		}
        [[nodiscard]] constexpr Square northwest() const {
			return this->square + COMPASS_ROSE[NORTHWEST];
		}

        [[nodiscard]] constexpr Square north(const int amount) const {
			return this->square + COMPASS_ROSE[NORTH] * amount;
		}
        [[nodiscard]] constexpr Square east(const int amount) const {
			return this->square + COMPASS_ROSE[EAST] * amount;
		}
        [[nodiscard]] constexpr Square south(const int amount) const {
			return this->square + COMPASS_ROSE[SOUTH] * amount;
		}
        [[nodiscard]] constexpr Square west(const int amount) const {
			return this->square + COMPASS_ROSE[WEST] * amount;
		}
        [[nodiscard]] constexpr Square northeast(const int amount) const {
			return this->square + COMPASS_ROSE[NORTHEAST] * amount;
		}
        [[nodiscard]] constexpr Square southeast(const int amount) const {
			return this->square + COMPASS_ROSE[SOUTHEAST] * amount;
		}
        [[nodiscard]] constexpr Square southwest(const int amount) const {
			return this->square + COMPASS_ROSE[SOUTHWEST] * amount;
		}
        [[nodiscard]] constexpr Square northwest(const int amount) const {
			return this->square + COMPASS_ROSE[NORTHWEST] * amount;
		}

        [[nodiscard]] constexpr Square forward(Piece::Colour colour) const {
			auto dir = (colour == Piece::WHITE) ? 1 : -1;
			return this->north(dir);
		}
        [[nodiscard]] constexpr Square backward(Piece::Colour colour) const {
			auto dir = (colour == Piece::WHITE) ? 1 : -1;
			return this->south(dir);
		}

        [[nodiscard]] constexpr Square flip_vertical() const {
			if (this->square == EMPTY)
				return EMPTY;
			else
				return this->square ^ 56;
		}

        [[nodiscard]] constexpr bool operator == (const Square rhs) const {
			return this->square == rhs.square;
		}

	private:
		unsigned int square;

		friend class BitBoardBitScanIterator;
	};

	namespace S {
		constexpr Square A1 =  0, B1 =  1, C1 =  2, D1 =  3, E1 =  4, F1 =  5, G1 =  6, H1 =  7;
		constexpr Square A2 =  8, B2 =  9, C2 = 10, D2 = 11, E2 = 12, F2 = 13, G2 = 14, H2 = 15;
		constexpr Square A3 = 16, B3 = 17, C3 = 18, D3 = 19, E3 = 20, F3 = 21, G3 = 22, H3 = 23;
		constexpr Square A4 = 24, B4 = 25, C4 = 26, D4 = 27, E4 = 28, F4 = 29, G4 = 30, H4 = 31;
		constexpr Square A5 = 32, B5 = 33, C5 = 34, D5 = 35, E5 = 36, F5 = 37, G5 = 38, H5 = 39;
		constexpr Square A6 = 40, B6 = 41, C6 = 42, D6 = 43, E6 = 44, F6 = 45, G6 = 46, H6 = 47;
		constexpr Square A7 = 48, B7 = 49, C7 = 50, D7 = 51, E7 = 52, F7 = 53, G7 = 54, H7 = 55;
		constexpr Square A8 = 56, B8 = 57, C8 = 58, D8 = 59, E8 = 60, F8 = 61, G8 = 62, H8 = 63;
	};
}