#ifndef BITBOARD_H
#define BITBOARD_H


#include "bitscan.hpp"
#include "square.hpp"
#include "coord.hpp"

#include <chess/utils/utils.hpp>

#include <cstdint>
#include <cassert>
#include <iostream>
#include <bitset>


namespace chess {
	class BitBoard;
	class BitBoardBitScanIterator {
	public:
		constexpr static unsigned int END = bitscan::END;

	public:
		explicit BitBoardBitScanIterator(const BitBoard*);
		BitBoardBitScanIterator& operator ++ ();
		bool operator != (unsigned int) const;
		Square operator * () const;

	private:
		const BitBoard* bb;
		Square square;
	};


	class BitBoardSubsetsIterator;
	class BitBoardSubsets {
	public:
	    class END {};

        explicit inline BitBoardSubsets(const BitBoard*);
        inline BitBoardSubsetsIterator begin() const;
        inline END end() const;
	private:
	    const BitBoard* bb;
    };

	/*
	Stores a board position represented as a bitboard
	*/
	class BitBoard {
	public: // Constructor from uint64
		constexpr BitBoard(std::uint64_t bb = 0) : bb(bb) {}
		constexpr BitBoard(const Square square) : BitBoard(UINT64_C(1) << square.get()) {}
		constexpr BitBoard(const Rank rank) : BitBoard(UINT64_C(0xFF) << 8 * static_cast<const int>(rank)) {}
		constexpr BitBoard(const File file) : BitBoard(UINT64_C(0x0101010101010101) << static_cast<const int>(file)) {}
		constexpr BitBoard(const File file, const Rank rank) : BitBoard(UINT64_C(0x01) << (static_cast<int>(file) + (static_cast<int>(rank) << 3))) {}

	public: // Alternate constructors
		BitBoard constexpr static from_west_files(int num_files) {
			assert(0 <= num_files && num_files <= 8);
			BitBoard files = 0;
			for (auto f = 7; num_files > 0; --f, --num_files) {
				assert(0 <= f && f < 8);
				files |= static_cast<File>(f);
			}
			return files;
		}
		BitBoard constexpr static from_east_files(int num_files) {
			assert(0 <= num_files && num_files <= 8);
			BitBoard files = 0;
			for (auto f = 0; num_files > 0; ++f, --num_files) {
				assert(0 <= f && f < 8);
				files |= static_cast<File>(f);
			}
			return files;
		}

	public: // Data access
		[[nodiscard]] constexpr bool is_piece_at(Square square) const {
			return !(*this & square).empty();
		}
        [[nodiscard]] constexpr bool contains(Square square) const {
            return this->is_piece_at(square);
        }
        [[nodiscard]] constexpr bool empty() const {
			return this->bb == UINT64_C(0);
		}
        [[nodiscard]] Square first() const {
	        return *BitBoardBitScanIterator(this);
        }

	public: // range-based iteration
        [[nodiscard]] inline auto begin() const { return BitBoardBitScanIterator(this); }
        [[nodiscard]] inline auto end() const { return BitBoardBitScanIterator::END; }

	public: // subsets
	    [[nodiscard]] inline auto subsets() const { return BitBoardSubsets(this); }

	public: // Operations we can perform on bitboards
		[[nodiscard]] constexpr BitBoard shift_N() const {
			return *this << 8;
		}
		[[nodiscard]] constexpr BitBoard shift_N(const int amount) const {
			return *this << (8 * amount);
		}
		[[nodiscard]] constexpr BitBoard shift_S() const {
			return *this >> 8;
		}
		[[nodiscard]] constexpr BitBoard shift_S(const int amount) const {
			return *this >> (8 * amount);
		}
		[[nodiscard]] constexpr BitBoard shift_W() const {
			return (*this >> 1) & ~BitBoard(File::H);
		}
		[[nodiscard]] constexpr BitBoard shift_W(const int amount) const {
			return (*this >> amount) & ~BitBoard::from_west_files(amount);
		}
		[[nodiscard]] constexpr BitBoard shift_E() const {
			return (*this << 1) & ~BitBoard(File::A);
		}
		[[nodiscard]] constexpr BitBoard shift_E(const int amount) const {
			return (*this << amount) & ~BitBoard::from_east_files(amount);
		}
		[[nodiscard]] constexpr BitBoard shift_NW() const {
			return (*this << 7) & ~BitBoard(File::H);
		}
		[[nodiscard]] constexpr BitBoard shift_NE() const {
			return (*this << 9) & ~BitBoard(File::A);
		}
		[[nodiscard]] constexpr BitBoard shift_SW() const {
			return (*this >> 9) & ~BitBoard(File::H);
		}
		[[nodiscard]] constexpr BitBoard shift_SE() const {
			return (*this >> 7) & ~BitBoard(File::A);
		}

