#include "perft.hpp"


namespace chess {

    size_t perft(const Board& board, size_t depth) {
        std::vector<std::vector<Move>> storage(depth, std::vector<Move>());
        return perft(board, depth, storage);
    }

    size_t perft(const Board& board,
                        size_t depth,
                        std::vector<std::vector<Move>>& storage) {
        if (depth == 0) return 1;

        auto& moves = storage[depth-1];
        board.legal_moves(moves);

        if (depth == 1)
            return moves.size();

        size_t nodes = 0;
        for (auto& move : moves) {
            Board b = board;
            b.push_move(move);
            nodes += perft(b, depth - 1, storage);
        }

        return nodes;
    }


}