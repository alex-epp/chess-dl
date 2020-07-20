#pragma once


#include "piece.hpp"
#include "coord.hpp"

#include <algorithm>
#include <string_view>
#include <limits>
#include <cassert>
#include <string>

#ifdef max
#undef max
#endif


namespace chess {
	enum Direction {
		NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST
	};

	class BitBoard;
	class BitBoardIterator;

	class Square {
	private:
	    constexpr static unsigned int construct_from_uci(std::string_view uci) noexcept {
            assert(uci == "-" || (uci.length() == 2 && 'a' <= utils::tolower(uci[0]) && utils::tolower(uci[0]) <= 'h' && '1' <= uci[1] && uci[1] <= '8'));
            if (uci == "-") return EMPTY;
            unsigned int file = utils::tolower(uci[0]) - 'a';
            unsigned int rank = uci[1] - '1';
            assert(file < 8);
            assert(rank < 8);
            return file + (rank << 3u);
	    }

	public:
		constexpr static unsigned int EMPTY = std::numeric_limits<unsigned int>::max();

		constexpr Square(unsigned int square = EMPTY) noexcept : square(square) {
			assert(square < 64 || square == EMPTY);
		}
		constexpr Square(std::string_view uci) noexcept : Square(construct_from_uci(uci)) {}
		constexpr Square(const File file, const Rank rank) noexcept : square(static_cast<int>(file) + (to_integral(rank) << 3u)) {
			assert(square < 64);
		}

	public: // Data-access
        [[nodiscard]] constexpr Rank rank() const noexcept {
			return static_cast<Rank>(this->square / 8);
		}
        [[nodiscard]] constexpr File file() const noexcept {
		    return static_cast<File>(this->square % 8);
		}
        [[nodiscard]] constexpr unsigned int get() const noexcept {
		    return this->square;
		}
        [[nodiscard]] constexpr bool is_empty() const noexcept {
		    return this->square == Square::EMPTY;
		}

		[[nodiscard]] inline std::string uci() const noexcept {
		    return std::string(1, 'a' + static_cast<char>(this->file())) + char('1' + static_cast<char>(this->rank()));
		}

		[[nodiscard]] constexpr bool valid() const noexcept {
			return this->is_empty() || this->square < 64;
		}

	private:

		constexpr static int COMPASS_ROSE[9] = { 8, 9, 1, -7, -8, -9, -1, 7 };

	public: // Operations
		[[nodiscard]] constexpr Square shift(Direction d) const noexcept {
			return this->square + COMPASS_ROSE[d];
		}
		[[nodiscard]] constexpr Square shift(Direction d, int amount) const noexcept {
			return this->square + COMPASS_ROSE[d] * amount;
		}

        [[nodiscard]] constexpr Square north() const noexcept {
			return this->square + COMPASS_ROSE[NORTH];
		}
        [[nodiscard]] constexpr Square east() const noexcept {
			return this->square + COMPASS_ROSE[EAST];
		}
        [[nodiscard]] constexpr Square south() const noexcept {
			return this->square + COMPASS_ROSE[SOUTH];
		}
        [[nodiscard]] constexpr Square west() const noexcept {
			return this->square + COMPASS_ROSE[WEST];
		}
        [[nodiscard]] constexpr Square northeast() const noexcept {
			return this->square + COMPASS_ROSE[NORTHEAST];
		}
        [[nodiscard]] constexpr Square southeast() const noexcept {
			return this->square + COMPASS_ROSE[SOUTHEAST];
		}
        [[nodiscard]] constexpr Square southwest() const noexcept {
			return this->square + COMPASS_ROSE[SOUTHWEST];
		}
        [[nodiscard]] constexpr Square northwest() const noexcept {
			return this->square + COMPASS_ROSE[NORTHWEST];
		}

