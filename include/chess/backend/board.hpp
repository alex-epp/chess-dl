#pragma once

#include "bitboard.hpp"
#include "move.hpp"
#include "piece.hpp"
#include "square.hpp"
#include "pext_attacks.hpp"

#include <cctype>
#include <regex>
#include <vector>


namespace chess {
    class BaseBoard {
    public:
        inline BaseBoard(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move) noexcept;

        inline void clear() noexcept;

        [[nodiscard]] constexpr BitBoard pieces() const noexcept;
        [[nodiscard]] constexpr BitBoard pieces(Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard pieces(Piece::Type p) const noexcept;
        [[nodiscard]] constexpr BitBoard pieces(Piece::Type p, Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard pawns() const noexcept;
        [[nodiscard]] constexpr BitBoard pawns(Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard knights() const noexcept;
        [[nodiscard]] constexpr BitBoard knights(Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard bishops() const noexcept;
        [[nodiscard]] constexpr BitBoard bishops(Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard rooks() const noexcept;
        [[nodiscard]] constexpr BitBoard rooks(Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard queens() const noexcept;
        [[nodiscard]] constexpr BitBoard queens(Colour c) const noexcept;
        [[nodiscard]] constexpr BitBoard kings() const noexcept;
        [[nodiscard]] constexpr BitBoard kings(Colour c) const noexcept;

        [[nodiscard]] constexpr bool is_piece_at(Square square) const noexcept;
        [[nodiscard]] constexpr Piece get_piece_at(Square square) const noexcept;

        [[nodiscard]] constexpr Square en_target() const noexcept;

        inline void set_queen_castle(Colour c, bool value) noexcept;
        inline void set_king_castle(Colour c, bool value) noexcept;
        [[nodiscard]] constexpr bool can_queen_castle(Colour c) const noexcept;
        [[nodiscard]] constexpr bool can_king_castle(Colour c) const noexcept;

        inline void put_piece(Piece piece, Square square) noexcept;
        inline void remove_piece(Square square) noexcept;

        [[nodiscard]] inline std::string fen() const noexcept;

        [[nodiscard]] constexpr Colour get_turn() const { return this->turn; }

    protected:
        // Bitboards stored with little-endian-rank-file-mapping
        std::array<BitBoard, 8> piece_BB;

        // Square behind the pawn that just made a two-square move.
        // If no such pawn exists, set to UINT_MAX
        Square en_passant_target;

        // Castling rights
        std::array<bool, 2> king_castling_rights;
        std::array<bool, 2> queen_castling_rights;
        size_t halfmove_clock, fullmove_clock;
        Colour turn;
    };

    inline std::ostream& print_board(std::ostream& stream, const BaseBoard& board);
    inline std::ostream& operator << (std::ostream& stream, const BaseBoard& board);

    class Board : public BaseBoard {
    public:
        inline Board(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move);

        [[nodiscard]] inline Move parse_uci(const std::string& uci) const;
        [[nodiscard]] inline Move parse_san(const std::string& san) const;
        [[nodiscard]] inline Move parse_move(Square from, Square to, Piece::Type promo_type = Piece::NO_TYPE) const noexcept;

        inline void push_uci(const std::string& uci);
        inline void push_san(const std::string& san);
        inline void push_move(Square from, Square to, Piece::Type promo_type = Piece::NO_TYPE) noexcept;
        inline void push_move(const Move& move) noexcept;

        inline void legal_moves(std::vector<Move>& moves) const noexcept;
        inline void legal_moves(std::vector<Move>& moves, BitBoard from_mask, BitBoard to_mask) const noexcept;
        [[nodiscard]] inline std::vector<Move> legal_moves() const noexcept;
        [[nodiscard]] inline std::vector<Move> legal_moves(BitBoard from_mask, BitBoard to_mask) const noexcept;

        [[nodiscard]] inline bool is_check() const noexcept;
        [[nodiscard]] inline bool is_check(Colour c) const noexcept;
        [[nodiscard]] inline bool is_draw() const noexcept;
        [[nodiscard]] inline bool is_draw_50_move() const noexcept;
        [[nodiscard]] inline bool is_draw_insufficient_material() const noexcept;

    private:
        template<Colour TurnColour>
        [[nodiscard]] Move parse_move(Square from, Square to, Piece::Type promo_type) const noexcept;

        template <Colour TurnColour>
        [[nodiscard]] Move parse_san(const std::string& san) const;

        template <Colour TurnColour>
        void push_move(const Move& m) noexcept;

        template <Colour TurnColour>
        void legal_moves(std::vector<Move>& moves) const noexcept;

        template <Colour TurnColour>
        void get_pawn_moves(std::vector<Move>& moves) const noexcept;
        template <Colour TurnColour>
        void get_knight_moves(std::vector<Move>& moves) const noexcept;
        template <Colour TurnColour>
        void get_bishop_moves(std::vector<Move>& moves) const noexcept;
        template <Colour TurnColour>
        void get_rook_moves(std::vector<Move>& moves) const noexcept;
        template <Colour TurnColour>
        void get_queen_moves(std::vector<Move>& moves) const noexcept;
        template <Colour TurnColour>
        void get_king_moves(std::vector<Move>& moves) const noexcept;

        template <Colour TurnColour>
        [[nodiscard]] bool move_in_check(const Move& move) const noexcept;
        template <Colour TurnColour>
        [[nodiscard]] bool is_in_check() const noexcept;

        template <Colour TurnColour>
        void add_pseudo_legal_pawn_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const noexcept;
        template <Colour TurnColour>
        void add_pseudo_legal_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const noexcept;

        [[nodiscard]] inline BitBoard knight_attacks(BitBoard knights) const noexcept;
        [[nodiscard]] inline BitBoard bishop_attacks(BitBoard bishops, BitBoard empty) const noexcept;
        [[nodiscard]] inline BitBoard rook_attacks(BitBoard rooks, BitBoard empty) const noexcept;
        [[nodiscard]] inline BitBoard queen_attacks(BitBoard queens, BitBoard empty) const noexcept;
        [[nodiscard]] inline BitBoard king_attacks(BitBoard kings) const noexcept;

        [[nodiscard]] inline bool move_in_check(const Move& move) const noexcept;

        inline void check_bb_sync() const noexcept;
    };
}


namespace chess {
    constexpr BitBoard BaseBoard::pieces() const noexcept {
        return this->piece_BB[static_cast<size_t>(Colour::WHITE)] |
               this->piece_BB[static_cast<size_t>(Colour::BLACK)];
    }
    constexpr BitBoard BaseBoard::pieces(Colour c) const noexcept {
        return this->piece_BB[static_cast<size_t>(c)];
    }
    constexpr BitBoard BaseBoard::pieces(Piece::Type p) const noexcept {
        return this->piece_BB[p];
    }
    constexpr BitBoard BaseBoard::pieces(Piece::Type p, Colour c) const noexcept {
        return this->piece_BB[p] & this->piece_BB[static_cast<size_t>(c)];
    }
    constexpr BitBoard BaseBoard::pawns() const noexcept {
        return this->pieces(Piece::PAWN);
    }
    constexpr BitBoard BaseBoard::pawns(Colour c) const noexcept {
        return this->pieces(Piece::PAWN, c);
    }
    constexpr BitBoard BaseBoard::knights() const noexcept {
        return this->pieces(Piece::KNIGHT);
    }
    constexpr BitBoard BaseBoard::knights(Colour c) const noexcept {
        return this->pieces(Piece::KNIGHT, c);
    }
    constexpr BitBoard BaseBoard::bishops() const noexcept {
        return this->pieces(Piece::BISHOP);
    }
    constexpr BitBoard BaseBoard::bishops(Colour c) const noexcept {
        return this->pieces(Piece::BISHOP, c);
    }
    constexpr BitBoard BaseBoard::rooks() const noexcept {
        return this->pieces(Piece::ROOK);
    }
    constexpr BitBoard BaseBoard::rooks(Colour c) const noexcept {
        return this->pieces(Piece::ROOK, c);
    }
    constexpr BitBoard BaseBoard::queens() const noexcept {
        return this->pieces(Piece::QUEEN);
    }
    constexpr BitBoard BaseBoard::queens(Colour c) const noexcept {
        return this->pieces(Piece::QUEEN, c);
    }
    constexpr BitBoard BaseBoard::kings() const noexcept {
        return this->pieces(Piece::KING);
    }
    constexpr BitBoard BaseBoard::kings(Colour c) const noexcept {
        return this->pieces(Piece::KING, c);
    }

    constexpr Square BaseBoard::en_target() const noexcept {
        return this->en_passant_target;
    }

    inline void BaseBoard::put_piece(Piece piece, Square square) noexcept {
        this->remove_piece(square);
        this->piece_BB[piece.type()] |= square;
        this->piece_BB[static_cast<size_t>(piece.colour())] |= square;
    }
    inline void BaseBoard::remove_piece(Square square) noexcept {
        for (int i = 0; i < 8; ++i)
            this->piece_BB[i] &= ~BitBoard(square);

        for (int i = 0; i < 8; ++i)
            assert(!this->piece_BB[i].is_piece_at(square));
    }

    void BaseBoard::set_queen_castle(Colour c, bool value) noexcept {
        this->queen_castling_rights[static_cast<size_t>(c)] = value;
    }
    void BaseBoard::set_king_castle(Colour c, bool value) noexcept {
        this->king_castling_rights[static_cast<size_t>(c)] = value;
    }
    [[nodiscard]] constexpr bool BaseBoard::can_queen_castle(Colour c) const noexcept {
        return this->queen_castling_rights[static_cast<size_t>(c)];
    }
    [[nodiscard]] constexpr bool BaseBoard::can_king_castle(Colour c) const noexcept {
        return this->king_castling_rights[static_cast<size_t>(c)];
    }

    constexpr bool BaseBoard::is_piece_at(Square square) const noexcept {
        for (auto& bb : this->piece_BB)
            if (bb.is_piece_at(square))
                return true;
        return false;
    }
    constexpr Piece BaseBoard::get_piece_at(Square square) const noexcept {
        for (size_t c = 0; c < 2; ++c) {
            if (!this->piece_BB[c].is_piece_at(square))
                continue;

            for (size_t t = 2; t < 8; ++t) {
                if (this->piece_BB[t].is_piece_at(square))
                    return Piece(static_cast<Piece::Type>(t), static_cast<Colour>(c));
            }
        }
        return Piece();
    }

    bool Board::is_check() const noexcept {
        return this->is_check(this->turn);
    }

    bool Board::is_check(Colour c) const noexcept {
        if (c == Colour::WHITE)
            return this->is_in_check<Colour::WHITE>();
        else
            return this->is_in_check<Colour::BLACK>();
    }

//    bool Board::is_draw() const {
//        return this->is_draw_50_move() || this->is_draw_insufficient_material();
//    }
    bool Board::is_draw_50_move() const noexcept {
        return this->halfmove_clock >= 50;
    }
    bool Board::is_draw_insufficient_material() const noexcept {
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
    bool Board::move_in_check(const Move& move) const noexcept {
        auto b = *this;
        b.push_move(move);
        return b.is_in_check<TurnColour>();
    }

    template <Colour TurnColour>
    bool Board::is_in_check() const noexcept {
        constexpr auto EnemyColour = Piece::enemy_colour(TurnColour);

        auto king_square = this->kings(TurnColour).first();

        if (!(PEXT_ATTACKS.bishop_attacks(king_square, this->pieces()) &
             (this->bishops(EnemyColour) | this->queens(EnemyColour))).empty())
            return true;

        if (!(PEXT_ATTACKS.rook_attacks(king_square, this->pieces()) &
              (this->rooks(EnemyColour) | this->queens(EnemyColour))).empty())
            return true;

        auto attacks = this->knight_attacks(this->knights(EnemyColour))
                       | this->king_attacks(this->kings(EnemyColour))
                       | this->pawns(EnemyColour).shift_NE(EnemyColour)
                       | this->pawns(EnemyColour).shift_NW(EnemyColour);
        return !(this->kings(TurnColour) & attacks).empty();
    }

    template <Colour TurnColour>
    void Board::add_pseudo_legal_pawn_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const noexcept {
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
    void Board::add_pseudo_legal_move(std::vector<Move>& moves, Square from, Square to, unsigned int flags) const noexcept {
        const auto move = Move(from, to, flags);
        if (!this->move_in_check<TurnColour>(move)) {
            moves.emplace_back(move);
        }
    }

    template <Colour TurnColour>
    void Board::get_pawn_moves(std::vector<Move>& moves) const noexcept {
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
    void Board::get_knight_moves(std::vector<Move>& moves) const noexcept {
        for (const auto from_position : this->knights(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::KNIGHT);
            auto move_bb = this->knight_attacks(BitBoard(from_position)) & ~this->pieces(TurnColour);
            for (const auto to_position : move_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->pieces(Piece::enemy_colour(TurnColour)).is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_bishop_moves(std::vector<Move>& moves) const noexcept {
        for (auto from_position : this->bishops(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::BISHOP);
            auto bishop_attacks = PEXT_ATTACKS.bishop_attacks(from_position, this->pieces());
            auto to_bb = bishop_attacks & ~this->pieces(TurnColour);
            for (const auto to_position : to_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->pieces(Piece::enemy_colour(TurnColour)).is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_rook_moves(std::vector<Move>& moves) const noexcept {
        for (auto from_position : this->rooks(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::ROOK);
            auto rook_attacks = PEXT_ATTACKS.rook_attacks(from_position, this->pieces());
            auto to_bb = rook_attacks & ~this->pieces(TurnColour);
            for (auto to_position : to_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->pieces(Piece::enemy_colour(TurnColour)).is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_queen_moves(std::vector<Move>& moves) const noexcept {
        for (const auto from_position : this->queens(TurnColour)) {
            assert(this->get_piece_at(from_position).colour() == TurnColour);
            assert(this->get_piece_at(from_position).type() == Piece::QUEEN);
            auto queen_attacks = PEXT_ATTACKS.rook_attacks(from_position, this->pieces()) |
                                 PEXT_ATTACKS.bishop_attacks(from_position, this->pieces());
            auto to_bb = queen_attacks & ~this->pieces(TurnColour);
            for (auto to_position : to_bb) {
                assert(!this->is_piece_at(to_position) || this->get_piece_at(to_position).colour() == Piece::enemy_colour(TurnColour));
                auto flags = this->pieces(Piece::enemy_colour(TurnColour)).is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
                this->add_pseudo_legal_move<TurnColour>(moves, from_position, to_position, flags);
            }
        }
    }
    template <Colour TurnColour>
    void Board::get_king_moves(std::vector<Move>& moves) const noexcept {
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
            auto flags = this->pieces(Piece::enemy_colour(TurnColour)).is_piece_at(to_position) ? Move::CAPTURE : Move::QUIET;
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
        [[maybe_unused]] auto matched = std::regex_match(san, match, SAN_REGEX);
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
            from_mask &= to_file(f);
        }

        // Filter by source rank
        if (!match[3].str().empty()) {
            auto r = match[3].str()[0] - '1';
            from_mask &= to_rank(r);
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
    void Board::push_move(const Move& move) noexcept {
        assert(this->is_piece_at(move.from()));
        assert(this->get_piece_at(move.from()).colour() == TurnColour);
        if (this->is_piece_at(move.to()))
            assert(this->get_piece_at(move.to()).colour() == Piece::enemy_colour<TurnColour>());
        [[maybe_unused]] auto type = this->get_piece_at(move.from()).type();  // For debug assertions later
        if (!this->en_passant_target.is_empty())
            assert(this->en_passant_target.rank() == orient<TurnColour>(Rank::R6));

        auto from_BB = BitBoard(move.from());
        auto to_BB = BitBoard(move.to());
        auto from_to_BB = from_BB ^ to_BB;
        auto piece = this->get_piece_at(move.from());

        auto captured_index = move.is_en_capture() ? move.to().backward(TurnColour) : move.to();
        auto captured_piece = this->get_piece_at(captured_index);
        auto captured_BB = BitBoard(captured_index);

        this->en_passant_target = Square::EMPTY;

        if (move.is_capture()) {
            if (move.to() != captured_index) {
                assert(this->is_piece_at(captured_index));
                assert(this->get_piece_at(captured_index).colour() == Piece::enemy_colour<TurnColour>());
            }
            this->piece_BB[captured_piece.type()] ^= captured_BB;
            this->piece_BB[static_cast<size_t>(captured_piece.colour())] ^= captured_BB;

            if (captured_piece.type() == Piece::ROOK) {
                if (captured_index == S::A8.orient(TurnColour))
                    this->set_queen_castle(Piece::enemy_colour(TurnColour), false);
                if (captured_index == S::H8.orient(TurnColour))
                    this->set_king_castle(Piece::enemy_colour(TurnColour), false);
            }
        }

        this->piece_BB[piece.type()] ^= from_to_BB;
        this->piece_BB[static_cast<size_t>(piece.colour())] ^= from_to_BB;

        if (move.is_promotion()) {
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
        if (!move.is_promotion())
            assert(this->get_piece_at(move.to()).type() == type);
        else
            assert(this->get_piece_at(move.to()).type() == move.promoted_type());
        if (captured_index.get() != move.to().get())
            assert(!this->is_piece_at(captured_index));
        if (this->en_passant_target.get() != Square::EMPTY)
            assert(this->en_passant_target.rank() == orient<TurnColour>(Rank::R3));
        this->check_bb_sync();
    }

    template <Colour TurnColour>
    void Board::legal_moves(std::vector<Move>& moves) const noexcept {
        moves.clear();
        this->get_pawn_moves<TurnColour>(moves);
        this->get_knight_moves<TurnColour>(moves);
        this->get_bishop_moves<TurnColour>(moves);
        this->get_rook_moves<TurnColour>(moves);
        this->get_queen_moves<TurnColour>(moves);
        this->get_king_moves<TurnColour>(moves);
    }


    inline std::ostream& print_board(std::ostream& stream, const BaseBoard& board) {
        for (int ri = 7; ri >= 0; --ri) {
            for (int fi = 0; fi < 8; ++fi) {
                auto r = to_rank(ri);
                auto f = to_file(fi);
                auto piece = board.get_piece_at(Square(f, r));

                stream << piece_repr(piece);
            }
            stream << '\n';
        }
        return stream;
    }
    inline std::ostream& operator << (std::ostream& stream, const BaseBoard& board) {
        return print_board(stream, board);
    }


    BaseBoard::BaseBoard(Colour turn, std::string_view castle_rights, Square en_passant_target, unsigned int half_move, unsigned int full_move) noexcept
            : en_passant_target(en_passant_target), halfmove_clock(half_move), fullmove_clock(full_move), turn(turn)
    {
        this->set_king_castle(Colour::WHITE, castle_rights.find_first_of('K') != std::string::npos);
        this->set_king_castle(Colour::BLACK, castle_rights.find_first_of('k') != std::string::npos);
        this->set_queen_castle(Colour::WHITE, castle_rights.find_first_of('Q') != std::string::npos);
        this->set_queen_castle(Colour::BLACK, castle_rights.find_first_of('q') != std::string::npos);
    }

    void BaseBoard::clear() noexcept {
        for (int i = 0; i < 8; ++i)
            this->piece_BB[i] = 0;
        this->en_passant_target = Square();

        for(int i = 0; i < 8; ++i)
            assert(this->piece_BB[i].empty());
        assert(this->en_passant_target == Square::EMPTY);
    }

    std::string BaseBoard::fen() const noexcept {
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

    template <Colour TurnColour>
    Move Board::parse_move(Square from, Square to, Piece::Type promo_type) const noexcept {
        auto piece = this->get_piece_at(from);
        auto flags = Move::QUIET;

        assert(piece.type() != Piece::NO_TYPE);
        assert(piece.colour() == TurnColour);
        if (this->is_piece_at(to))
            assert(this->get_piece_at(to).colour() != TurnColour);

        if (piece.type() == Piece::PAWN) {
            if (from.rank() == orient<TurnColour>(Rank::R2) && to.rank() == orient<TurnColour>(Rank::R4)) {
                flags |= Move::D_P_PUSH;
            }
            else if (to == this->en_passant_target) {
                flags |= Move::EN_CAPTURE;
            }
        }

        if (piece.type() == Piece::KING) {
            if (from == S::E1.orient(TurnColour) && to == S::G1.orient(TurnColour)) {
                flags |= Move::K_CASTLE;
            }
            else if (from == S::E1.orient(TurnColour) && to == S::C1.orient(TurnColour)) {
                flags |= Move::Q_CASTLE;
            }
        }

        if (this->is_piece_at(to)) {
            flags |= Move::CAPTURE;
        }

        return Move(from, to, flags, promo_type);
    }

    Move Board::parse_move(Square from, Square to, Piece::Type promo_type) const noexcept {
        if (this->turn == Colour::WHITE)
            return this->parse_move<Colour::WHITE>(from, to, promo_type);
        else
            return this->parse_move<Colour::BLACK>(from, to, promo_type);
    }

    void Board::push_uci(const std::string& uci) {
        this->push_move(this->parse_uci(uci));
    }
    void Board::push_san(const std::string& san) {
        this->push_move(this->parse_san(san));
    }
    void Board::push_move(Square from, Square to, Piece::Type promo_type) noexcept {
        this->push_move(this->parse_move(from, to, promo_type));
    }
    void Board::push_move(const Move& move) noexcept {
        switch (this->turn) {
            case Colour::WHITE: this->push_move<Colour::WHITE>(move); break;
            case Colour::BLACK: this->push_move<Colour::BLACK>(move); break;
        }
    }

    void Board::legal_moves(std::vector<Move>& moves) const noexcept {
        switch (this->turn) {
            case Colour::WHITE: this->legal_moves<Colour::WHITE>(moves); break;
            case Colour::BLACK: this->legal_moves<Colour::BLACK>(moves); break;
        }
    }
    void Board::legal_moves(std::vector<Move>& moves, BitBoard from_mask, BitBoard to_mask) const noexcept {
        moves.clear();
        for (const auto& move : this->legal_moves()) {
            if (from_mask.contains(move.from()) && to_mask.contains(move.to())) {
                moves.push_back(move);
            }
        }
    }

    std::vector<Move> Board::legal_moves() const noexcept {
        std::vector<Move> moves;
        this->legal_moves(moves);
        return moves;
    }

    std::vector<Move> Board::legal_moves(BitBoard from_mask, BitBoard to_mask) const noexcept {
        std::vector<Move> moves;
        this->legal_moves(moves, from_mask, to_mask);
        return moves;
    }

    BitBoard Board::knight_attacks(const BitBoard knights) const noexcept {
        auto w1 = knights.shift_W();
        auto w2 = knights.shift_W(2);
        auto e1 = knights.shift_E();
        auto e2 = knights.shift_E(2);
        auto m1 = e1 | w1;
        auto m2 = e2 | w2;
        return m1.shift_N(2) | m1.shift_S(2) | m2.shift_N() | m2.shift_S();
    }
    BitBoard Board::bishop_attacks(const BitBoard bishops, const BitBoard empty) const noexcept {
        return bishops.attack_NE_occluded(empty) | bishops.attack_NW_occluded(empty) |
               bishops.attack_SE_occluded(empty) | bishops.attack_SW_occluded(empty);
    }
    BitBoard Board::rook_attacks(const BitBoard rooks, const BitBoard empty) const noexcept {
        return rooks.attack_N_occluded(empty) | rooks.attack_E_occluded(empty) |
               rooks.attack_S_occluded(empty) | rooks.attack_W_occluded(empty);
    }
    BitBoard Board::queen_attacks(const BitBoard queens, const BitBoard empty) const noexcept {
        return this->bishop_attacks(queens, empty) | this->rook_attacks(queens, empty);
    }
    BitBoard Board::king_attacks(const BitBoard kings) const noexcept {
        return kings.shift_N() | kings.shift_NE() | kings.shift_E() | kings.shift_SE()
               | kings.shift_S() | kings.shift_SW() | kings.shift_W() | kings.shift_NW();
    }

    void Board::check_bb_sync() const noexcept {
        for (int i = 0; i < 64; ++i) {
            auto piece = this->get_piece_at(i);
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