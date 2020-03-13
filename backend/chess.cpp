#include "chess.h"


#include <iostream>
#include <map>
#include <string_view>


namespace chess {
    std::string san_flipped(std::string san) {
        for (char& ch : san) {
            if (isdigit(ch)) {
                int d = ch - '0';
                d = 9 - d; // Convert from [1, 8] to [8, 1]
                ch = '0' + d;
            }
        }
        return san;
    }

    BaseBoard::BaseBoard(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move)
                : en_passant_target(en_passant_target), halfmove_clock(half_move), fullmove_clock(full_move), turn(turn)
    {
        this->set_king_castle(Colour::WHITE, castle_rights.find_first_of('K') != std::string::npos);
        this->set_king_castle(Colour::BLACK, castle_rights.find_first_of('k') != std::string::npos);
        this->set_queen_castle(Colour::WHITE, castle_rights.find_first_of('Q') != std::string::npos);
        this->set_queen_castle(Colour::BLACK, castle_rights.find_first_of('q') != std::string::npos);
    }

    void BaseBoard::clear() {
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

    std::string BaseBoard::fen() const {
        std::string fen;
        for (int r = 7; r >= 0; --r) {
            int space = 0;
            for (int f = 0; f < 8; ++f) {
                auto square = Square(static_cast<File>(f), static_cast<Rank>(r));
                if (this->is_piece_at(square)) {
                    if (space > 0) {
                        fen += char(space + '0');
                        space = 0;
                    }

                    char c;
                    switch (this->get_piece_at(square).type()) {
                        case Piece::PAWN: c = 'p'; break;
                        case Piece::KNIGHT: c = 'n'; break;
                        case Piece::BISHOP: c = 'b'; break;
                        case Piece::ROOK: c = 'r'; break;
                        case Piece::QUEEN: c = 'q'; break;
                        case Piece::KING: c = 'k'; break;
                        default: assert(false);
                    }
                    if (this->get_piece_at(square).colour() == Colour::WHITE)
                        c = ::toupper(c);
                    fen += c;
                } else {
                    ++space;
                }
            }
            if (space > 0) {
                fen += char(space + '0');
                space = 0;
            }

            if (r > 0) fen += '/';
        }

        fen += std::string(" ") + (this->turn == Colour::WHITE ? "w" : "b");

        std::string castling_string;
        if (this->can_king_castle(Colour::WHITE)) castling_string += 'K';
        if (this->can_queen_castle(Colour::WHITE)) castling_string += 'Q';
        if (this->can_king_castle(Colour::BLACK)) castling_string += 'k';
        if (this->can_queen_castle(Colour::BLACK)) castling_string += 'q';
        fen += " " + (castling_string.empty() ? "-" : castling_string);

        fen += " " + (en_passant_target.is_empty() ? "-" : en_passant_target.uci());

        fen += " " + std::to_string(this->halfmove_clock);
        fen += " " + std::to_string(this->fullmove_clock);

        return fen;
    }

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

    Board::Board(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move)
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
     * Based on Python-chess's parse_san in __init__.py.
     *
     * How this handles turns is a little unintuitive, and I really need to fix it. Essentially, the Board always plays
     * from white's position. However, SAN can be given from white's or black's perspective. If you have SAN from black's
     * perspective you therefore need to set turn to Black.
     */
    Move Board::parse_san(const std::string& san) const {
        if (this->turn == Colour::WHITE)
            return this->parse_san<Colour::WHITE>(san);
        else
            return this->parse_san<Colour::BLACK>(san);
    }
    Move Board::parse_move(Square from, Square to, Piece::Type promo_type) const {
        auto piece = this->piece_mailbox.get(from);
        auto flags = Move::QUIET;

        assert(piece.type() != Piece::NO_TYPE);
        assert(piece.colour() == Colour::WHITE);
        if (piece_mailbox.is_piece_at(to))
            assert(piece_mailbox.get(to).colour() == Colour::BLACK);

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
    void Board::push_san(const std::string& san) {
        this->push_move(this->parse_san(san));
    }
    void Board::push_move(Square from, Square to, Piece::Type promo_type) {
        this->push_move(this->parse_move(from, to, promo_type));
    }
    void Board::push_move(const Move& move) {
        switch (this->turn) {
            case Colour::WHITE: this->push_move<Colour::WHITE>(move); break;
            case Colour::BLACK: this->push_move<Colour::BLACK>(move); break;
        }
    }

    void Board::legal_moves(std::vector<Move>& moves) const {
        switch (this->turn) {
            case Colour::WHITE: this->legal_moves<Colour::WHITE>(moves); break;
            case Colour::BLACK: this->legal_moves<Colour::BLACK>(moves); break;
        }
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

    size_t Board::perft(size_t depth) {
        std::vector<std::vector<Move>> storage(depth, std::vector<Move>());
        return this->perft(depth, storage);
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

    size_t Board::perft(size_t depth, std::vector<std::vector<Move>>& storage) {
        if (depth == 0) return 1;

        auto& moves = storage[depth-1];
        this->legal_moves(moves);

        if (depth == 1) return moves.size();

        size_t nodes = 0;
        for (auto& move : moves) {
            Board b = *this;
            b.push_move(move);
            nodes += b.perft(depth - 1, storage);
        }
        return nodes;
    }

    void Board::check_bb_mailbox_sync() const {
        for (int i = 0; i < 64; ++i) {
            auto piece = this->piece_mailbox.get(i);
            if (piece.type() != Piece::NO_TYPE) {
                assert(this->piece_BB[static_cast<size_t>(piece.colour())].is_piece_at(i));
                assert(!this->piece_BB[static_cast<size_t>(Piece::enemy_colour(piece.colour()))].is_piece_at(i));
            }
            else {
                assert(!this->piece_BB[static_cast<size_t>(Colour::WHITE)].is_piece_at(i));
                assert(!this->piece_BB[static_cast<size_t>(Colour::BLACK)].is_piece_at(i));
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