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
		explicit inline BitBoardBitScanIterator(const BitBoard*) noexcept;
		inline BitBoardBitScanIterator& operator ++ () noexcept;
		[[nodiscard]] inline bool operator != (unsigned int) const noexcept;
		[[nodiscard]] inline Square operator * () const noexcept;

	private:
		const BitBoard* bb;
		Square square;
	};


	class BitBoardSubsetsIterator;
	class BitBoardSubsets {
	public:
	    class END {};

        explicit inline BitBoardSubsets(const BitBoard*) noexcept;
        [[nodiscard]] inline BitBoardSubsetsIterator begin() const noexcept;
        [[nodiscard]] inline END end() const noexcept;
	private:
	    const BitBoard* bb;
    };

	/*
	Stores a board position represented as a bitboard
	*/
	class BitBoard {
	public: // Constructor from uint64
		constexpr BitBoard(std::uint64_t bb = 0) noexcept;
		constexpr BitBoard(Square square) noexcept;
		constexpr BitBoard(Rank rank) noexcept;
		constexpr BitBoard(File file) noexcept;
		constexpr BitBoard(File file, Rank rank) noexcept;

	public: // Alternate constructors
		[[nodiscard]] BitBoard constexpr static from_west_files(int num_files) noexcept;
        [[nodiscard]] BitBoard constexpr static from_east_files(int num_files) noexcept;

	public: // Data access
		[[nodiscard]] constexpr bool is_piece_at(Square square) const noexcept;
        [[nodiscard]] constexpr bool contains(Square square) const noexcept;
        [[nodiscard]] constexpr bool empty() const noexcept;
        [[nodiscard]] inline Square first() const noexcept;

	public: // range-based iteration
        [[nodiscard]] inline auto begin() const noexcept;
        [[nodiscard]] inline auto end() const noexcept;

	public: // subsets
	    [[nodiscard]] inline auto subsets() const noexcept;

	public: // Operations we can perform on bitboards
		[[nodiscard]] constexpr BitBoard shift_N() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_N(int amount) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_S() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_S(int amount) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_W() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_W(int amount) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_E() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_E(int amount) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_NW() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_NE() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_SW() const noexcept;
		[[nodiscard]] constexpr BitBoard shift_SE() const noexcept;

		[[nodiscard]] constexpr BitBoard orient(Colour c) const noexcept;

		[[nodiscard]] constexpr BitBoard shift_N(Colour c) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_N(Colour c, int amount) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_S(Colour c) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_S(Colour c, int amount) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_NW(Colour c) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_NE(Colour c) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_SW(Colour c) const noexcept;
		[[nodiscard]] constexpr BitBoard shift_SE(Colour c) const noexcept;

		[[nodiscard]] inline BitBoard flip_vertical() const noexcept;
		[[nodiscard]] inline size_t pop_count() const noexcept;

		[[nodiscard]] constexpr BitBoard fill_N_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_S_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_E_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_W_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_NE_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_SE_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_NW_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard fill_SW_occluded(BitBoard empty) const noexcept;

		[[nodiscard]] constexpr BitBoard attack_N_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_S_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_E_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_W_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_NE_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_SE_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_NW_occluded(BitBoard empty) const noexcept;
		[[nodiscard]] constexpr BitBoard attack_SW_occluded(BitBoard empty) const noexcept;

        [[nodiscard]] constexpr bool operator == (BitBoard rhs) const noexcept;
		constexpr BitBoard& operator |= (BitBoard rhs) noexcept;
		constexpr BitBoard& operator &= (BitBoard rhs) noexcept;
		constexpr BitBoard& operator ^= (BitBoard rhs) noexcept;
        [[nodiscard]] constexpr BitBoard operator | (BitBoard rhs) const noexcept;
        [[nodiscard]] constexpr BitBoard operator & (BitBoard rhs) const noexcept;
        [[nodiscard]] constexpr BitBoard operator ^ (BitBoard rhs) const noexcept;
        [[nodiscard]] constexpr BitBoard operator << (unsigned int rhs) const noexcept;
        [[nodiscard]] constexpr BitBoard operator >> (unsigned int rhs) const noexcept;
        [[nodiscard]] constexpr BitBoard operator ~ () const noexcept;
        [[nodiscard]] constexpr BitBoard operator - (BitBoard rhs) const noexcept;

	public:
		std::uint64_t bb;

		friend class BitBoardBitScanIterator;
	};

    class BitBoardSubsetsIterator {
    public:
        inline BitBoardSubsetsIterator& operator ++ () noexcept;
        [[nodiscard]] inline bool operator != (BitBoardSubsets::END) const noexcept;
        [[nodiscard]] inline BitBoard operator * () const noexcept;
        explicit inline BitBoardSubsetsIterator(const BitBoard*) noexcept;
    private:
        const BitBoard* bb;
        BitBoard n;
        size_t i;
    };

    constexpr BitBoard::BitBoard(std::uint64_t bb) noexcept
            : bb(bb) {}
    constexpr BitBoard::BitBoard(Square square) noexcept
            : BitBoard(UINT64_C(1) << square.get()) {}
    constexpr BitBoard::BitBoard(Rank rank) noexcept
            : BitBoard(UINT64_C(0xFF) << 8 * to_integral(rank)) {}
    constexpr BitBoard::BitBoard(File file) noexcept
            : BitBoard(UINT64_C(0x0101010101010101) << to_integral(file)) {}
    constexpr BitBoard::BitBoard(File file, Rank rank) noexcept
            : BitBoard(UINT64_C(0x01) << (to_integral(file) + (to_integral(rank) << 3u))) {}

    BitBoard constexpr BitBoard::from_west_files(int num_files) noexcept {
        assert(0 <= num_files && num_files <= 8);
        BitBoard files = 0;
        for (auto f = 7; num_files > 0; --f, --num_files) {
            files |= to_file(f);
        }
        return files;
    }
    BitBoard constexpr BitBoard::from_east_files(int num_files) noexcept {
        assert(0 <= num_files && num_files <= 8);
        BitBoard files = 0;
        for (auto f = 0; num_files > 0; ++f, --num_files) {
            files |= to_file(f);
        }
        return files;
    }
    constexpr bool BitBoard::is_piece_at(Square square) const noexcept {
        return !(*this & square).empty();
    }
    constexpr bool BitBoard::contains(Square square) const noexcept {
        return this->is_piece_at(square);
    }
    constexpr bool BitBoard::empty() const noexcept {
        return this->bb == UINT64_C(0);
    }
    inline Square BitBoard::first() const noexcept {
        return *BitBoardBitScanIterator(this);
    }
    inline auto BitBoard::begin() const noexcept {
        return BitBoardBitScanIterator(this);
    }
    inline auto BitBoard::end() const noexcept {
        return BitBoardBitScanIterator::END;
    }
    inline auto BitBoard::subsets() const noexcept {
        return BitBoardSubsets(this);
    }
    constexpr BitBoard BitBoard::shift_N() const noexcept {
        return *this << 8;
    }
    constexpr BitBoard BitBoard::shift_N(int amount) const noexcept {
        return *this << (8 * amount);
    }
    constexpr BitBoard BitBoard::shift_S() const noexcept {
        return *this >> 8;
    }
    constexpr BitBoard BitBoard::shift_S(int amount) const noexcept {
        return *this >> (8 * amount);
    }
    constexpr BitBoard BitBoard::shift_W() const noexcept {
        return (*this >> 1) & ~BitBoard(File::H);
    }
    constexpr BitBoard BitBoard::shift_W(int amount) const noexcept {
        return (*this >> amount) & ~BitBoard::from_west_files(amount);
    }
    constexpr BitBoard BitBoard::shift_E() const noexcept {
        return (*this << 1) & ~BitBoard(File::A);
    }
    constexpr BitBoard BitBoard::shift_E(int amount) const noexcept {
        return (*this << amount) & ~BitBoard::from_east_files(amount);
    }
    constexpr BitBoard BitBoard::shift_NW() const noexcept {
        return (*this << 7) & ~BitBoard(File::H);
    }
    constexpr BitBoard BitBoard::shift_NE() const noexcept {
        return (*this << 9) & ~BitBoard(File::A);
    }
    constexpr BitBoard BitBoard::shift_SW() const noexcept {
        return (*this >> 9) & ~BitBoard(File::H);
    }
    constexpr BitBoard BitBoard::shift_SE() const noexcept {
        return (*this >> 7) & ~BitBoard(File::A);
    }
    constexpr BitBoard BitBoard::orient(Colour c) const noexcept {
        return c == Colour::WHITE ? *this : this->flip_vertical();
    }
    constexpr BitBoard BitBoard::shift_N(Colour c) const noexcept {
        return c == Colour::WHITE ? this->shift_N() : this->shift_S();
    }
    constexpr BitBoard BitBoard::shift_N(Colour c, int amount) const noexcept {
        return c == Colour::WHITE ? this->shift_N(amount) : this->shift_S(amount);
    }
    constexpr BitBoard BitBoard::shift_S(Colour c) const noexcept {
        return c == Colour::WHITE ? this->shift_S() : this->shift_N();
    }
    constexpr BitBoard BitBoard::shift_S(Colour c, int amount) const noexcept {
        return c == Colour::WHITE ? this->shift_S(amount) : this->shift_N(amount);
    }
    constexpr BitBoard BitBoard::shift_NW(Colour c) const noexcept {
        return c == Colour::WHITE ? this->shift_NW() : this->shift_SW();
    }
    constexpr BitBoard BitBoard::shift_NE(Colour c) const noexcept {
        return c == Colour::WHITE ? this->shift_NE() : this->shift_SE();
    }
    constexpr BitBoard BitBoard::shift_SW(Colour c) const noexcept {
        return c == Colour::WHITE ? this->shift_SW() : this->shift_NW();
    }
    constexpr BitBoard BitBoard::shift_SE(Colour c) const noexcept {
        return c == Colour::WHITE ? this->shift_SE() : this->shift_NE();
    }
    inline BitBoard BitBoard::flip_vertical() const noexcept {
        return utils::byteswap(this->bb);
    }
    inline size_t BitBoard::pop_count() const noexcept {
        return std::bitset<64>(this->bb).count();
    }
    constexpr BitBoard BitBoard::fill_N_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        gen |= empty & (gen << 8);
        empty &= (empty << 8);
        gen |= empty & (gen << 16);
        empty &= (empty << 16);
        gen |= empty & (gen << 32);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_S_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        gen |= empty & (gen >> 8);
        empty &= (empty >> 8);
        gen |= empty & (gen >> 16);
        empty &= (empty >> 16);
        gen |= empty & (gen >> 32);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_E_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        empty &= ~BitBoard(File::A);
        gen |= empty & (gen << 1);
        empty &= (empty << 1);
        gen |= empty & (gen << 2);
        empty &= (empty << 2);
        gen |= empty & (gen << 4);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_W_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        empty &= ~BitBoard(File::H);
        gen |= empty & (gen >> 1);
        empty &= (empty >> 1);
        gen |= empty & (gen >> 2);
        empty &= (empty >> 2);
        gen |= empty & (gen >> 4);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_NE_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        empty &= ~BitBoard(File::A);
        gen |= empty & (gen << 9);
        empty &= (empty << 9);
        gen |= empty & (gen << 18);
        empty &= (empty << 18);
        gen |= empty & (gen << 36);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_SE_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        empty &= ~BitBoard(File::A);
        gen |= empty & (gen >> 7);
        empty &= (empty >> 7);
        gen |= empty & (gen >> 14);
        empty &= (empty >> 14);
        gen |= empty & (gen >> 28);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_NW_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        empty &= ~BitBoard(File::H);
        gen |= empty & (gen << 7);
        empty &= (empty << 7);
        gen |= empty & (gen << 14);
        empty &= (empty << 14);
        gen |= empty & (gen << 28);
        return gen;
    }
    constexpr BitBoard BitBoard::fill_SW_occluded(BitBoard empty) const noexcept {
        BitBoard gen = *this;
        empty &= ~BitBoard(File::H);
        gen |= empty & (gen >> 9);
        empty &= (empty >> 9);
        gen |= empty & (gen >> 18);
        empty &= (empty >> 18);
        gen |= empty & (gen >> 36);
        return gen;
    }
    constexpr BitBoard BitBoard::attack_N_occluded(BitBoard empty) const noexcept {
        return fill_N_occluded(empty).shift_N();
    }
    constexpr BitBoard BitBoard::attack_S_occluded(BitBoard empty) const noexcept {
        return fill_S_occluded(empty).shift_S();
    }
    constexpr BitBoard BitBoard::attack_E_occluded(BitBoard empty) const noexcept {
        return fill_E_occluded(empty).shift_E();
    }
    constexpr BitBoard BitBoard::attack_W_occluded(BitBoard empty) const noexcept {
        return fill_W_occluded(empty).shift_W();
    }
    constexpr BitBoard BitBoard::attack_NE_occluded(BitBoard empty) const noexcept {
        return fill_NE_occluded(empty).shift_NE();
    }
    constexpr BitBoard BitBoard::attack_SE_occluded(BitBoard empty) const noexcept {
        return fill_SE_occluded(empty).shift_SE();
    }
    constexpr BitBoard BitBoard::attack_NW_occluded(BitBoard empty) const noexcept {
        return fill_NW_occluded(empty).shift_NW();
    }
    constexpr BitBoard BitBoard::attack_SW_occluded(BitBoard empty) const noexcept {
        return fill_SW_occluded(empty).shift_SW();
    }

    constexpr bool BitBoard::operator == (BitBoard rhs) const noexcept {
        return this->bb == rhs.bb;
    }
    constexpr BitBoard& BitBoard::operator |= (BitBoard rhs) noexcept {
        this->bb |= rhs.bb;
        return *this;
    }
    constexpr BitBoard& BitBoard::operator &= (BitBoard rhs) noexcept {
        this->bb &= rhs.bb;
        return *this;
    }
    constexpr BitBoard& BitBoard::operator ^= (BitBoard rhs) noexcept {
        this->bb ^= rhs.bb;
        return *this;
    }
    constexpr BitBoard BitBoard::operator | (BitBoard rhs) const noexcept {
        return this->bb | rhs.bb;
    }
    constexpr BitBoard BitBoard::operator & (BitBoard rhs) const noexcept {
        return this->bb & rhs.bb;
    }
    constexpr BitBoard BitBoard::operator ^ (BitBoard rhs) const noexcept {
        return this->bb ^ rhs.bb;
    }
    constexpr BitBoard BitBoard::operator << (unsigned int rhs) const noexcept {
        return this->bb << rhs;
    }
    constexpr BitBoard BitBoard::operator >> (unsigned int rhs) const noexcept {
        return this->bb >> rhs;
    }
    constexpr BitBoard BitBoard::operator ~ () const noexcept {
        return ~this->bb;
    }
    constexpr BitBoard BitBoard::operator - (BitBoard rhs) const noexcept {
        return this->bb - rhs.bb;
    }

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


    BitBoardBitScanIterator::BitBoardBitScanIterator(const BitBoard* bb) noexcept
        : bb(bb), square(bitscan::first(bb->bb)) {}

    BitBoardBitScanIterator& BitBoardBitScanIterator::operator ++ () noexcept {
        this->square = bitscan::next(this->square.get(), this->bb->bb);
        return *this;
    }
    bool BitBoardBitScanIterator::operator != (unsigned int rhs) const noexcept {
        return this->square.get() != rhs;
    }
    Square BitBoardBitScanIterator::operator * () const noexcept {
        return this->square;
    }


	BitBoardSubsets::BitBoardSubsets(const BitBoard* bb) noexcept
	    : bb(bb) {}
	BitBoardSubsetsIterator BitBoardSubsets::begin() const noexcept {
	    return BitBoardSubsetsIterator(this->bb);
	}
	BitBoardSubsets::END BitBoardSubsets::end() const noexcept {
	    return END{};
	}
	BitBoardSubsetsIterator::BitBoardSubsetsIterator(const BitBoard* bb) noexcept
	    : bb(bb), n(0), i(0) {}
	BitBoardSubsetsIterator& BitBoardSubsetsIterator::operator ++ () noexcept {
	    this->n = (this->n - *this->bb) & *this->bb;
	    this->i ++;
	    return *this;
	}
	bool BitBoardSubsetsIterator::operator != (BitBoardSubsets::END) const noexcept {
	    return this->i == 0 || !this->n.empty();
	}
	BitBoard BitBoardSubsetsIterator::operator * () const noexcept {
	    return this->n;
	}
}


#endif