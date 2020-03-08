#include "chess.h"


#include <iostream>
#include <map>
#include <string_view>


namespace chess {
	std::wostream& print_board(std::wostream& stream, const BaseBoard& board) {
		for (int ri = 7; ri >= 0; --ri) {
			for (int fi = 0; fi < 8; ++fi) {
				auto r = static_cast<Rank>(ri);
				auto f = static_cast<File>(fi);
				auto piece = board.get_piece_at(Square(f, r));

				stream << piece_repr(piece);
			}
			stream << '\n';
		}
		return stream;
	}

    Board::Board(Piece::Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move)
            : BaseBoard(turn, castle_rights, en_passant_target, half_move, full_move)
    {}

    Move Board::parse_uci(const std::string& uci) const {
        assert(uci.length() == 4 || uci.length() == 5);
        auto from = Square(uci.substr(0, 2));
        auto to = Square(uci.substr(2, 2));
        auto promo_type = (uci.length() > 4) ? Piece(uci[4]).type() : Piece::NO_TYPE;
        return this->parse_move(from, to, promo_type);
    }
    /**
    Move Board::parse_move(Square from, Square to, Piece::Type promo_type) const {
        auto piece = this->piece_mailbox.get(from);
        auto flags = Move::QUIET;

        assert(piece.type() != Piece::NO_TYPE);
        assert(piece.colour() == Piece::WHITE);
        if (piece_mailbox.is_piece_at(to))
            assert(piece_mailbox.get(to).colour() == Piece::BLACK);

        if (piece.type() == Piece::PAWN) {
            if ((from.rank() == Rank::R2 && to.rank() == Rank::R4) ||
                (from.rank() == Rank::R7 && to.rank() == Rank::R5)) {
                flags |= Move::D_P_PUSH;
            }
            else if (to == this->en_passant_target) {
                flags |= Move::EN_CAPTURE;
            }
        }

        if (piece.type() == Piece::KING) {
            if ((from == S::E1 && to == S::G1) ||
                (from == S::E8 && to == S::G8)) {
                flags |= Move::K_CASTLE;
            }
            else if ((from == S::E1 && to == S::C1) ||
                     (from == S::E8 && to == S::C8)) {
                flags |= Move::Q_CASTLE;
            }
        }

        if (this->piece_mailbox.is_piece_at(to)) {
            flags |= Move::CAPTURE;
        }

        return Move(from, to, flags, promo_type);
    }

    void Board::push_uci(const std::string& uci) {
        this->push_move(this->parse_uci(uci));
    }
    void Board::push_san(const std::string& san, Piece::Colour turn) {
        this->push_move(this->parse_san(san, turn));
    }
    void Board::push_move(Square from, Square to, Piece::Type promo_type) {
        this->push_move(this->parse_move(from, to, promo_type));
    }
    void Board::push_move(const Move& move) {
        assert(this->is_piece_at(move.from()));
        assert(this->piece_mailbox.get(move.from()).colour() == Piece::WHITE);
        if (this->is_piece_at(move.to()))
            assert(this->piece_mailbox.get(move.to()).colour() == Piece::BLACK);
        auto type = this->piece_mailbox.get(move.from()).type();
        if (this->en_passant_target.get() != Square::EMPTY)
            assert(this->en_passant_target.rank() == Rank::R6);

        auto from_BB = BitBoard(move.from());
        auto to_BB = BitBoard(move.to());
        auto from_to_BB = from_BB ^ to_BB;
        auto piece = this->piece_mailbox.get(move.from());

        auto captured_index = move.is_en_capture() ? move.to().backward(piece.colour()) : move.to();
        auto captured_piece = this->piece_mailbox.get(captured_index);
        auto captured_BB = BitBoard(captured_index);

        this->en_passant_target = Square::EMPTY;

        if (move.is_capture()) {
            if (move.to().get() != captured_index.get()) {
                assert(this->is_piece_at(captured_index));
                assert(this->get_piece_at(captured_index).colour() == Piece::BLACK);
            }
            this->piece_mailbox.clear(captured_index);
            this->piece_BB[captured_piece.type()] ^= captured_BB;
            this->piece_BB[captured_piece.colour()] ^= captured_BB;

            if (captured_piece.type() == Piece::ROOK) {
                if (captured_index == S::A8) this->can_black_queen_castle = false;
                if (captured_index == S::H8) this->can_black_king_castle = false;
            }
        }

        this->piece_mailbox.clear(move.from());
        this->piece_mailbox.set(move.to(), piece);
        this->piece_BB[piece.type()] ^= from_to_BB;
        this->piece_BB[piece.colour()] ^= from_to_BB;

        if (move.is_promotion()) {
            this->piece_mailbox.set(move.to(), Piece(move.promoted_type(), piece.colour()));
            this->piece_BB[piece.type()] ^= to_BB;
            this->piece_BB[move.promoted_type()] ^= to_BB;
        }

        if (move.is_castle()) {
            Square rook_from = 0, rook_to = 0;
            if (move.is_king_castle()) {
                assert(this->can_white_king_castle);
                rook_from = (piece.colour() == Piece::WHITE) ? S::H1 : S::H8;
                rook_to = (piece.colour() == Piece::WHITE) ? S::F1 : S::F8;
                this->can_white_king_castle = false;
            }
            else {
                assert(this->can_white_queen_castle);
                rook_from = (piece.colour() == Piece::WHITE) ? S::A1 : S::A8;
                rook_to = (piece.colour() == Piece::WHITE) ? S::D1 : S::D8;
                this->can_white_queen_castle = false;
            }
            auto rook_from_to_BB = BitBoard(rook_from) ^ BitBoard(rook_to);
            this->piece_BB[piece.colour()] ^= rook_from_to_BB;
            this->piece_BB[Piece::ROOK] ^= rook_from_to_BB;
            this->piece_mailbox.set(rook_to, this->piece_mailbox.get(rook_from));
            this->piece_mailbox.clear(rook_from);
        }

        if (piece.type() == Piece::ROOK) {
            if (move.from() == S::A1) this->can_white_queen_castle = false;
            if (move.from() == S::H1) this->can_white_king_castle = false;
        }
        else if (piece.type() == Piece::KING) {
            this->can_white_king_castle = this->can_white_queen_castle = false;
        }

        if (move.is_double_pawn_push()) {
            this->en_passant_target = move.to().backward(piece.colour());
        }

        assert(!this->is_piece_at(move.from()));
        assert(this->is_piece_at(move.to()));
        assert(this->piece_mailbox.get(move.to()).colour() == Piece::WHITE);
        if (!move.is_promotion())
            assert(this->piece_mailbox.get(move.to()).type() == type);
        else
            assert(this->piece_mailbox.get(move.to()).type() == move.promoted_type());
        if (captured_index.get() != move.to().get())
            assert(!this->is_piece_at(captured_index));
        if (this->en_passant_target.get() != Square::EMPTY)
            assert(this->en_passant_target.rank() == Rank::R3);
        this->check_bb_mailbox_sync();
    }

    void Board::legal_moves(std::vector<Move>& moves) const {
        moves.clear();
        this->get_pawn_moves(moves);
        this->get_knight_moves(moves);
        this->get_bishop_moves(moves);
        this->get_rook_moves(moves);
        this->get_queen_moves(moves);
        this->get_king_moves(moves);
    }
    void Board::legal_moves(std::vector<Move>& moves, BitBoard from_mask, BitBoard to_mask) const {
        moves.clear();
        for (const auto& move : this->legal_moves()) {
            if (from_mask.contains(move.from()) && to_mask.contains(move.to())) {
                moves.push_back(move);
            }
        }
    }

    std::vector<Move> Board::legal_moves() const {
        std::vector<Move> moves;
        this->legal_moves(moves);
        return moves;
    }

    std::vector<Move> Board::legal_moves(BitBoard from_mask, BitBoard to_mask) const {
        std::vector<Move> moves;
        this->legal_moves(moves, from_mask, to_mask);
        return moves;
    }

    void Board::flip() {
        for (auto& bb : this->piece_BB) assert(bb == bb.flip_vertical().flip_vertical());
        assert(this->en_passant_target == this->en_passant_target.flip_vertical().flip_vertical());
        auto old_en_passant_target = this->en_passant_target;
        auto old_can_white_king_castle = this->can_white_king_castle;
        auto old_can_black_king_castle = this->can_black_king_castle;
        auto old_can_white_queen_castle = this->can_white_queen_castle;
        auto old_can_black_queen_castle = this->can_black_queen_castle;
        auto old_white_BB = this->piece_BB[Piece::WHITE];
        auto old_black_BB = this->piece_BB[Piece::BLACK];

        for (auto& bb : this->piece_BB) bb = bb.flip_vertical();
        std::swap(this->piece_BB[Piece::WHITE], this->piece_BB[Piece::BLACK]);
        this->piece_mailbox.flip();
        this->en_passant_target = this->en_passant_target.flip_vertical();
        std::swap(this->can_white_king_castle, this->can_black_king_castle);
        std::swap(this->can_white_queen_castle, this->can_black_queen_castle);

        assert(this->en_passant_target.file() == old_en_passant_target.file());
        if (this->en_passant_target.get() != Square::EMPTY)
            assert((int)this->en_passant_target.rank() == (int)Rank::R8 - (int)old_en_passant_target.rank());
        assert(can_white_king_castle == old_can_black_king_castle);
        assert(can_black_king_castle == old_can_white_king_castle);
        assert(can_white_queen_castle == old_can_black_queen_castle);
        assert(can_black_queen_castle == old_can_white_queen_castle);
        assert(this->piece_BB[Piece::WHITE] == old_black_BB.flip_vertical());
        assert(this->piece_BB[Piece::BLACK] == old_white_BB.flip_vertical());
        this->check_bb_mailbox_sync();
    }

    size_t Board::perft(size_t depth) {
        std::vector<std::vector<Move>> storage(depth, std::vector<Move>());
        return this->perft(depth, storage);
    }

    void Board::get_pawn_moves(std::vector<Move>& moves) const {
        const auto right_capture_BB = this->pawns(Piece::WHITE) & this->pieces(Piece::BLACK).shift_SW();
        const auto left_capture_BB = this->pawns(Piece::WHITE) & this->pieces(Piece::BLACK).shift_SE();
        const auto push_BB = this->pawns(Piece::WHITE) & ~this->pieces().shift_S();
        const auto double_push_BB = push_BB & BB::R2 & ~this->pieces().shift_S(2);

        for (const auto from_position : left_capture_BB) {
            auto to_position = from_position.northwest();
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(this->is_piece_at(to_position));
            assert(this->get_piece_at(to_position).colour() == Piece::BLACK);
            this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::CAPTURE);
        }

        for (const auto from_position : right_capture_BB) {
            auto to_position = from_position.northeast();
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(this->is_piece_at(to_position));
            assert(this->get_piece_at(to_position).colour() == Piece::BLACK);
            this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::CAPTURE);
        }

        for (const auto from_position : push_BB) {
            auto to_position = from_position.north();
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(!this->is_piece_at(to_position));
            this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::QUIET);
        }

        for (const auto from_position : double_push_BB) {
            auto to_position = from_position.north(2);
            assert(this->get_piece_at(from_position).type() == Piece::PAWN);
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(!this->is_piece_at(to_position));
            assert(!this->is_piece_at(to_position.south()));
            assert(from_position.rank() == Rank::R2);
            this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::D_P_PUSH);
        }

        if (this->en_passant_target.get() != Square::EMPTY) {
            assert(this->en_passant_target.rank() == Rank::R6);

            // En-passant capture to the left
            for (const auto from_position : this->pawns(Piece::WHITE) & BitBoard(this->en_passant_target).shift_SE()) {
                auto to_position = from_position.northwest();
                assert(this->get_piece_at(from_position).type() == Piece::PAWN);
                assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
                assert(!this->is_piece_at(to_position));
                assert(this->get_piece_at(to_position.south()).type() == Piece::PAWN);
                assert(this->get_piece_at(to_position.south()).colour() == Piece::BLACK);
                assert(to_position == this->en_passant_target);
                this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::EN_CAPTURE);
            }

            // En-passant capture to the right
            for (const auto from_position : this->pawns(Piece::WHITE) & BitBoard(this->en_passant_target).shift_SW()) {
                auto to_position = from_position.northeast();
                assert(this->get_piece_at(from_position).type() == Piece::PAWN);
                assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
                assert(!this->is_piece_at(to_position));
                assert(this->get_piece_at(to_position.south()).type() == Piece::PAWN);
                assert(this->get_piece_at(to_position.south()).colour() == Piece::BLACK);
                assert(to_position == this->en_passant_target);
                this->add_pseudo_legal_pawn_move(moves, from_position, to_position, Move::EN_CAPTURE);
            }
        }

    }
    void Board::get_knight_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->knights(Piece::WHITE)) {
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(this->get_piece_at(from_position).type() == Piece::KNIGHT);
            auto move_bb = this->knight_attacks(BitBoard(from_position)) & ~this->pieces(Piece::WHITE);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::BLACK);
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move(moves, from_position, to_position, flags);
            }
        }
    }
    void Board::get_bishop_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->bishops(Piece::WHITE)) {
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(this->get_piece_at(from_position).type() == Piece::BISHOP);
            auto position_bb = BitBoard(from_position);
            auto move_bb = this->bishop_attacks(position_bb, ~this->pieces()) & ~this->pieces(Piece::WHITE);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::BLACK);
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move(moves, from_position, to_position, flags);
            }
        }
    }
    void Board::get_rook_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->rooks(Piece::WHITE)) {
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(this->get_piece_at(from_position).type() == Piece::ROOK);
            auto move_bb = this->rook_attacks(BitBoard(from_position), ~this->pieces()) & ~this->pieces(Piece::WHITE);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::BLACK);
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move(moves, from_position, to_position, flags);
            }
        }
    }
    void Board::get_queen_moves(std::vector<Move>& moves) const {
        for (const auto from_position : this->queens(Piece::WHITE)) {
            assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
            assert(this->get_piece_at(from_position).type() == Piece::QUEEN);
            auto move_bb = this->queen_attacks(BitBoard(from_position), ~this->pieces()) & ~this->pieces(Piece::WHITE);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::BLACK);
                auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move(moves, from_position, to_position, flags);
            }
        }
    }
    void Board::get_king_moves(std::vector<Move>& moves) const {
        auto empty = ~this->pieces();
        auto attacks = this->bishop_attacks(this->bishops(Piece::BLACK) | this->queens(Piece::BLACK), empty)
                       | this->rook_attacks(this->rooks(Piece::BLACK) | this->queens(Piece::BLACK), empty)
                       | this->king_attacks(this->kings(Piece::BLACK))
                       | this->knight_attacks(this->knights(Piece::BLACK))
                       | this->pawns(Piece::BLACK).shift_SE() | this->pawns(Piece::BLACK).shift_SW();

        auto move_bb = this->king_attacks(this->kings(Piece::WHITE)) & ~this->pieces(Piece::WHITE) & ~attacks;
        auto from_position = *this->kings(Piece::WHITE).begin();
        assert(this->get_piece_at(from_position).colour() == Piece::WHITE);
        assert(this->get_piece_at(from_position).type() == Piece::KING);

        for (const auto to_position : move_bb) {
            assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::BLACK);
            auto flags = this->piece_mailbox.is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
            this->add_pseudo_legal_move(moves, from_position, to_position, flags);
        }

        if (this->can_white_king_castle && (attacks & (BB::E1 | BB::F1 | BB::G1)).empty() && (this->pieces() & (BB::F1 | BB::G1)).empty()) {
            this->add_pseudo_legal_move(moves, S::E1, S::G1, Move::K_CASTLE);
        }
        if (this->can_white_queen_castle && (attacks & (BB::E1 | BB::D1 | BB::C1)).empty() && (this->pieces() & (BB::D1 | BB::C1 | BB::B1)).empty()) {
            this->add_pseudo_legal_move(moves, S::E1, S::C1, Move::Q_CASTLE);
        }
    }

    void Board::add_pseudo_legal_pawn_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const {
        const auto move = Move(from, to, flags);
        if (!this->move_in_check(move)) {
            if (to.rank() == Rank::R8) {
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
    void Board::add_pseudo_legal_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const {
        const auto move = Move(from, to, flags);
        if (!this->move_in_check(move)) {
            moves.emplace_back(move);
        }
    }

    BitBoard Board::knight_attacks(const BitBoard knights) const {
        auto w1 = knights.shift_W();
        auto w2 = knights.shift_W(2);
        auto e1 = knights.shift_E();
        auto e2 = knights.shift_E(2);
        auto m1 = e1 | w1;
        auto m2 = e2 | w2;
        return m1.shift_N(2) | m1.shift_S(2) | m2.shift_N() | m2.shift_S();
    }
    BitBoard Board::bishop_attacks(const BitBoard bishops, const BitBoard empty) const {
        return bishops.attack_NE_occluded(empty) | bishops.attack_NW_occluded(empty) |
               bishops.attack_SE_occluded(empty) | bishops.attack_SW_occluded(empty);
    }
    BitBoard Board::rook_attacks(const BitBoard rooks, const BitBoard empty) const {
        return rooks.attack_N_occluded(empty) | rooks.attack_E_occluded(empty) |
               rooks.attack_S_occluded(empty) | rooks.attack_W_occluded(empty);
    }
    BitBoard Board::queen_attacks(const BitBoard queens, const BitBoard empty) const {
        return this->bishop_attacks(queens, empty) | this->rook_attacks(queens, empty);
    }
    BitBoard Board::king_attacks(const BitBoard kings) const {
        return kings.shift_N() | kings.shift_NE() | kings.shift_E() | kings.shift_SE()
               | kings.shift_S() | kings.shift_SW() | kings.shift_W() | kings.shift_NW();
    }

    bool Board::move_in_check(const Move& move) const {
        auto b = *this;
        b.push_move(move);
        return b.is_white_in_check();
    }
    bool Board::is_white_in_check() const {
        const auto empty = ~this->pieces();
        auto attacks = this->bishop_attacks(this->bishops(Piece::BLACK) | this->queens(Piece::BLACK), empty)
                       | this->rook_attacks(this->rooks(Piece::BLACK) | this->queens(Piece::BLACK), empty)
                       | this->knight_attacks(this->knights(Piece::BLACK))
                       | this->pawns(Piece::BLACK).shift_SE()
                       | this->pawns(Piece::BLACK).shift_SW();
        return !(this->kings(Piece::WHITE) & attacks).empty();
    }

    size_t Board::perft(size_t depth, std::vector<std::vector<Move>>& storage) {
        if (depth == 0) return 1;

        auto& moves = storage[depth-1];
        this->legal_moves(moves);

        if (depth == 1) return moves.size();

        size_t nodes = 0;
        for (auto& move : moves) {
            Board b = *this;
            b.push_move(move);
            b.flip();
            nodes += b.perft(depth - 1, storage);
        }
        return nodes;
    }

    void Board::check_bb_mailbox_sync() const {
        for (int i = 0; i < 64; ++i) {
            auto piece = this->piece_mailbox.get(i);
            if (piece.type() != Piece::NO_TYPE) {
                assert(this->piece_BB[piece.colour()].is_piece_at(i));
                assert(!this->piece_BB[Piece::enemy_colour(piece.colour())].is_piece_at(i));
            }
            else {
                assert(!this->piece_BB[Piece::WHITE].is_piece_at(i));
                assert(!this->piece_BB[Piece::BLACK].is_piece_at(i));
            }

            for (int j = Piece::PAWN; j < Piece::KING; ++j) {
                if (j == piece.type())
                    assert(this->piece_BB[j].is_piece_at(i));
                else
                    assert(!this->piece_BB[j].is_piece_at(i));
            }
        }
    }
}