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
		BaseBoard(Piece::Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move);

		void clear();

		[[nodiscard]] auto pieces() const;
        [[nodiscard]] auto pieces(Piece::Colour c) const;
        [[nodiscard]] auto pieces(Piece::Type p) const;
        [[nodiscard]] auto pieces(Piece::Type p, Piece::Colour c) const;
        [[nodiscard]] auto pawns() const;
        [[nodiscard]] auto pawns(Piece::Colour c) const;
        [[nodiscard]] auto knights() const;
        [[nodiscard]] auto knights(Piece::Colour c) const;
        [[nodiscard]] auto bishops() const;
        [[nodiscard]] auto bishops(Piece::Colour c) const;
        [[nodiscard]] auto rooks() const;
        [[nodiscard]] auto rooks(Piece::Colour c) const;
        [[nodiscard]] auto queens() const;
        [[nodiscard]] auto queens(Piece::Colour c) const;
        [[nodiscard]] auto kings() const;
        [[nodiscard]] auto kings(Piece::Colour c) const;

        [[nodiscard]] auto is_piece_at(Square square) const;
        [[nodiscard]] auto get_piece_at(Square square) const;

        [[nodiscard]] auto en_target() const;

        [[nodiscard]] auto white_king_castling_rights() const;
        [[nodiscard]] auto white_queen_castling_rights() const;
        [[nodiscard]] auto black_king_castling_rights() const;
        [[nodiscard]] auto black_queen_castling_rights() const;

		void put_piece(Piece piece, Square square);
		void remove_piece(Square square);

		std::string fen() const;

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
        int halfmove_clock, fullmove_clock;
        Piece::Colour turn;
    };
	inline std::wostream& operator << (std::wostream& stream, const BaseBoard& board);

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

        [[nodiscard]] bool is_white_in_check() const;
        [[nodiscard]] bool is_black_in_check() const;

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

        size_t perft(size_t depth, std::vector<std::vector<Move>>& storage);

		void check_bb_mailbox_sync() const;
	};
} 

#endif