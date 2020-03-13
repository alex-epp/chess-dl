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

	template <Colour C>
	constexpr Rank orient(Rank r);
	template <>
	constexpr Rank orient<Colour::WHITE> (Rank r) {
	    return r;
	}
    template <>
    constexpr Rank orient<Colour::BLACK> (Rank r) {
        return static_cast<Rank>(7 - static_cast<size_t>(r));
    }

	class BaseBoard {
	public:
		BaseBoard(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move);

		void clear();

		[[nodiscard]] constexpr BitBoard pieces() const;
        [[nodiscard]] constexpr BitBoard pieces(Colour c) const;
        [[nodiscard]] constexpr BitBoard pieces(Piece::Type p) const;
        [[nodiscard]] constexpr BitBoard pieces(Piece::Type p, Colour c) const;
        [[nodiscard]] constexpr BitBoard pawns() const;
        [[nodiscard]] constexpr BitBoard pawns(Colour c) const;
        [[nodiscard]] constexpr BitBoard knights() const;
        [[nodiscard]] constexpr BitBoard knights(Colour c) const;
        [[nodiscard]] constexpr BitBoard bishops() const;
        [[nodiscard]] constexpr BitBoard bishops(Colour c) const;
        [[nodiscard]] constexpr BitBoard rooks() const;
        [[nodiscard]] constexpr BitBoard rooks(Colour c) const;
        [[nodiscard]] constexpr BitBoard queens() const;
        [[nodiscard]] constexpr BitBoard queens(Colour c) const;
        [[nodiscard]] constexpr BitBoard kings() const;
        [[nodiscard]] constexpr BitBoard kings(Colour c) const;

        [[nodiscard]] constexpr bool is_piece_at(Square square) const;
        [[nodiscard]] constexpr Piece get_piece_at(Square square) const;

        [[nodiscard]] constexpr Square en_target() const;

        inline void set_queen_castle(Colour c, bool value);
        inline void set_king_castle(Colour c, bool value);
        [[nodiscard]] constexpr bool can_queen_castle(Colour c) const;
        [[nodiscard]] constexpr bool can_king_castle(Colour c) const;

		void inline put_piece(Piece piece, Square square);
		void inline remove_piece(Square square);

		[[nodiscard]] std::string fen() const;

	protected:
		// Bitboards stored with little-endian-rank-file-mapping
		std::array<BitBoard, 8> piece_BB;

		// Mailbox stored with least-significant-file-mapping
		Mailbox piece_mailbox;

		// Square behind the pawn that just made a two-square move.
		// If no such pawn exists, set to UINT_MAX
		Square en_passant_target;

		// Castling rights
		std::array<bool, 2> king_castling_rights;
        std::array<bool, 2> queen_castling_rights;
		size_t halfmove_clock, fullmove_clock;
        Colour turn;
    };
	inline std::wostream& operator << (std::wostream& stream, const BaseBoard& board) {
        return print_board(stream, board);
    }

	class Board : public BaseBoard {
	public:
		Board(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move);

        [[nodiscard]] Move parse_uci(const std::string& uci) const;
        [[nodiscard]] Move parse_san(const std::string& san) const;
        [[nodiscard]] Move parse_move(Square from, Square to, Piece::Type promo_type = Piece::NO_TYPE) const;

		void push_uci(const std::string& uci);
		void push_san(const std::string& san);
        void push_move(Square from, Square to, Piece::Type promo_type = Piece::NO_TYPE);
        void push_move(const Move& move);

        void legal_moves(std::vector<Move>& moves) const;
        void legal_moves(std::vector<Move>& moves, BitBoard from_mask, BitBoard to_mask) const;
        [[nodiscard]] std::vector<Move> legal_moves() const;
        [[nodiscard]] std::vector<Move> legal_moves(BitBoard from_mask, BitBoard to_mask) const;

        [[nodiscard]] inline bool is_check() const;
        [[nodiscard]] inline bool is_check(Colour c) const;
        [[nodiscard]] inline bool is_draw() const;
        [[nodiscard]] inline bool is_draw_50_move() const;
        [[nodiscard]] inline bool is_draw_insufficient_material() const;

        size_t perft(size_t depth);

	private:
        template <Colour TurnColour>
        [[nodiscard]] Move parse_san(const std::string& san) const;

	    template <Colour TurnColour>
	    void push_move(const Move& m);

	    template <Colour TurnColour>
        void legal_moves(std::vector<Move>& moves) const;

	    template <Colour TurnColour>
		void get_pawn_moves(std::vector<Move>& moves) const;
        template <Colour TurnColour>
        void get_knight_moves(std::vector<Move>& moves) const;
        template <Colour TurnColour>
        void get_bishop_moves(std::vector<Move>& moves) const;
        template <Colour TurnColour>
        void get_rook_moves(std::vector<Move>& moves) const;
        template <Colour TurnColour>
        void get_queen_moves(std::vector<Move>& moves) const;
        template <Colour TurnColour>
        void get_king_moves(std::vector<Move>& moves) const;

        template <Colour TurnColour>
        [[nodiscard]] bool move_in_check(const Move& move) const;
        template <Colour TurnColour>
        [[nodiscard]] bool is_in_check() const;

        template <Colour TurnColour>
		void add_pseudo_legal_pawn_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const;
        template <Colour TurnColour>
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

    constexpr BitBoard BaseBoard::pieces() const {
        return this->piece_BB[static_cast<size_t>(Colour::WHITE)] |
               this->piece_BB[static_cast<size_t>(Colour::BLACK)];
    }
    constexpr BitBoard BaseBoard::pieces(Colour c) const {
        return this->piece_BB[static_cast<size_t>(c)];
    }
    constexpr BitBoard BaseBoard::pieces(Piece::Type p) const {
        return this->piece_BB[p];
    }
    constexpr BitBoard BaseBoard::pieces(Piece::Type p, Colour c) const {
        return this->piece_BB[p] & this->piece_BB[static_cast<size_t>(c)];
    }
    constexpr BitBoard BaseBoard::pawns() const {
        return this->pieces(Piece::PAWN);
    }
    constexpr BitBoard BaseBoard::pawns(Colour c) const {
        return this->pieces(Piece::PAWN, c);
    }
    constexpr BitBoard BaseBoard::knights() const {
        return this->pieces(Piece::KNIGHT);
    }
    constexpr BitBoard BaseBoard::knights(Colour c) const {
        return this->pieces(Piece::KNIGHT, c);
    }
    constexpr BitBoard BaseBoard::bishops() const {
        return this->pieces(Piece::BISHOP);
    }
    constexpr BitBoard BaseBoard::bishops(Colour c) const {
        return this->pieces(Piece::BISHOP, c);
    }
    constexpr BitBoard BaseBoard::rooks() const {
        return this->pieces(Piece::ROOK);
    }
    constexpr BitBoard BaseBoard::rooks(Colour c) const {
        return this->pieces(Piece::ROOK, c);
    }
    constexpr BitBoard BaseBoard::queens() const {
        return this->pieces(Piece::QUEEN);
    }
    constexpr BitBoard BaseBoard::queens(Colour c) const {
        return this->pieces(Piece::QUEEN, c);
    }
    constexpr BitBoard BaseBoard::kings() const {
        return this->pieces(Piece::KING);
    }
    constexpr BitBoard BaseBoard::kings(Colour c) const {
        return this->pieces(Piece::KING, c);
    }

    constexpr Square BaseBoard::en_target() const {
        return this->en_passant_target;
    }

    inline void BaseBoard::put_piece(Piece piece, Square square) {
        this->remove_piece(square);
        this->piece_BB[piece.type()] |= square;
        this->piece_BB[static_cast<size_t>(piece.colour())] |= square;
        this->piece_mailbox.set(square, piece);
    }
    inline void BaseBoard::remove_piece(Square square) {
        for (int i = 0; i < 8; ++i)
            this->piece_BB[i] &= ~BitBoard(square);
        this->piece_mailbox.clear(square);

        for (int i = 0; i < 8; ++i)
            assert(!this->piece_BB[i].is_piece_at(square));
        assert(!this->piece_mailbox.is_piece_at(square));
    }

    void BaseBoard::set_queen_castle(Colour c, bool value) {
        this->queen_castling_rights[static_cast<size_t>(c)] = value;
    }
    void BaseBoard::set_king_castle(Colour c, bool value) {
        this->king_castling_rights[static_cast<size_t>(c)] = value;
    }
    [[nodiscard]] constexpr bool BaseBoard::can_queen_castle(Colour c) const {
        return this->queen_castling_rights[static_cast<size_t>(c)];
    }
    [[nodiscard]] constexpr bool BaseBoard::can_king_castle(Colour c) const {
        return this->king_castling_rights[static_cast<size_t>(c)];
    }

    constexpr bool BaseBoard::is_piece_at(Square square) const {
        return this->piece_mailbox.is_piece_at(square);
    }
    constexpr Piece BaseBoard::get_piece_at(Square square) const {
        return this->piece_mailbox.get(square);
    }

    bool Board::is_check() const {
        return this->is_check(this->turn);
    }

    bool Board::is_check(Colour c) const {
        if (c == Colour::WHITE)
            return this->is_in_check<Colour::WHITE>();
        else
            return this->is_in_check<Colour::BLACK>();
    }

//    bool Board::is_draw() const {
//        return this->is_draw_50_move() || this->is_draw_insufficient_material();
//    }
    bool Board::is_draw_50_move() const {
        return this->halfmove_clock >= 50;
    }
    bool Board::is_draw_insufficient_material() const {
        if (this->pieces().pop_count() > 4)
            return false;  // No draw, too many pieces
        else if (this->pieces().pop_count() == 2)
            return true; // King vs king
        else if (this->pieces().pop_count() == 3 && (this->bishops().pop_count() == 1 ||
                                                     this->knights().pop_count() == 1))
            return true; // King+bishop vs king or king+knight vs king
        else if (this->bishops(Colour::WHITE).pop_count() == 1 &&
                 this->bishops(Colour::BLACK).pop_count() == 1 &&
                 this->bishops(Colour::WHITE).first().is_white_square() == this->bishops(Colour::BLACK).first().is_white_square())
            return true; // bishops on same square colour
        else
            return false;
    }

    template <Colour TurnColour>
    bool Board::move_in_check(const Move& move) const {
        auto b = *this;
        b.push_move(move);
        return b.is_in_check<TurnColour>();
    }

    template <Colour TurnColour>
    bool Board::is_in_check() const {
        constexpr auto EnemyColour = Piece::enemy_colour(TurnColour);

        const auto empty = ~this->pieces();
        auto attacks = this->bishop_attacks(this->bishops(EnemyColour) | this->queens(EnemyColour), empty)
                       | this->rook_attacks(this->rooks(EnemyColour) | this->queens(EnemyColour), empty)
                       | this->knight_attacks(this->knights(EnemyColour))
                       | this->pawns(EnemyColour).shift_NE(EnemyColour)
                       | this->pawns(EnemyColour).shift_NW(EnemyColour);
        return !(this->kings(TurnColour) & attacks).empty();
    }

    template <Colour TurnColour>
    void Board::add_pseudo_legal_pawn_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const {
        const auto move = Move(from, to, flags);
        if (!this->move_in_check<TurnColour>(move)) {
            if (to.rank() == orient<TurnColour>(Rank::R8)) {
                moves.emplace_back(from, to, flags | Move::N_PROMOTION);
                moves.emplace_back(from, to, flags | Move::B_PROMOTION);
                moves.emplace_back(from, to, flags | Move::R_PROMOTION);
                moves.emplace_back(from, to, flags | Move::Q_PROMOTION);
            }
            else {
                moves.emplace_back(move);
            }
        }
    }
    template <Colour TurnColour>
    void Board::add_pseudo_legal_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const {
        const auto move = Move(from, to, flags);
        if (!this->move_in_check<TurnColour>(move)) {
            moves.emplace_back(move);
        }
    }

    template <Colour TurnColour>
    void Board::get_pawn_moves(std::vector<Move>& moves) const {
        constexpr auto EnemyColour = Piece::enemy_colour(TurnColour);

        const auto right_capture_BB = this->pawns(TurnColour) & this->pieces(EnemyColour).shift_SW(TurnColour);
        const auto left_capture_BB = this->pawns(TurnColour) & this->pieces(EnemyColour).shift_SE(TurnColour);
        const auto push_BB = this->pawns(TurnColour) & ~this->pieces().shift_S(TurnColour);
        const auto double_push_BB = push_BB & BB::R2.orient(TurnColour) & ~this->pieces().shift_S(TurnColour, 2);

        for (const auto from_position : left_capture_BB) {
            auto to_position = from_position.northwest(TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->is_piece_at(to_position));
            assert(this->get_piece_at(to_position).colour() == EnemyColour);
            this->add_pseudo_legal_pawn_move<TurnColour>(moves, from_position, to_position, Move::CAPTURE);
        }

        for (const auto from_position : right_capture_BB) {
            auto to_position = from_position.northeast(TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->is_piece_at(to_position));
            assert(this->get_piece_at(to_position).colour() == EnemyColour);
            this->add_pseudo_legal_pawn_move<TurnColour>(moves, from_position, to_position, Move::CAPTURE);
        }

        for (const auto from_position : push_BB) {
            auto to_position = from_position.north(TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(!this->is_piece_at(to_position));
            this->add_pseudo_legal_pawn_move<TurnColour>(moves, from_position, to_position, Move::QUIET);
        }

        for (const auto from_position : double_push_BB) {
            auto to_position = from_position.north(TurnColour, 2);
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(!this->is_piece_at(to_position));
            assert(!this->is_piece_at(to_position.south(TurnColour)));
            assert(from_position.rank() == orient<TurnColour>(Rank::R2));
            this->add_pseudo_legal_pawn_move<TurnColour>(moves, from_position, to_position, Move::D_P_PUSH);
        }

        if (this->en_passant_target.get() != Square::EMPTY) {
            assert(this->en_passant_target.rank() == orient<TurnColour>(Rank::R6));

            // En-passant capture to the left
            for (const auto from_position : this->pawns(TurnColour) & BitBoard(this->en_passant_target).shift_SE(TurnColour)) {
                auto to_position = from_position.northwest(TurnColour);
                assert(this->get_piece_at(from_position).type() == Piece::PAWN);
                assert(this->get_piece_at(from_position).colour() == TurnColour);
                assert(!this->is_piece_at(to_position));
                assert(this->get_piece_at(to_position.south(TurnColour)).type() == Piece::PAWN);
                assert(this->get_piece_at(to_position.south(TurnColour)).colour() == EnemyColour);
                assert(to_position == this->en_passant_target);
                this->add_pseudo_legal_pawn_move<TurnColour>(moves, from_position, to_position, Move::EN_CAPTURE);
            }

            // En-passant capture to the right
            for (const auto from_position : this->pawns(TurnColour) & BitBoard(this->en_passant_target).shift_SW(TurnColour)) {
                auto to_position = from_position.northeast(TurnColour);
                assert(this->get_piece_at(from_position).type() == Piece::PAWN);
                assert(this->get_piece_at(from_position).colour() == TurnColour);
                assert(!this->is_piece_at(to_position));
                assert(this->get_piece_at(to_position.south(TurnColour)).type() == Piece::PAWN);
                assert(this->get_piece_at(to_position.south(TurnColour)).colour() == EnemyColour);
                assert(to_position == this->en_passant_target);
                this->add_pseudo_legal_pawn_move<TurnColour>(moves, from_position, to_position, Move::EN_CAPTURE);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_knight_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->knights(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::KNIGHT);
            auto move_bb = this->knight_attacks(BitBoard(from_position)) & ~this->pieces(TurnColour);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_bishop_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->bishops(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::BISHOP);
            auto position_bb = BitBoard(from_position);
            auto move_bb = this->bishop_attacks(position_bb, ~this->pieces()) & ~this->pieces(TurnColour);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_rook_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->rooks(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::ROOK);
            auto move_bb = this->rook_attacks(BitBoard(from_position), ~this->pieces()) & ~this->pieces(TurnColour);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_queen_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->queens(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::QUEEN);
            auto move_bb = this->queen_attacks(BitBoard(from_position), ~this->pieces()) & ~this->pieces(TurnColour);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_king_moves(std::vector<Move>& moves) const {
        constexpr auto EnemyColour = Piece::enemy_colour(TurnColour);

        auto empty = ~this->pieces();
        auto attacks = this->bishop_attacks(this->bishops(EnemyColour) | this->queens(EnemyColour), empty)
                       | this->rook_attacks(this->rooks(EnemyColour) | this->queens(EnemyColour), empty)
                       | this->king_attacks(this->kings(EnemyColour))
                       | this->knight_attacks(this->knights(EnemyColour))
                       | this->pawns(EnemyColour).shift_SE(TurnColour) | this->pawns(EnemyColour).shift_SW(TurnColour);

        auto move_bb = this->king_attacks(this->kings(TurnColour)) & ~this->pieces(TurnColour) & ~attacks;
        auto from_position = this->kings(TurnColour).first();
        assert(this->get_piece_at(from_position).colour() == TurnColour);
        assert(this->get_piece_at(from_position).type() == Piece::KING);

        for (const auto to_position : move_bb) {
            assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == EnemyColour);
            auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
            this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
        }

        if (this->can_king_castle(TurnColour) &&
                (attacks & (BB::E1 | BB::F1 | BB::G1).orient(TurnColour)).empty() &&
                (this->pieces() & (BB::F1 | BB::G1).orient(TurnColour)).empty()) {
            this->add_pseudo_legal_move<TurnColour>(moves, S::E1.orient(TurnColour), S::G1.orient(TurnColour), Move::K_CASTLE);
        }
        if (this->can_queen_castle(TurnColour) &&
                (attacks & (BB::E1 | BB::D1 | BB::C1).orient(TurnColour)).empty() &&
                (this->pieces() & (BB::D1 | BB::C1 | BB::B1).orient(TurnColour)).empty()) {
            this->add_pseudo_legal_move<TurnColour>(moves, S::E1.orient(TurnColour), S::C1.orient(TurnColour), Move::Q_CASTLE);
        }
    }

    template <Colour TurnColour>
    Move Board::parse_san(const std::string& san) const {
        // Castling
        if (san == "O-O" || san == "O-O+" || san == "O-O#") {
            assert(this->can_king_castle(TurnColour));
            return this->parse_move(S::E1.orient(TurnColour), S::G1.orient(TurnColour));
        } else if (san == "O-O-O" || san == "O-O-O+" || san == "O-O-O#") {
            assert(this->can_queen_castle(TurnColour));
            return this->parse_move(S::E1.orient(TurnColour), S::C1.orient(TurnColour));
        }

        const static std::regex SAN_REGEX(R"(^([NBKRQ])?([a-h])?([1-8])?[\-x]?([a-h][1-8])=?([nbrqkNBRQK])?(\+|#)?$)");

        // Match normal moves
        std::smatch match;
        auto matched = std::regex_match(san, match, SAN_REGEX);
        assert(matched);

        // Get target square
        Square to_square(match[4].str());

        // Get the promotion type
        auto p = match[5].str();
        auto promotion = p.empty() ? Piece::NO_TYPE : Piece(std::toupper(p[0])).type();

        // Filter by piece type
        BitBoard from_mask;
        if (!match[1].str().empty()) {
            auto piece_type = Piece(match[1].str()[0]).type();
            from_mask = this->pieces(piece_type, TurnColour);
        } else {
            from_mask = this->pawns(TurnColour);
        }

        // Filter by source file
        if (!match[2].str().empty()) {
            auto f = match[2].str()[0] - 'a';
            assert(0 <= f && f <= 7);
            from_mask &= static_cast<File>(f);
        }

        // Filter by source rank
        if (!match[3].str().empty()) {
            auto r = match[3].str()[0] - '1';
            assert(0 <= r && r <= 7);
            from_mask &= static_cast<Rank>(r);
        }

        // Match legal moves
        Move matched_move(Square::EMPTY, Square::EMPTY);
        for (const auto& move : this->legal_moves(from_mask, to_square)) {
            if (move.is_promotion()) {
                if (promotion != move.promoted_type()) continue;
            } else {
                if (promotion != Piece::NO_TYPE) continue;
            }

            assert(matched_move.from().get() != Square::EMPTY);

            matched_move = move;
        }
        assert(matched_move.from().get() != Square::EMPTY);

        return matched_move;
    }

	template <Colour TurnColour>
	void Board::push_move(const Move& move) {
        assert(this->is_piece_at(move.from()));
        assert(this->piece_mailbox.get(move.from()).colour() == TurnColour);
        if (this->is_piece_at(move.to()))
            assert(this->piece_mailbox.get(move.to()).colour() == Piece::enemy_colour<TurnColour>());
        auto type = this->piece_mailbox.get(move.from()).type();
        if (!this->en_passant_target.is_empty())
            assert(this->en_passant_target.rank() == orient<TurnColour>(Rank::R6));

        auto from_BB = BitBoard(move.from());
        auto to_BB = BitBoard(move.to());
        auto from_to_BB = from_BB ^ to_BB;
        auto piece = this->piece_mailbox.get(move.from());

        auto captured_index = move.is_en_capture() ? move.to().backward(TurnColour) : move.to();
        auto captured_piece = this->piece_mailbox.get(captured_index);
        auto captured_BB = BitBoard(captured_index);

        this->en_passant_target = Square::EMPTY;

        if (move.is_capture()) {
            if (move.to() != captured_index) {
                assert(this->is_piece_at(captured_index));
                assert(this->get_piece_at(captured_index).colour() == Piece::enemy_colour<TurnColour>());
            }
            this->piece_mailbox.clear(captured_index);
            this->piece_BB[captured_piece.type()] ^= captured_BB;
            this->piece_BB[static_cast<size_t>(captured_piece.colour())] ^= captured_BB;

            if (captured_piece.type() == Piece::ROOK) {
                if (captured_index == S::A8.orient(TurnColour))
                    this->set_queen_castle(Piece::enemy_colour(TurnColour), false);
                if (captured_index == S::H8.orient(TurnColour))
                    this->set_king_castle(Piece::enemy_colour(TurnColour), false);
            }
        }

        this->piece_mailbox.clear(move.from());
        this->piece_mailbox.set(move.to(), piece);
        this->piece_BB[piece.type()] ^= from_to_BB;
        this->piece_BB[static_cast<size_t>(piece.colour())] ^= from_to_BB;

        if (move.is_promotion()) {
            this->piece_mailbox.set(move.to(), Piece(move.promoted_type(), piece.colour()));
            this->piece_BB[piece.type()] ^= to_BB;
            this->piece_BB[move.promoted_type()] ^= to_BB;
        }

        if (move.is_castle()) {
            Square rook_from = 0, rook_to = 0;
            if (move.is_king_castle()) {
                assert(move.from() == S::E1.orient(TurnColour));
                assert(move.to() == S::G1.orient(TurnColour));
                assert(this->can_king_castle(TurnColour));
                rook_from = S::H1.orient(TurnColour);
                rook_to = S::F1.orient(TurnColour);
                this->set_king_castle(TurnColour, false);
            }
            else {
                assert(move.from() == S::E1.orient(TurnColour));
                assert(move.to() == S::C1.orient(TurnColour));
                assert(this->can_queen_castle(TurnColour));
                rook_from = S::A1.orient(TurnColour);
                rook_to = S::D1.orient(TurnColour);
                this->set_queen_castle(TurnColour, false);
            }
            auto rook_from_to_BB = BitBoard(rook_from) ^ BitBoard(rook_to);
            this->piece_BB[static_cast<size_t>(piece.colour())] ^= rook_from_to_BB;
            this->piece_BB[Piece::ROOK] ^= rook_from_to_BB;
            this->piece_mailbox.set(rook_to, this->piece_mailbox.get(rook_from));
            this->piece_mailbox.clear(rook_from);
        }

        if (piece.type() == Piece::ROOK) {
            if (move.from() == S::A1.orient(TurnColour)) this->set_queen_castle(TurnColour, false);
            if (move.from() == S::H1.orient(TurnColour)) this->set_king_castle(TurnColour, false);
        }
        else if (piece.type() == Piece::KING) {
            this->set_queen_castle(TurnColour, false);
            this->set_king_castle(TurnColour, false);
        }

        if (move.is_double_pawn_push()) {
            this->en_passant_target = move.to().backward(TurnColour);
        }

        if (move.is_capture() || piece.type() == Piece::PAWN) {
            this->halfmove_clock = 0;
        } else {
            ++this->halfmove_clock;
        }

        if (TurnColour == Colour::BLACK) {
            ++this->fullmove_clock;
        }

        this->turn = Piece::enemy_colour(TurnColour);

        assert(!this->is_piece_at(move.from()));
        assert(this->is_piece_at(move.to()));
        assert(this->piece_mailbox.get(move.to()).colour() == TurnColour);
        if (!move.is_promotion())
            assert(this->piece_mailbox.get(move.to()).type() == type);
        else
            assert(this->piece_mailbox.get(move.to()).type() == move.promoted_type());
        if (captured_index.get() != move.to().get())
            assert(!this->is_piece_at(captured_index));
        if (this->en_passant_target.get() != Square::EMPTY)
            assert(this->en_passant_target.rank() == orient<TurnColour>(Rank::R3));
        this->check_bb_mailbox_sync();
	}

    template <Colour TurnColour>
    void Board::legal_moves(std::vector<Move>& moves) const {
        moves.clear();
        this->get_pawn_moves<TurnColour>(moves);
        this->get_knight_moves<TurnColour>(moves);
        this->get_bishop_moves<TurnColour>(moves);
        this->get_rook_moves<TurnColour>(moves);
        this->get_queen_moves<TurnColour>(moves);
        this->get_king_moves<TurnColour>(moves);
    }
} 

#endif