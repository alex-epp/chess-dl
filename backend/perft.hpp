#pragma once


#include "board.hpp"
#include "zobrist_hash.hpp"

#include <map>
#include <vector>

namespace chess {

    size_t perft(const Board& board, size_t depth);

    size_t perft(const Board& board,
                 size_t depth,
                 std::vector<std::vector<Move>>& storage);

}