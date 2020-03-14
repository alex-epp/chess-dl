#include "zobrist_hash.hpp"

namespace chess {

    ZobristHasher::ZobristHasher(std::array<std::uint64_t, 781> random_numbers)
            : random_numbers(random_numbers) {}

    ZobristHasher::Hash ZobristHasher::hash_board(const chess::Board& board) {
        Hash hash = 0;

        for (auto square : board.pieces(Colour::WHITE)) {
            auto piece = board.get_piece_at(square);
            size_t piece_index = (piece.type() - Piece::PAWN) * 2 + 1;
            hash ^= this->random_numbers[64 * piece_index + square.get()];
        }
        for (auto square : board.pieces(Colour::BLACK)) {
            auto piece = board.get_piece_at(square);
            size_t piece_index = (piece.type() - Piece::PAWN) * 2;
            hash ^= this->random_numbers[64 * piece_index + square.get()];
        }

        return hash;
    }

    ZobristHasher::Hash ZobristHasher::hash_castling(const Board& board) {
        Hash hash = 0;

        if (board.can_king_castle(Colour::WHITE))
            hash ^= this->random_numbers[768 + 0];
        if (board.can_queen_castle(Colour::WHITE))
            hash ^= this->random_numbers[768 + 1];
        if (board.can_king_castle(Colour::BLACK))
            hash ^= this->random_numbers[768 + 2];
        if (board.can_queen_castle(Colour::BLACK))
            hash ^= this->random_numbers[768 + 3];

        return hash;
    }

    ZobristHasher::Hash ZobristHasher::hash_ep_square(const Board& board) {
        if (!board.en_target().is_empty()) {
            chess::Colour turn = board.get_turn();
            chess::BitBoard en_mask = board.en_target().south(turn);
            en_mask = en_mask.shift_E() | en_mask.shift_W();
            if (!(en_mask & board.pawns(turn)).empty()) {
                auto file = static_cast<size_t>(board.en_target().file());
                return this->random_numbers[772 + file];
            }
        }
        return 0;
    }

    ZobristHasher::Hash ZobristHasher::hash_turn(const Board& board) {
        return board.get_turn() == Colour::WHITE ? this->random_numbers[780] : 0;
    }

    ZobristHasher::Hash ZobristHasher::operator()(const Board& board) {
        return this->hash_board(board) ^ this->hash_castling(board) ^
               this->hash_ep_square(board) ^ this->hash_turn(board);
    }
}


