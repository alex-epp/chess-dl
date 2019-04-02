#ifndef BITBOARD_H
#define BITBOARD_H


#include <cstdint>


namespace chess {
	namespace BB {
		using BitBoard = std::uint64_t;

		BitBoard constexpr rank(const int r) {
			return UINT64_C(0xFF) << 8 * r;
		}
		BitBoard constexpr file(const int f) {
			return (UINT64_C(0x0101010101010101) << f);
		}
		BitBoard constexpr from_file_rank(const int f, const int r) {
			return UINT64_C(0x01) << (f + (r << 3));
		}

		BitBoard constexpr west_files(int num_files) {
			BitBoard files = 0;
			for (auto f = 7; num_files > 0; --f, --num_files)
				files |= file(f);
			return files;
		}
		BitBoard constexpr east_files(int num_files) {
			BitBoard files = 0;
			for (auto f = 0; num_files > 0; ++f, --num_files)
				files |= file(f);
			return files;
		}

		template <typename T>
		T& print(T& stream, std::uint64_t BB) {
			for (int r = 7; r >= 0; --r) {
				for (int f = 0; f < 8; ++f) {
					stream << ((BB & BB::rank(r) & BB::file(f)) ? "1 " : ". ");
				}
				stream << std::endl;
			}
			return stream;
		}

		constexpr std::uint64_t R1 = rank(0), R2 = rank(1), R3 = rank(2), R4 = rank(3), R5 = rank(4), R6 = rank(5), R7 = rank(6), R8 = rank(7);
		constexpr std::uint64_t A = file(0), B = file(1), C = file(2), D = file(3), E = file(4), F = file(5), G = file(6), H = file(7);

		constexpr std::uint64_t A1 = from_file_rank(0, 0), A2 = from_file_rank(0, 1), A3 = from_file_rank(0, 2), A4 = from_file_rank(0, 3), A5 = from_file_rank(0, 4), A6 = from_file_rank(0, 5), A7 = from_file_rank(0, 6), A8 = from_file_rank(0, 7);
		constexpr std::uint64_t B1 = from_file_rank(1, 0), B2 = from_file_rank(1, 1), B3 = from_file_rank(1, 2), B4 = from_file_rank(1, 3), B5 = from_file_rank(1, 4), B6 = from_file_rank(1, 5), B7 = from_file_rank(1, 6), B8 = from_file_rank(1, 7);
		constexpr std::uint64_t C1 = from_file_rank(2, 0), C2 = from_file_rank(2, 1), C3 = from_file_rank(2, 2), C4 = from_file_rank(2, 3), C5 = from_file_rank(2, 4), C6 = from_file_rank(2, 5), C7 = from_file_rank(2, 6), C8 = from_file_rank(2, 7);
		constexpr std::uint64_t D1 = from_file_rank(3, 0), D2 = from_file_rank(3, 1), D3 = from_file_rank(3, 2), D4 = from_file_rank(3, 3), D5 = from_file_rank(3, 4), D6 = from_file_rank(3, 5), D7 = from_file_rank(3, 6), D8 = from_file_rank(3, 7);
		constexpr std::uint64_t E1 = from_file_rank(4, 0), E2 = from_file_rank(4, 1), E3 = from_file_rank(4, 2), E4 = from_file_rank(4, 3), E5 = from_file_rank(4, 4), E6 = from_file_rank(4, 5), E7 = from_file_rank(4, 6), E8 = from_file_rank(4, 7);
		constexpr std::uint64_t F1 = from_file_rank(5, 0), F2 = from_file_rank(5, 1), F3 = from_file_rank(5, 2), F4 = from_file_rank(5, 3), F5 = from_file_rank(5, 4), F6 = from_file_rank(5, 5), F7 = from_file_rank(5, 6), F8 = from_file_rank(5, 7);
		constexpr std::uint64_t G1 = from_file_rank(6, 0), G2 = from_file_rank(6, 1), G3 = from_file_rank(6, 2), G4 = from_file_rank(6, 3), G5 = from_file_rank(6, 4), G6 = from_file_rank(6, 5), G7 = from_file_rank(6, 6), G8 = from_file_rank(6, 7);
		constexpr std::uint64_t H1 = from_file_rank(7, 0), H2 = from_file_rank(7, 1), H3 = from_file_rank(7, 2), H4 = from_file_rank(7, 3), H5 = from_file_rank(7, 4), H6 = from_file_rank(7, 5), H7 = from_file_rank(7, 6), H8 = from_file_rank(7, 7);

