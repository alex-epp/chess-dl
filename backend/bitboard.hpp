#ifndef BITBOARD_H
#define BITBOARD_H


#include "bitscan.hpp"
#include "position_index.hpp"
#include "coord.hpp"

#include <cstdint>
#include <cassert>
#include <iostream>

namespace chess {
	class BitBoard;

	class BitBoardBitScanIterator {
	public:
		constexpr static unsigned int END = bitscan::END;

	public:
		BitBoardBitScanIterator(const BitBoard*);
		BitBoardBitScanIterator& operator ++ ();
		bool operator != (const unsigned int) const;
		PositionIndex operator * () const;

	private:
		const BitBoard* bb;
		PositionIndex position;
	};

	/*
	Stores a board position represented as a bitboard
	*/
	class BitBoard {
	public: // Constructor from uint64
		constexpr BitBoard(std::uint64_t bb = 0) : bb(bb) {}
		constexpr BitBoard(const PositionIndex position) : BitBoard(UINT64_C(1) << position.get()) {}
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
		constexpr bool is_piece_at(PositionIndex position) const {
			return !(*this & position).empty();
		}
		constexpr bool empty() const {
			return this->bb == UINT64_C(0);
		}

	public: // range-based iteration
		auto begin() const { return BitBoardBitScanIterator(this); }
		auto end() const { return BitBoardBitScanIterator::END; }

	public: // Operations we can perform on bitboards
		constexpr BitBoard shift_N() const {
			return *this << 8;
		}
		constexpr BitBoard shift_N(const int amount) const {
			return *this << (8 * amount);
		}
		constexpr BitBoard shift_S() const {
			return *this >> 8;
		}
		constexpr BitBoard shift_S(const int amount) const {
			return *this >> (8 * amount);
		}
		constexpr BitBoard shift_W() const {
			return (*this >> 1) & ~BitBoard(File::H);
		}
		constexpr BitBoard shift_W(const int amount) const {
			return (*this >> amount) & ~BitBoard::from_west_files(amount);
		}
		constexpr BitBoard shift_E() const {
			return (*this << 1) & ~BitBoard(File::A);
		}
		constexpr BitBoard shift_E(const int amount) const {
			return (*this << amount) & ~BitBoard::from_east_files(amount);
		}
		constexpr BitBoard shift_NW() const {
			return (*this << 7) & ~BitBoard(File::H);
		}
		constexpr BitBoard shift_NE() const {
			return (*this << 9) & ~BitBoard(File::A);
		}
		constexpr BitBoard shift_SW() const {
			return (*this >> 9) & ~BitBoard(File::H);
		}
		constexpr BitBoard shift_SE() const {
			return (*this >> 7) & ~BitBoard(File::A);
		}

		constexpr BitBoard flip_vertical() const {
#ifdef _WIN32
			return _byteswap_uint64(this->bb);
#else
	        return __builtin_bswap64(this->bb);
#endif
		}