        [[nodiscard]] constexpr Square north(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[NORTH] * amount;
		}
        [[nodiscard]] constexpr Square east(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[EAST] * amount;
		}
        [[nodiscard]] constexpr Square south(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[SOUTH] * amount;
		}
        [[nodiscard]] constexpr Square west(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[WEST] * amount;
		}
        [[nodiscard]] constexpr Square northeast(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[NORTHEAST] * amount;
		}
        [[nodiscard]] constexpr Square southeast(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[SOUTHEAST] * amount;
		}
        [[nodiscard]] constexpr Square southwest(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[SOUTHWEST] * amount;
		}
        [[nodiscard]] constexpr Square northwest(const int amount) const noexcept {
			return this->square + COMPASS_ROSE[NORTHWEST] * amount;
		}

        [[nodiscard]] constexpr Square north(Colour c) const noexcept {
            return c == Colour::WHITE ? this->north() : this->south();
        }
        [[nodiscard]] constexpr Square south(Colour c) const noexcept {
            return c == Colour::WHITE ? this->south() : this->north();
        }
        [[nodiscard]] constexpr Square northeast(Colour c) const noexcept {
            return c == Colour::WHITE ? this->northeast() : this->southeast();
        }
        [[nodiscard]] constexpr Square southeast(Colour c) const noexcept {
            return c == Colour::WHITE ? this->southeast() : this->northeast();
        }
        [[nodiscard]] constexpr Square southwest(Colour c) const noexcept {
            return c == Colour::WHITE ? this->southwest() : this->northwest();
        }
        [[nodiscard]] constexpr Square northwest(Colour c) const noexcept {
            return c == Colour::WHITE ? this->northwest() : this->southwest();
        }

        [[nodiscard]] constexpr Square north(Colour c, const int amount) const noexcept {
            return c == Colour::WHITE ? this->north(amount) : this->south(amount);
        }
        [[nodiscard]] constexpr Square south(Colour c, const int amount) const noexcept {
            return c == Colour::WHITE ? this->south(amount) : this->north(amount);
        }
        [[nodiscard]] constexpr Square northeast(Colour c, const int amount) const noexcept {
            return c == Colour::WHITE ? this->northeast(amount) : this->southeast(amount);
        }
        [[nodiscard]] constexpr Square southeast(Colour c, const int amount) const noexcept {
            return c == Colour::WHITE ? this->southeast(amount) : this->northeast(amount);
        }
        [[nodiscard]] constexpr Square southwest(Colour c, const int amount) const noexcept {
            return c == Colour::WHITE ? this->southwest(amount) : this->northwest(amount);
        }
        [[nodiscard]] constexpr Square northwest(Colour c, const int amount) const noexcept {
            return c == Colour::WHITE ? this->northwest(amount) : this->southwest(amount);
        }

        [[nodiscard]] constexpr Square forward(Colour colour) const noexcept {
			auto dir = (colour == Colour::WHITE) ? 1 : -1;
			return this->north(dir);
		}
        [[nodiscard]] constexpr Square backward(Colour colour) const noexcept {
			auto dir = (colour == Colour::WHITE) ? 1 : -1;
			return this->south(dir);
		}
        [[nodiscard]] constexpr Square flip_vertical() const noexcept {
			if (this->square == EMPTY)
				return EMPTY;
			else
				return this->square ^ 56u;
		}

		[[nodiscard]] int distance_rank(Square s) const noexcept {
			return std::abs(static_cast<int>(this->rank()) - static_cast<int>(s.rank()));
		}
		[[nodiscard]] int distance_file(Square s) const noexcept {
			return std::abs(static_cast<int>(this->file()) - static_cast<int>(s.file()));
		}
		[[nodiscard]] int distance(Square s) const noexcept {
			return std::max(this->distance_rank(s), this->distance_file(s));
		}

		[[nodiscard]] constexpr Square orient(Colour c) const noexcept {
		    if (c == Colour::WHITE) return this->get();
		    else return this->flip_vertical();
		}

		[[nodiscard]] constexpr bool is_white_square() const noexcept {
		    return static_cast<size_t>(this->rank()) % 2 !=
                    static_cast<size_t>(this->file()) % 2;
		}

        [[nodiscard]] constexpr bool operator == (const Square rhs) const noexcept {
			return this->square == rhs.square;
		}
        [[nodiscard]] constexpr bool operator != (const Square rhs) const noexcept {
            return this->square != rhs.square;
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