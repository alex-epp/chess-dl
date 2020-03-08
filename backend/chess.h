#ifndef CHESS_H
#define CHESS_H

#include "bitboard.hpp"
#include "piece.hpp"
#include "mailbox.hpp"
#include "move.hpp"
#include "coord.hpp"

#include <array>
#include <cassert>
#include <string_view>
#include <iostream>
#include <iterator>
#include <regex>
#include <vector>

namespace chess {
    [[nodiscard]] std::string san_flipped(std::string san);

	class BaseBoard;
	std::wostream& print_board(std::wostream&, const BaseBoard&);

	class BaseBoard {
	public:
		BaseBoard(Piece::Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move)
			: en_passant_target(en_passant_target)
		{
			this->can_white_king_castle = castle_rights.find_first_of('K') != std::string::npos;
			this->can_white_queen_castle = castle_rights.find_first_of('Q') != std::string::npos;
			this->can_black_king_castle = castle_rights.find_first_of('k') != std::string::npos;
			this->can_black_queen_castle = castle_rights.find_first_of('q') != std::string::npos;
		}

		void clear() {
			for (int i = 0; i < 8; ++i)
				this->piece_BB[i] = 0;
			this->piece_mailbox.clear();
			this->en_passant_target = Square();

			for(int i = 0; i < 8; ++i)
			    assert(this->piece_BB[i].empty());
			for(int i = 0; i < 64; ++i)
			    assert(this->piece_mailbox.get(i).type() == Piece::NO_TYPE);
			assert(this->en_passant_target == Square::EMPTY);
		}

		[[nodiscard]] auto pieces() const {
			return this->piece_BB[Piece::WHITE] | this->piece_BB[Piece::BLACK];
		}
        [[nodiscard]] auto pieces(Piece::Colour c) const {
			return this->piece_BB[c];
		}
        [[nodiscard]] auto pieces(Piece::Type p) const {
			return this->piece_BB[p];
		}
        [[nodiscard]] auto pieces(Piece::Type p, Piece::Colour c) const {
			return this->piece_BB[p] & this->piece_BB[c];
		}

        [[nodiscard]] auto pawns() const {
			return this->pieces(Piece::PAWN);
		}
        [[nodiscard]] auto pawns(Piece::Colour c) const {
			return this->pieces(Piece::PAWN, c);
		}
        [[nodiscard]] auto knights() const {
			return this->pieces(Piece::KNIGHT);
		}
        [[nodiscard]] auto knights(Piece::Colour c) const {
			return this->pieces(Piece::KNIGHT, c);
		}
        [[nodiscard]] auto bishops() const {
			return this->pieces(Piece::BISHOP);
		}
        [[nodiscard]] auto bishops(Piece::Colour c) const {
			return this->pieces(Piece::BISHOP, c);
		}
        [[nodiscard]] auto rooks() const {
			return this->pieces(Piece::ROOK);
		}
        [[nodiscard]] auto rooks(Piece::Colour c) const {
			return this->pieces(Piece::ROOK, c);
		}
        [[nodiscard]] auto queens() const {
			return this->pieces(Piece::QUEEN);
		}
        [[nodiscard]] auto queens(Piece::Colour c) const {
			return this->pieces(Piece::QUEEN, c);
		}
        [[nodiscard]] auto kings() const {
			return this->pieces(Piece::KING);
		}
        [[nodiscard]] auto kings(Piece::Colour c) const {
			return this->pieces(Piece::KING, c);
		}

        [[nodiscard]] auto is_piece_at(Square square) const {
			return this->piece_mailbox.is_piece_at(square);
		}
        [[nodiscard]] auto get_piece_at(Square square) const {
			return this->piece_mailbox.get(square);
		}

        [[nodiscard]] auto en_target() const {
		    return this->en_passant_target;
		}

        [[nodiscard]] auto white_king_castling_rights() const {
		    return this->can_white_king_castle;
		}

        [[nodiscard]] auto white_queen_castling_rights() const {
            return this->can_white_queen_castle;
        }

        [[nodiscard]] auto black_king_castling_rights() const {
            return this->can_black_king_castle;
        }

        [[nodiscard]] auto black_queen_castling_rights() const {
            return this->can_black_queen_castle;
        }

		void put_piece(Piece piece, Square square) {
			this->remove_piece(square);
			this->piece_BB[piece.type()] |= square;
			this->piece_BB[piece.colour()] |= square;
			this->piece_mailbox.set(square, piece);
		}
		void remove_piece(Square square) {
			for (int i = 0; i < 8; ++i)
				this->piece_BB[i] &= ~BitBoard(square);
			this->piece_mailbox.clear(square);

			for (int i = 0; i < 8; ++i)
			    assert(!this->piece_BB[i].is_piece_at(square));
			assert(!this->piece_mailbox.is_piece_at(square));
		}

	protected:
		// Bitboards stored with little-endian-rank-file-mapping
		std::array<BitBoard, 8> piece_BB;

		// Mailbox stored with least-significant-file-mapping
		Mailbox piece_mailbox;

		// Square behind the pawn that just made a two-square move.
		// If no such pawn exists, set to UINT_MAX
		Square en_passant_target;

		// Castling rights
		bool can_white_king_castle, can_white_queen_castle, can_black_king_castle, can_black_queen_castle;
	};
	inline std::wostream& operator << (std::wostream& stream, const BaseBoard& board) {
		return print_board(stream, board);
	}

	class Board : public BaseBoard {
	public:
		Board(Piece::Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move);

        [[nodiscard]] Move parse_uci(const std::string& uci) const;
        [[nodiscard]] Move parse_san(std::string san, Piece::Colour turn = Piece::WHITE) const;
        [[nodiscard]] Move parse_move(Square from, Square to, Piece::Type promo_type = Piece::NO_TYPE) const;

		void push_uci(const std::string& uci);
		void push_san(const std::string& san, Piece::Colour turn = Piece::WHITE);
        void push_move(Square from, Square to, Piece::Type promo_type = Piece::NO_TYPE);
        void push_move(const Move& move);

        void legal_moves(std::vector<Move>& moves) const;
        void legal_moves(std::vector<Move>& moves, BitBoard from_mask, BitBoard to_mask) const;
        [[nodiscard]] std::vector<Move> legal_moves() const;
        [[nodiscard]] std::vector<Move> legal_moves(BitBoard from_mask, BitBoard to_mask) const;

		void flip();

		size_t perft(size_t depth);

	private:
		void get_pawn_moves(std::vector<Move>& moves) const;
		void get_knight_moves(std::vector<Move>& moves) const;
		void get_bishop_moves(std::vector<Move>& moves) const;
		void get_rook_moves(std::vector<Move>& moves) const;
		void get_queen_moves(std::vector<Move>& moves) const;
		void get_king_moves(std::vector<Move>& moves) const;

		void add_pseudo_legal_pawn_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const;
		void add_pseudo_legal_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const;

		[[nodiscard]] BitBoard knight_attacks(BitBoard knights) const;
		[[nodiscard]] BitBoard bishop_attacks(BitBoard bishops, BitBoard empty) const;
		[[nodiscard]] BitBoard rook_attacks(BitBoard rooks, BitBoard empty) const;
		[[nodiscard]] BitBoard queen_attacks(BitBoard queens, BitBoard empty) const;
		[[nodiscard]] BitBoard king_attacks(BitBoard kings) const;

		[[nodiscard]] inline bool move_in_check(const Move& move) const;

        [[nodiscard]] inline bool is_white_in_check() const;

		size_t perft(size_t depth, std::vector<std::vector<Move>>& storage);

		void check_bb_mailbox_sync() const;
	};
} 

#endif