		constexpr BitBoard shift_N(BitBoard bb) {
			return bb << 8;
		}
		constexpr BitBoard shift_N(BitBoard bb, int amount) {
			return bb << (8 * amount);
		}
		constexpr BitBoard shift_S(BitBoard bb) {
			return bb >> 8;
		}
		constexpr BitBoard shift_S(BitBoard bb, int amount) {
			return bb >> (8 * amount);
		}
		constexpr BitBoard shift_W(BitBoard bb) {
			return (bb >> 1) & ~H;
		}
		constexpr BitBoard shift_W(BitBoard bb, int amount) {
			return (bb >> amount) & ~west_files(amount);
		}
		constexpr BitBoard shift_E(BitBoard bb) {
			return (bb << 1) & ~A;
		}
		constexpr BitBoard shift_E(BitBoard bb, int amount) {
			return (bb << amount) & ~east_files(amount);
		}
		constexpr BitBoard shift_NW(BitBoard bb) {
			return (bb << 7) & ~H;
		}
		constexpr BitBoard shift_NE(BitBoard bb) {
			return (bb << 9) & ~A;
		}
		constexpr BitBoard shift_SW(BitBoard bb) {
			return (bb >> 9) & ~H;
		}
		constexpr BitBoard shift_SE(BitBoard bb) {
			return (bb >> 7) & ~A;
		}

		constexpr BitBoard fill_N_occluded(BitBoard gen, BitBoard empty) {
			gen |= empty & (gen << 8);
			empty &= (empty << 8);
			gen |= empty & (gen << 16);
			empty &= (empty << 16);
			gen |= empty & (gen << 32);
			return gen;
		}
		constexpr BitBoard fill_S_occluded(BitBoard gen, BitBoard empty) {
			gen |= empty & (gen >> 8);
			empty &= (empty >> 8);
			gen |= empty & (gen >> 16);
			empty &= (empty >> 16);
			gen |= empty & (gen >> 32);
			return gen;
		}
		constexpr BitBoard fill_E_occluded(BitBoard gen, BitBoard empty) {
			empty &= ~A;
			gen |= empty & (gen << 1);
			empty &= (empty << 1);
			gen |= empty & (gen << 2);
			empty &= (empty << 2);
			gen |= empty & (gen << 4);
			return gen;
		}
		constexpr BitBoard fill_W_occluded(BitBoard gen, BitBoard empty) {
			empty &= ~H;
			gen |= empty & (gen >> 1);
			empty &= (empty >> 1);
			gen |= empty & (gen >> 2);
			empty &= (empty >> 2);
			gen |= empty & (gen >> 4);
			return gen;
		}
		constexpr BitBoard fill_NE_occluded(BitBoard gen, BitBoard empty) {
			empty &= ~A;
			gen |= empty & (gen << 9);
			empty &= (empty << 9);
			gen |= empty & (gen << 18);
			empty &= (empty << 18);
			gen |= empty & (gen << 36);
			return gen;
		}
		constexpr BitBoard fill_SE_occluded(BitBoard gen, BitBoard empty) {
			empty &= ~A;
			gen |= empty & (gen >> 7);
			empty &= (empty >> 7);
			gen |= empty & (gen >> 14);
			empty &= (empty >> 14);
			gen |= empty & (gen >> 28);
			return gen;
		}
		constexpr BitBoard fill_NW_occluded(BitBoard gen, BitBoard empty) {
			empty &= ~H;
			gen |= empty & (gen >> 9);
			empty &= (empty >> 9);
			gen |= empty & (gen >> 18);
			empty &= (empty >> 18);
			gen |= empty & (gen >> 36);
			return gen;
		}
		constexpr BitBoard fill_SW_occluded(BitBoard gen, BitBoard empty) {
			empty &= ~H;
			gen |= empty & (gen << 7);
			empty &= (empty << 7);
			gen |= empty & (gen << 14);
			empty &= (empty << 14);
			gen |= empty & (gen << 28);
			return gen;
		}
	}
}


#endif