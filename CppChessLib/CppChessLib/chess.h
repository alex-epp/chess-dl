#ifndef CHESS_H
#define CHESS_H

#include "bitboard.h"
#include "bitscan.h"

#include <sstream>
#include <string>
#include <iostream>
#include <iterator>
#include <vector>

namespace chess {

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
		constexpr BB::BitBoard to_BB_mask(unsigned int position) {
			return UINT64_C(1) << position;
		}
		constexpr unsigned int from_file_rank(const int file, const int rank) {
			return file + (rank << 3);
		}
		inline unsigned int from_uci(std::string uci) {
			return from_file_rank(::tolower(uci[0]) - 'a', uci[1] - '1');
		}

		constexpr unsigned int rank(unsigned int position) {
			return position / 8;
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
		static const unsigned int N_PROMO_CAPTURE = CAPTURE | N_PROMOTION;
		static const unsigned int B_PROMO_CAPTURE = CAPTURE | B_PROMOTION;
		static const unsigned int R_PROMO_CAPTURE = CAPTURE | R_PROMOTION;
		static const unsigned int Q_PROMO_CAPTURE = CAPTURE | Q_PROMOTION;

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

			std::istringstream iss(fen);
			std::vector<std::string> tokens{std::istream_iterator<std::string>{iss},
					                        std::istream_iterator<std::string>{}};
			auto board_fen = tokens[0];
			auto turn_fen = tokens[1];
			auto castle_rights_fen = tokens[2];
			auto en_passant_fen = tokens[3];
			auto half_move_fen = tokens[4];
			auto full_move_fen = tokens[5];

			int file = 0, rank = 7;
			for (auto c : board_fen) {
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

			this->en_passant_target = PosIndex::from_uci(en_passant_fen);
		}
		void clear() {
			for (int i = 0; i < 8; ++i)
				this->piece_BB[i] = 0;
			for (int i = 0; i < 64; ++i)
				this->piece_mailbox[i].clear();
			this->en_passant_target = UINT_MAX;
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

		// Position index behind the pawn that just made a two-square move.
		// If no such pawn exists, set to UINT_MAX
		unsigned int en_passant_target;
	};
	inline std::wostream& operator << (std::wostream& stream, const BaseBoard& board) {
		return board.print(stream);
	}

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

			if (piece.type() == Piece::PAWN) {
				if ((PosIndex::rank(from) == 1 && PosIndex::rank(to) == 3) ||
					(PosIndex::rank(from) == 6 && PosIndex::rank(to) == 4)) {
					flags |= Move::D_P_PUSH;
				}
				else if (to == this->en_passant_target) {
					flags |= Move::EN_CAPTURE;
				}
			}

			if (piece.type() == Piece::KING) {
				if ((from == PosIndex::E1 && to == PosIndex::G1) ||
				    (from == PosIndex::E8 && to == PosIndex::G8)) {
					flags |= Move::K_CASTLE;
				}
				else if ((from == PosIndex::E1 && to == PosIndex::C1) ||
				         (from == PosIndex::E8 && to == PosIndex::C8)) {
					flags |= Move::Q_CASTLE;
				}
			}

			if (this->piece_mailbox[to].type() != Piece::NONE) {
				flags |= Move::CAPTURE;
			}

			this->push_move(Move(from, to, flags, promo_type));
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

			if (move.is_capture()) {
				this->piece_BB[captured_piece.type()] ^= to_BB;
				this->piece_BB[captured_piece.colour()] ^= to_BB;
			}

			if (move.is_promotion()) {
				this->piece_mailbox[move.to()] = Piece(move.promoted_type(), piece.colour());
				this->piece_BB[piece.type()] ^= to_BB;
				this->piece_BB[move.promoted_type()] ^= to_BB;
			}

			if (move.is_castle()) {
				unsigned int rook_from, rook_to;
				if (move.is_king_castle()) {
					rook_from = (piece.colour() == Piece::WHITE) ? PosIndex::H1 : PosIndex::H8;
					rook_to = (piece.colour() == Piece::WHITE) ? PosIndex::F1 : PosIndex::F8;
				}
				else {
					rook_from = (piece.colour() == Piece::WHITE) ? PosIndex::A1 : PosIndex::A8;
					rook_to = (piece.colour() == Piece::WHITE) ? PosIndex::D1 : PosIndex::D8;
				}
				auto rook_from_to_BB = PosIndex::to_BB_mask(rook_from) ^ PosIndex::to_BB_mask(rook_to);
				this->piece_BB[piece.colour()] ^= rook_from_to_BB;
				this->piece_BB[Piece::ROOK] ^= rook_from_to_BB;
				this->piece_mailbox[rook_to] = this->piece_mailbox[rook_from];
				this->piece_mailbox[rook_to].clear();
			}

			if (move.is_double_pawn_push()) {
				this->en_passant_target = PosIndex::backward(move.to(), piece.colour());
			}
		}

	private:
		void get_pawn_moves(std::vector<Move>& moves) {
			auto left_capture_BB = this->pawns(Piece::WHITE) & BB::shift_SW(this->pieces(Piece::BLACK));
			auto right_capture_BB = this->pawns(Piece::WHITE) & BB::shift_SE(this->pieces(Piece::BLACK));
			auto push_BB = this->pawns(Piece::WHITE) & ~BB::shift_S(this->pieces());
			auto double_push_BB = push_BB & BB::R2 && ~BB::shift_S(this->pieces(), 2);

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
				// TODO: pretty sure this should be rank 7
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

		bool is_move_pinned(unsigned int from, unsigned int to) {
			return false;
		}
	};
}

#endif