		constexpr BitBoard fill_N_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			gen |= empty & (gen << 8);
			empty &= (empty << 8);
			gen |= empty & (gen << 16);
			empty &= (empty << 16);
			gen |= empty & (gen << 32);
			return gen;
		}
		constexpr BitBoard fill_S_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			gen |= empty & (gen >> 8);
			empty &= (empty >> 8);
			gen |= empty & (gen >> 16);
			empty &= (empty >> 16);
			gen |= empty & (gen >> 32);
			return gen;
		}
		constexpr BitBoard fill_E_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::A);
			gen |= empty & (gen << 1);
			empty &= (empty << 1);
			gen |= empty & (gen << 2);
			empty &= (empty << 2);
			gen |= empty & (gen << 4);
			return gen;
		}
		constexpr BitBoard fill_W_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::H);
			gen |= empty & (gen >> 1);
			empty &= (empty >> 1);
			gen |= empty & (gen >> 2);
			empty &= (empty >> 2);
			gen |= empty & (gen >> 4);
			return gen;
		}
		constexpr BitBoard fill_NE_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::A);
			gen |= empty & (gen << 9);
			empty &= (empty << 9);
			gen |= empty & (gen << 18);
			empty &= (empty << 18);
			gen |= empty & (gen << 36);
			return gen;
		}
		constexpr BitBoard fill_SE_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::A);
			gen |= empty & (gen >> 7);
			empty &= (empty >> 7);
			gen |= empty & (gen >> 14);
			empty &= (empty >> 14);
			gen |= empty & (gen >> 28);
			return gen;
		}
		constexpr BitBoard fill_NW_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::H);
			gen |= empty & (gen << 7);
			empty &= (empty << 7);
			gen |= empty & (gen << 14);
			empty &= (empty << 14);
			gen |= empty & (gen << 28);
			return gen;
		}
		constexpr BitBoard fill_SW_occluded(BitBoard empty) const {
			BitBoard gen = *this;
			empty &= ~BitBoard(File::H);
			gen |= empty & (gen >> 9);
			empty &= (empty >> 9);
			gen |= empty & (gen >> 18);
			empty &= (empty >> 18);
			gen |= empty & (gen >> 36);
			return gen;
		}

		constexpr BitBoard attack_N_occluded(BitBoard empty) const {
			return fill_N_occluded(empty).shift_N();
		}
		constexpr BitBoard attack_S_occluded(BitBoard empty) const {
			return fill_S_occluded(empty).shift_S();
		}
		constexpr BitBoard attack_E_occluded(BitBoard empty) const {
			return fill_E_occluded(empty).shift_E();
		}
		constexpr BitBoard attack_W_occluded(BitBoard empty) const {
			return fill_W_occluded(empty).shift_W();
		}
		constexpr BitBoard attack_NE_occluded(BitBoard empty) const {
			return fill_NE_occluded(empty).shift_NE();
		}
		constexpr BitBoard attack_SE_occluded(BitBoard empty) const {
			return fill_SE_occluded(empty).shift_SE();
		}
		constexpr BitBoard attack_NW_occluded(BitBoard empty) const {
			return fill_NW_occluded(empty).shift_NW();
		}
		constexpr BitBoard attack_SW_occluded(BitBoard empty) const {
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

	public:
		std::uint64_t bb;

		friend class BitBoardBitScanIterator;
	};

	/*
	Helpful predefined bitboards
	*/
	namespace BB {
		constexpr auto A1 = BitBoard(PositionIndex("A1")), A2 = BitBoard(PositionIndex("A2")), A3 = BitBoard(PositionIndex("A3")), A4 = BitBoard(PositionIndex("A4")), A5 = BitBoard(PositionIndex("A5")), A6 = BitBoard(PositionIndex("A6")), A7 = BitBoard(PositionIndex("A7")), A8 = BitBoard(PositionIndex("A8"));
		constexpr auto B1 = BitBoard(PositionIndex("B1")), B2 = BitBoard(PositionIndex("B2")), B3 = BitBoard(PositionIndex("B3")), B4 = BitBoard(PositionIndex("B4")), B5 = BitBoard(PositionIndex("B5")), B6 = BitBoard(PositionIndex("B6")), B7 = BitBoard(PositionIndex("B7")), B8 = BitBoard(PositionIndex("B8"));
		constexpr auto C1 = BitBoard(PositionIndex("C1")), C2 = BitBoard(PositionIndex("C2")), C3 = BitBoard(PositionIndex("C3")), C4 = BitBoard(PositionIndex("C4")), C5 = BitBoard(PositionIndex("C5")), C6 = BitBoard(PositionIndex("C6")), C7 = BitBoard(PositionIndex("C7")), C8 = BitBoard(PositionIndex("C8"));
		constexpr auto D1 = BitBoard(PositionIndex("D1")), D2 = BitBoard(PositionIndex("D2")), D3 = BitBoard(PositionIndex("D3")), D4 = BitBoard(PositionIndex("D4")), D5 = BitBoard(PositionIndex("D5")), D6 = BitBoard(PositionIndex("D6")), D7 = BitBoard(PositionIndex("D7")), D8 = BitBoard(PositionIndex("D8"));
		constexpr auto E1 = BitBoard(PositionIndex("E1")), E2 = BitBoard(PositionIndex("E2")), E3 = BitBoard(PositionIndex("E3")), E4 = BitBoard(PositionIndex("E4")), E5 = BitBoard(PositionIndex("E5")), E6 = BitBoard(PositionIndex("E6")), E7 = BitBoard(PositionIndex("E7")), E8 = BitBoard(PositionIndex("E8"));
		constexpr auto F1 = BitBoard(PositionIndex("F1")), F2 = BitBoard(PositionIndex("F2")), F3 = BitBoard(PositionIndex("F3")), F4 = BitBoard(PositionIndex("F4")), F5 = BitBoard(PositionIndex("F5")), F6 = BitBoard(PositionIndex("F6")), F7 = BitBoard(PositionIndex("F7")), F8 = BitBoard(PositionIndex("F8"));
		constexpr auto G1 = BitBoard(PositionIndex("G1")), G2 = BitBoard(PositionIndex("G2")), G3 = BitBoard(PositionIndex("G3")), G4 = BitBoard(PositionIndex("G4")), G5 = BitBoard(PositionIndex("G5")), G6 = BitBoard(PositionIndex("G6")), G7 = BitBoard(PositionIndex("G7")), G8 = BitBoard(PositionIndex("G8"));
		constexpr auto H1 = BitBoard(PositionIndex("H1")), H2 = BitBoard(PositionIndex("H2")), H3 = BitBoard(PositionIndex("H3")), H4 = BitBoard(PositionIndex("H4")), H5 = BitBoard(PositionIndex("H5")), H6 = BitBoard(PositionIndex("H6")), H7 = BitBoard(PositionIndex("H7")), H8 = BitBoard(PositionIndex("H8"));

		constexpr auto R1 = BitBoard(Rank::R1), R2 = BitBoard(Rank::R2), R3 = BitBoard(Rank::R3), R4 = BitBoard(Rank::R4), R5 = BitBoard(Rank::R5), R6 = BitBoard(Rank::R6), R7 = BitBoard(Rank::R7), R8 = BitBoard(Rank::R8);
		constexpr auto A = BitBoard(File::A), B = BitBoard(File::B), C = BitBoard(File::C), D = BitBoard(File::D), E = BitBoard(File::E), F = BitBoard(File::F), G = BitBoard(File::G), H = BitBoard(File::H);
	}

	// TODO: should probably be in its own class
	template <typename T>
	T&& print_bitboard(T&& stream, BitBoard bb) {
		for (int r = 7; r >= 0; --r) {
			for (int f = 0; f < 8; ++f) {
				auto pos = PositionIndex(static_cast<File>(f), static_cast<Rank>(r));
				stream << (bb.is_piece_at(pos) ? '1' : '.') << ' ';
			}
			stream << std::endl;
		}
		return stream;
	}
}


#endif