#ifndef CHESS_H
#define CHESS_H

#include "bitscan.h"

#include <cstdint>
#include <string>
#include <iostream>
#include <vector>

namespace chess {

	namespace BB {
		std::uint64_t constexpr rank(const int r) {
			return UINT64_C(0xFF) << 8 * r;
		}
		std::uint64_t constexpr file(const int f) {
			return (UINT64_C(0x0101010101010101) << f);
		}
		std::uint64_t constexpr from_file_rank(const int f, const int r) {
			return UINT64_C(0x01) << (f + (r << 3));
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
	}

	class Piece {
	public:
		enum Colour {
			WHITE = 0,
			BLACK,
		};
		enum Type {
			PAWN = 2,
			KNIGHT,
			BISHOP,
			ROOK,
			QUEEN,
			KING,
			NONE
		};

		static Colour enemy_colour(Colour c) {
			return static_cast<Colour>(Colour::BLACK - c);
		}

		Piece() : m_type(NONE), m_colour(WHITE) {}
		Piece(Type t, Colour c) : m_type(t), m_colour(c) {}
		Piece(char name) {
			this->m_colour = ::isupper(name) ? WHITE : BLACK;
			switch (::tolower(name)) {
			case 'p': this->m_type = PAWN; break;
			case 'n': this->m_type = KNIGHT; break;
			case 'b': this->m_type = BISHOP; break;
			case 'r': this->m_type = ROOK; break;
			case 'q': this->m_type = QUEEN; break;
			case 'k': this->m_type = KING; break;
			default: this->m_type = NONE; break;
			}
		}

		auto type() { return this->m_type; }
		auto colour() { return this->m_colour; }

		void clear() { this->m_type = NONE; }

	private:
		Type m_type;
		Colour m_colour;
	};

	namespace PosIndex {
		constexpr std::uint64_t to_BB_mask(unsigned int position) {
			return UINT64_C(1) << position;
		}
		constexpr unsigned int from_file_rank(const int file, const int rank) {
			return file + (rank << 3);
		}
		inline unsigned int from_uci(std::string uci) {
			return from_file_rank(::tolower(uci[0]) - 'a', uci[1] - '1');
		}

		enum Direction {
			NORTH = 0, NORTHEAST, EAST, SOUTHEAST, SOUTH, SOUTHWEST, WEST, NORTHWEST
		};

		const int COMPASS_ROSE[9] = { 8, 9, 1, -7, -8, -9, -2, 7 };

		constexpr unsigned int forward(unsigned int position, Piece::Colour colour) {
			auto dir = (colour == Piece::WHITE) ? 1 : -1;
			return position + COMPASS_ROSE[NORTH] * dir;
		}
		constexpr unsigned int backward(unsigned int position, Piece::Colour colour) {
			auto dir = (colour == Piece::WHITE) ? 1 : -1;
			return position + COMPASS_ROSE[SOUTH] * dir;
		}

		enum {
			A1, B1, C1, D1, E1, F1, G1, H1,
			A2, B2, C2, D2, E2, F2, G2, H2,
			A3, B3, C3, D3, E3, F3, G3, H3,
			A4, B4, C4, D4, E4, F4, G4, H4,
			A5, B5, C5, D5, E5, F5, G5, H5,
			A6, B6, C6, D6, E6, F6, G6, H6,
			A7, B7, C7, D7, E7, F7, G7, H7,
			A8, B8, C8, D8, E8, F8, G8, H8
		};
	}

	class Move {
	public:
		static const unsigned int QUIET = 0b0000;
		static const unsigned int D_P_PUSH = 0b0001;
		static const unsigned int K_CASTLE = 0b0010;
		static const unsigned int Q_CASTLE = 0b0011;
		static const unsigned int CAPTURE = 0b0100;
		static const unsigned int EN_CAPTURE = 0b0101;
		static const unsigned int PROMOTION = 0b1000;
		static const unsigned int N_PROMOTION = 0b1000;
		static const unsigned int B_PROMOTION = 0b1001;
		static const unsigned int R_PROMOTION = 0b1010;
		static const unsigned int Q_PROMOTION = 0b1011;
		static const unsigned int N_PROMO_CAPTURE = 0b1100;
		static const unsigned int B_PROMO_CAPTURE = 0b1101;
		static const unsigned int R_PROMO_CAPTURE = 0b1110;
		static const unsigned int Q_PROMO_CAPTURE = 0b1111;