		[[nodiscard]] constexpr BitBoard orient(Colour c) const {
            return c == Colour::WHITE ? *this : this->flip_vertical();
		}

		[[nodiscard]] constexpr BitBoard shift_N(Colour c) const {
            return c == Colour::WHITE ? this->shift_N() : this->shift_S();
		}
		[[nodiscard]] constexpr BitBoard shift_N(Colour c, const int amount) const {
            return c == Colour::WHITE ? this->shift_N(amount) : this->shift_S(amount);
		}
		[[nodiscard]] constexpr BitBoard shift_S(Colour c) const {
            return c == Colour::WHITE ? this->shift_S() : this->shift_N();
		}
		[[nodiscard]] constexpr BitBoard shift_S(Colour c, const int amount) const {
            return c == Colour::WHITE ? this->shift_S(amount) : this->shift_N(amount);
		}
		[[nodiscard]] constexpr BitBoard shift_NW(Colour c) const {
            return c == Colour::WHITE ? this->shift_NW() : this->shift_SW();
		}
		[[nodiscard]] constexpr BitBoard shift_NE(Colour c) const {
	        return c == Colour::WHITE ? this->shift_NE() : this->shift_SE();
		}
		[[nodiscard]] constexpr BitBoard shift_SW(Colour c) const {
            return c == Colour::WHITE ? this->shift_SW() : this->shift_NW();
		}
		[[nodiscard]] constexpr BitBoard shift_SE(Colour c) const {
            return c == Colour::WHITE ? this->shift_SE() : this->shift_NE();
        }

		[[nodiscard]] inline BitBoard flip_vertical() const {
	        return utils::byteswap(this->bb);
		}
		[[nodiscard]] inline size_t pop_count() const {
	        return std::bitset<64>(this->bb).count();
	    }