		Move(unsigned int f, unsigned int t, unsigned int flags = QUIET, Piece::Type p = Piece::NONE) {
			switch (p) {
			case Piece::KNIGHT: flags |= N_PROMOTION; break;
			case Piece::BISHOP: flags |= B_PROMOTION; break;
			case Piece::ROOK: flags |= R_PROMOTION; break;
			case Piece::QUEEN: flags |= Q_PROMOTION; break;
			}
			this->m_data = ((flags & 0xF) << 12) | ((f & 0x3F) << 6) | (t & 0x3F);
		}
		auto to() { return this->m_data & 0x3F; }
		auto from() { return (this->m_data >> 6) & 0x3F; }
		auto flags() { return (this->m_data >> 12) & 0x3F; }

		bool is_quiet() { return this->flags() == QUIET; }
		bool is_promotion() { return this->flags() & PROMOTION; }
		bool is_capture() { return this->flags() & CAPTURE; }
		bool is_en_capture() { return this->flags() == EN_CAPTURE; }
		bool is_double_pawn_push() { return this->flags() == D_P_PUSH; }
		bool is_castle() { return this->is_king_castle() || this->is_queen_castle(); }
		Piece::Type promoted_type() { return PROMOTION_TYPES[this->flags() & 0b11]; }

		bool is_king_castle() { return this->flags() == K_CASTLE; }
		bool is_queen_castle() { return this->flags() == Q_CASTLE; }

	private:
		unsigned int m_data;
		static const Piece::Type PROMOTION_TYPES[4];
	};

	class BaseBoard {
	public:
		BaseBoard(std::string fen = STARTING_FEN) {
			this->load_FEN(fen);
		}

		void load_FEN(std::string fen) {
			this->clear();

			int file = 0, rank = 7;
			for (auto c : fen) {
				if (c == ' ') {
					break;
				}
				if (c == '/') {
					--rank;
					file = 0;
				}
				else if (::isdigit(c)) {
					file += c - '0';
				}
				else {
					this->put_piece(Piece(c), PosIndex::from_file_rank(file, rank));
					++file;
				}
			}
		}
		void clear() {
			for (int i = 0; i < 8; ++i)
				this->piece_BB[i] = 0;
			for (int i = 0; i < 64; ++i)
				this->piece_mailbox[i].clear();
		}

		auto pieces() const {
			return this->piece_BB[Piece::WHITE] | this->piece_BB[Piece::BLACK];
		}
		auto pieces(Piece::Colour c) const {
			return this->piece_BB[c];
		}
		auto pieces(Piece::Type p) const {
			return this->piece_BB[p];
		}
		auto pieces(Piece::Type p, Piece::Colour c) const {
			return this->piece_BB[p] & this->piece_BB[c];
		}

		auto pawns() const {
			return this->pieces(Piece::PAWN);
		}
		auto pawns(Piece::Colour c) const {
			return this->pieces(Piece::PAWN, c);
		}
		auto knights() const {
			return this->pieces(Piece::KNIGHT);
		}
		auto knights(Piece::Colour c) const {
			return this->pieces(Piece::KNIGHT, c);
		}
		auto bishops() const {
			return this->pieces(Piece::BISHOP);
		}
		auto bishops(Piece::Colour c) const {
			return this->pieces(Piece::BISHOP, c);
		}
		auto rooks() const {
			return this->pieces(Piece::ROOK);
		}
		auto rooks(Piece::Colour c) const {
			return this->pieces(Piece::ROOK, c);
		}
		auto queens() const {
			return this->pieces(Piece::QUEEN);
		}
		auto queens(Piece::Colour c) const {
			return this->pieces(Piece::QUEEN, c);
		}
		auto kings() const {
			return this->pieces(Piece::KING);
		}
		auto kings(Piece::Colour c) const {
			return this->pieces(Piece::KING, c);
		}

		auto is_piece_at(unsigned int position) const {
			return (PosIndex::to_BB_mask(position) & this->pieces()) != 0;
		}
		auto get_piece_at(unsigned int position) const {
			return this->piece_mailbox[position];
		}

		void put_piece(Piece piece, unsigned int position) {
			this->remove_piece(position);
			this->piece_BB[piece.type()] |= PosIndex::to_BB_mask(position);
			this->piece_BB[piece.colour()] |= PosIndex::to_BB_mask(position);
			this->piece_mailbox[position] = piece;
		}
		void remove_piece(unsigned int position) {
			for (int i = 0; i < 8; ++i)
				this->piece_BB[i] &= ~PosIndex::to_BB_mask(position);
			this->piece_mailbox[position].clear();
		}

		template <typename T>
		T& print(T& stream) const {
			for (int r = 7; r >= 0; --r) {
				for (int f = 0; f < 8; ++f) {
					if (this->pawns(Piece::WHITE) & BB::from_file_rank(f, r))
						stream << L"\u2659";
					else if (this->knights(Piece::WHITE) & BB::from_file_rank(f, r))
						stream << L"\u2658";
					else if (this->bishops(Piece::WHITE) & BB::from_file_rank(f, r))
						stream << L"\u2657";
					else if (this->rooks(Piece::WHITE) & BB::from_file_rank(f, r))
						stream << L"\u2656";
					else if (this->queens(Piece::WHITE) & BB::from_file_rank(f, r))
						stream << L"\u2655";
					else if (this->kings(Piece::WHITE) & BB::from_file_rank(f, r))
						stream << L"\u2654";

					else if (this->pawns(Piece::BLACK) & BB::from_file_rank(f, r))
						stream << L"\u265F";
					else if (this->knights(Piece::BLACK) & BB::from_file_rank(f, r))
						stream << L"\u265E";
					else if (this->bishops(Piece::BLACK) & BB::from_file_rank(f, r))
						stream << L"\u265D";
					else if (this->rooks(Piece::BLACK) & BB::from_file_rank(f, r))
						stream << L"\u265C";
					else if (this->queens(Piece::BLACK) & BB::from_file_rank(f, r))
						stream << L"\u265B";
					else if (this->kings(Piece::BLACK) & BB::from_file_rank(f, r))
						stream << L"\u265A";

					else
						stream << L"\u2610";
				}
				stream << std::endl;
			}
			return stream;
		}

		static const std::string STARTING_FEN;
	protected:
		// Bitboards stored with little-endian-rank-file-mapping
		std::uint64_t piece_BB[8];

		// Mailbox stored with least-significant-file-mapping
		Piece piece_mailbox[64];
	};
	inline std::wostream& operator << (std::wostream& stream, const BaseBoard& board) {
		return board.print(stream);
	}

	/*
	 * BoardBoard with move generation added. Uses mirroring so that internally it always moves as if it's white.
	 */
	class Board : public BaseBoard {
	public:
		Board(std::string fen = STARTING_FEN) : BaseBoard(fen) {}

		void push_move(std::string uci) {
			auto from = PosIndex::from_uci(uci.substr(0, 2));
			auto to = PosIndex::from_uci(uci.substr(2, 2));
			auto promo_type = (uci.length() > 4) ? Piece(uci[4]).type() : Piece::NONE;
			this->push_move(from, to, promo_type);
		}
		void push_move(unsigned int from, unsigned int to, Piece::Type promo_type = Piece::NONE) {
			auto piece = this->piece_mailbox[from];
			auto flags = Move::QUIET;

			if (this->piece_mailbox[to].type() != Piece::NONE) {
				flags |= Move::CAPTURE;
			}



			this->push_move(Move(from, to, flags));
		}
		void push_move(Move move) {
			auto from_BB = PosIndex::to_BB_mask(move.from());
			auto to_BB = PosIndex::to_BB_mask(move.to());
			auto from_to_BB = from_BB ^ to_BB;
			auto piece = this->piece_mailbox[move.from()];

			auto captured_index = move.is_en_capture() ? PosIndex::backward(move.to(), piece.colour()) : move.to();
			auto captured_BB = PosIndex::to_BB_mask(captured_index);
			auto captured_piece = this->piece_mailbox[captured_index];

			this->piece_mailbox[move.from()].clear();
			this->piece_mailbox[move.to()] = piece;
			this->piece_BB[piece.type()] ^= from_to_BB;
			this->piece_BB[piece.colour()] ^= from_to_BB;

			if (move.is_en_capture())
				this->piece_mailbox[captured_index].clear();

			if (move.is_promotion()) {
				this->piece_mailbox[move.to()] = Piece(move.promoted_type(), piece.colour());
				this->piece_BB[piece.type()] ^= to_BB;
				this->piece_BB[move.promoted_type()] ^= to_BB;
			}

			if (move.is_capture()) {
				this->piece_BB[captured_piece.type()] ^= to_BB;
				this->piece_BB[captured_piece.colour()] ^= to_BB;
			}

			if (move.is_castle()) {
				// Castling not yet implemented
			}

			/*if (move.is_quiet() || move.is_double_pawn_push()) {
				this->piece_mailbox[move.from()].clear();
				this->piece_mailbox[move.to()] = piece;
				this->piece_BB[piece.type()] ^= from_to_BB;
				this->piece_BB[piece.colour()] ^= from_to_BB;
			}
			else if (move.is_en_capture()) {
				auto captured_index = PosIndex::backward(move.to(), piece.colour());
				auto captured_piece = this->piece_mailbox[captured_index];
				auto captured_BB = PosIndex::to_BB_mask(captured_index);
				this->piece_mailbox[move.from()].clear();
				this->piece_mailbox[captured_index].clear();
				this->piece_mailbox[move.to()] = piece;
				this->piece_BB[piece.type()] ^= from_to_BB;
				this->piece_BB[piece.colour()] ^= from_to_BB;
				this->piece_BB[captured_piece.type()] ^= captured_BB;
				this->piece_BB[captured_piece.colour()] ^= captured_BB;
			}
			else if (move.is_capture() && !move.is_promotion()) {
				auto captured_piece = this->piece_mailbox[move.to()];
				this->piece_mailbox[move.from()].clear();
				this->piece_mailbox[move.to()] = piece;
				this->piece_BB[piece.type()] ^= from_to_BB;
				this->piece_BB[piece.colour()] ^= from_to_BB;
				this->piece_BB[captured_piece.type()] ^= to_BB;
				this->piece_BB[captured_piece.colour()] ^= to_BB;
			}
			else if (!move.is_capture() && move.is_promotion()) {
				auto promoted_type = move.promoted_type();
				this->piece_mailbox[move.from()].clear();
				this->piece_mailbox[move.to()] = Piece(promoted_type, piece.colour());
				this->piece_BB[piece.type()] ^= from_to_BB;
				this->piece_BB[promoted_type] ^= to_BB;
				this->piece_BB[piece.colour()] ^= from_to_BB;
			}
			else if (move.is_capture() && move.is_promotion()) {
				auto captured_piece = this->piece_mailbox[move.to()];
				auto promoted_type = move.promoted_type();
				this->piece_mailbox[move.from()].clear();
				this->piece_mailbox[move.to()] = Piece(promoted_type, piece.colour());
				this->piece_BB[piece.type()] ^= from_BB;
				this->piece_BB[promoted_type] ^= to_BB;
				this->piece_BB[piece.colour()] ^= from_to_BB;
				this->piece_BB[captured_piece.type()] ^= to_BB;
				this->piece_BB[captured_piece.colour()] ^= to_BB;
			}
			else if (move.is_castle()) {
				auto castle_piece = this->piece_mailbox[move.castle_target()];
				auto castle_BB = PosIndex::to_BB_mask(move.castle_target());
				this->piece_mailbox[move.from()] = castle_piece;
				this->piece_mailbox[move.to()] = piece;
				this->piece_mailbox[move.castle_target()].clear();
				this->piece_BB[piece.type()] ^= from_to_BB;
				this->piece_BB[castle_piece.type()] ^= (castle_BB | from_BB);
				this->piece_BB[piece.colour()] ^= castle_BB;
			}*/
		}