		[[nodiscard]] constexpr BitBoard fill_N_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			gen |= empty & (gen << 8);
			empty &= (empty << 8);
			gen |= empty & (gen << 16);
			empty &= (empty << 16);
			gen |= empty & (gen << 32);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_S_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			gen |= empty & (gen >> 8);
			empty &= (empty >> 8);
			gen |= empty & (gen >> 16);
			empty &= (empty >> 16);
			gen |= empty & (gen >> 32);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_E_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::A);
			gen |= empty & (gen << 1);
			empty &= (empty << 1);
			gen |= empty & (gen << 2);
			empty &= (empty << 2);
			gen |= empty & (gen << 4);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_W_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::H);
			gen |= empty & (gen >> 1);
			empty &= (empty >> 1);
			gen |= empty & (gen >> 2);
			empty &= (empty >> 2);
			gen |= empty & (gen >> 4);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_NE_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::A);
			gen |= empty & (gen << 9);
			empty &= (empty << 9);
			gen |= empty & (gen << 18);
			empty &= (empty << 18);
			gen |= empty & (gen << 36);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_SE_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::A);
			gen |= empty & (gen >> 7);
			empty &= (empty >> 7);
			gen |= empty & (gen >> 14);
			empty &= (empty >> 14);
			gen |= empty & (gen >> 28);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_NW_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::H);
			gen |= empty & (gen << 7);
			empty &= (empty << 7);
			gen |= empty & (gen << 14);
			empty &= (empty << 14);
			gen |= empty & (gen << 28);
			return gen;
		}
		[[nodiscard]] constexpr BitBoard fill_SW_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::H);
			gen |= empty & (gen >> 9);
			empty &= (empty >> 9);
			gen |= empty & (gen >> 18);
			empty &= (empty >> 18);
			gen |= empty & (gen >> 36);
			return gen;
		}

		[[nodiscard]] constexpr BitBoard attack_N_occluded(BitBoard empty) const {
			return fill_N_occluded(empty).shift_N();
		}
		[[nodiscard]] constexpr BitBoard attack_S_occluded(BitBoard empty) const {
			return fill_S_occluded(empty).shift_S();
		}
		[[nodiscard]] constexpr BitBoard attack_E_occluded(BitBoard empty) const {
			return fill_E_occluded(empty).shift_E();
		}
		[[nodiscard]] constexpr BitBoard attack_W_occluded(BitBoard empty) const {
			return fill_W_occluded(empty).shift_W();
		}
		[[nodiscard]] constexpr BitBoard attack_NE_occluded(BitBoard empty) const {
			return fill_NE_occluded(empty).shift_NE();
		}
		[[nodiscard]] constexpr BitBoard attack_SE_occluded(BitBoard empty) const {
			return fill_SE_occluded(empty).shift_SE();
		}
		[[nodiscard]] constexpr BitBoard attack_NW_occluded(BitBoard empty) const {
			return fill_NW_occluded(empty).shift_NW();
		}
		[[nodiscard]] constexpr BitBoard attack_SW_occluded(BitBoard empty) const {
			return fill_SW_occluded(empty).shift_SW();
		}

		constexpr bool operator == (const BitBoard rhs) const {
			return this->bb == rhs.bb;
		}
		constexpr BitBoard& operator |= (const BitBoard rhs) {
			this->bb |= rhs.bb;
			return *this;
		}
		constexpr BitBoard& operator &= (const BitBoard rhs) {
			this->bb &= rhs.bb;
			return *this;
		}
		constexpr BitBoard& operator ^= (const BitBoard rhs) {
			this->bb ^= rhs.bb;
			return *this;
		}
		constexpr BitBoard operator | (const BitBoard rhs) const {
			return this->bb | rhs.bb;
		}
		constexpr BitBoard operator & (const BitBoard rhs) const {
			return this->bb & rhs.bb;
		}
		constexpr BitBoard operator ^ (const BitBoard rhs) const {
			return this->bb ^ rhs.bb;
		}
		constexpr BitBoard operator << (const int rhs) const {
			return this->bb << rhs;
		}
		constexpr BitBoard operator >> (const int rhs) const {
			return this->bb >> rhs;
		}
		constexpr BitBoard operator ~ () const {
			return ~this->bb;
		}
		constexpr BitBoard operator - (const BitBoard rhs) {
	        return this->bb - rhs.bb;
	    }



	public:
		std::uint64_t bb;

		friend class BitBoardBitScanIterator;
	};

    class BitBoardSubsetsIterator {
    public:
        inline BitBoardSubsetsIterator& operator ++ ();
        [[nodiscard]] inline bool operator != (BitBoardSubsets::END) const;
        [[nodiscard]] inline BitBoard operator * () const;
        explicit inline BitBoardSubsetsIterator(const BitBoard*);
    private:
        const BitBoard* bb;
        BitBoard n;
        size_t i;
    };

	/*
	Helpful predefined bitboards
	*/
	namespace BB {
		constexpr auto A1 = BitBoard(Square("A1")), A2 = BitBoard(Square("A2")), A3 = BitBoard(Square("A3")), A4 = BitBoard(Square("A4")), A5 = BitBoard(Square("A5")), A6 = BitBoard(Square("A6")), A7 = BitBoard(Square("A7")), A8 = BitBoard(Square("A8"));
		constexpr auto B1 = BitBoard(Square("B1")), B2 = BitBoard(Square("B2")), B3 = BitBoard(Square("B3")), B4 = BitBoard(Square("B4")), B5 = BitBoard(Square("B5")), B6 = BitBoard(Square("B6")), B7 = BitBoard(Square("B7")), B8 = BitBoard(Square("B8"));
		constexpr auto C1 = BitBoard(Square("C1")), C2 = BitBoard(Square("C2")), C3 = BitBoard(Square("C3")), C4 = BitBoard(Square("C4")), C5 = BitBoard(Square("C5")), C6 = BitBoard(Square("C6")), C7 = BitBoard(Square("C7")), C8 = BitBoard(Square("C8"));
		constexpr auto D1 = BitBoard(Square("D1")), D2 = BitBoard(Square("D2")), D3 = BitBoard(Square("D3")), D4 = BitBoard(Square("D4")), D5 = BitBoard(Square("D5")), D6 = BitBoard(Square("D6")), D7 = BitBoard(Square("D7")), D8 = BitBoard(Square("D8"));
		constexpr auto E1 = BitBoard(Square("E1")), E2 = BitBoard(Square("E2")), E3 = BitBoard(Square("E3")), E4 = BitBoard(Square("E4")), E5 = BitBoard(Square("E5")), E6 = BitBoard(Square("E6")), E7 = BitBoard(Square("E7")), E8 = BitBoard(Square("E8"));
		constexpr auto F1 = BitBoard(Square("F1")), F2 = BitBoard(Square("F2")), F3 = BitBoard(Square("F3")), F4 = BitBoard(Square("F4")), F5 = BitBoard(Square("F5")), F6 = BitBoard(Square("F6")), F7 = BitBoard(Square("F7")), F8 = BitBoard(Square("F8"));
		constexpr auto G1 = BitBoard(Square("G1")), G2 = BitBoard(Square("G2")), G3 = BitBoard(Square("G3")), G4 = BitBoard(Square("G4")), G5 = BitBoard(Square("G5")), G6 = BitBoard(Square("G6")), G7 = BitBoard(Square("G7")), G8 = BitBoard(Square("G8"));
		constexpr auto H1 = BitBoard(Square("H1")), H2 = BitBoard(Square("H2")), H3 = BitBoard(Square("H3")), H4 = BitBoard(Square("H4")), H5 = BitBoard(Square("H5")), H6 = BitBoard(Square("H6")), H7 = BitBoard(Square("H7")), H8 = BitBoard(Square("H8"));

		constexpr auto R1 = BitBoard(Rank::R1), R2 = BitBoard(Rank::R2), R3 = BitBoard(Rank::R3), R4 = BitBoard(Rank::R4), R5 = BitBoard(Rank::R5), R6 = BitBoard(Rank::R6), R7 = BitBoard(Rank::R7), R8 = BitBoard(Rank::R8);
		constexpr auto A = BitBoard(File::A), B = BitBoard(File::B), C = BitBoard(File::C), D = BitBoard(File::D), E = BitBoard(File::E), F = BitBoard(File::F), G = BitBoard(File::G), H = BitBoard(File::H);

		constexpr auto ALL = A | B | C | D | E | F | G | H;
	}

	// TODO: should probably be in its own class
	template <typename T>
	T&& print_bitboard(T&& stream, BitBoard bb) {
		for (int r = 7; r >= 0; --r) {
			for (int f = 0; f < 8; ++f) {
				auto pos = Square(static_cast<File>(f), static_cast<Rank>(r));
				stream << (bb.is_piece_at(pos) ? '1' : '.') << ' ';
			}
			stream << std::endl;
		}
		return stream;
	}

	BitBoardSubsets::BitBoardSubsets(const BitBoard* bb) : bb(bb) {}
	BitBoardSubsetsIterator BitBoardSubsets::begin() const {
	    return BitBoardSubsetsIterator(this->bb);
	}
	BitBoardSubsets::END BitBoardSubsets::end() const {
	    return END{};
	}
	BitBoardSubsetsIterator::BitBoardSubsetsIterator(const BitBoard* bb) : bb(bb), n(0), i(0) {}
	BitBoardSubsetsIterator& BitBoardSubsetsIterator::operator ++ () {
	    this->n = (this->n - *this->bb) & *this->bb;
	    this->i ++;
	    return *this;
	}
	bool BitBoardSubsetsIterator::operator != (BitBoardSubsets::END) const {
	    return this->i == 0 || !this->n.empty();
	}
	BitBoard BitBoardSubsetsIterator::operator * () const {
	    return this->n;
	}
}


#endif