	private:
		void get_pawn_moves(std::vector<Move>& moves) {
			auto left_capture_BB = this->pawns(Piece::WHITE) & BB::shift_right_back(this->pieces(Piece::BLACK));
			auto right_capture_BB = this->pawns(Piece::WHITE) & BB::shift_left_back(this->pieces(Piece::BLACK));
			auto push_BB = this->pawns(Piece::WHITE) & ~BB::shift_back(this->pieces());
			auto double_push_BB = push_BB & BB::R2 && ~BB::shift_back(this->pieces(), 2);

			BITSCAN_FOREACH(left_capture_BB, from_position) {
				auto to_position = from_position + PosIndex::COMPASS_ROSE[PosIndex::Direction::NORTHWEST];
				this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::CAPTURE);
			}

			BITSCAN_FOREACH(right_capture_BB, from_position) {
				auto to_position = from_position + PosIndex::COMPASS_ROSE[PosIndex::Direction::NORTHEAST];
				this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::CAPTURE);
			}

			BITSCAN_FOREACH(push_BB, from_position) {
				auto to_position = from_position + PosIndex::COMPASS_ROSE[PosIndex::Direction::NORTH];
				this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::QUIET);
			}

			BITSCAN_FOREACH(push_BB, from_position) {
				auto to_position = from_position + PosIndex::COMPASS_ROSE[PosIndex::Direction::NORTH] * 2;
				this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::D_P_PUSH);
			}
		}

		void add_pseudo_legal_pawn_move(std::vector<Move>& moves, unsigned int from, unsigned int to, unsigned int flags) {
			if (!this->is_move_pinned(from, to)) {
				if (PosIndex::rank(to) == 8) {
					moves.emplace_back(from, to, flags | Move::N_PROMOTION);
					moves.emplace_back(from, to, flags | Move::B_PROMOTION);
					moves.emplace_back(from, to, flags | Move::R_PROMOTION);
					moves.emplace_back(from, to, flags | Move::Q_PROMOTION);
				}
				else {
					moves.emplace_back(from, to, flags);
				}
			}
		}
	};
}

#endif