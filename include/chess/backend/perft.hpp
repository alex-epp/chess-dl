#pragma once


#include "board.hpp"
#include "zobrist_hash.hpp"

#include <map>
#include <vector>
#include <thread>
#include <future>

namespace chess {

    inline size_t perft(const Board& board, size_t depth);
    inline size_t perft(const Board& board,
                 size_t depth,
                 std::vector<std::vector<Move>>& storage);
    inline size_t perft_cache(const Board& board, size_t depth);
    inline size_t perft_cache(const Board& board,
                       size_t depth,
                       std::vector<std::vector<Move>>& storage,
                       const ZobristHasher& hasher,
                       std::map<std::pair<ZobristHasher::Hash, size_t>, size_t>& cache);
    inline size_t perft_parallel(const Board& board, size_t depth);


    inline size_t perft(const Board& board, size_t depth) {
        std::vector<std::vector<Move>> storage(depth, std::vector<Move>());
        return perft(board, depth, storage);
    }
    inline size_t perft(const Board& board,
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
    inline size_t perft_cache(const Board& board, size_t depth) {
        std::vector<std::vector<Move>> storage(depth, std::vector<Move>());
        std::map<std::pair<ZobristHasher::Hash, size_t>, size_t> cache;
        const ZobristHasher hasher;
        return perft_cache(board, depth, storage, hasher, cache);
    }
    inline size_t perft_cache(const Board& board,
                       size_t depth,
                       std::vector<std::vector<Move>>& storage,
                       const ZobristHasher& hasher,
                       std::map<std::pair<ZobristHasher::Hash, size_t>, size_t>& cache) {
        if (depth == 0) return 1;

        auto hash = std::make_pair(hasher(board), depth);

        auto it = cache.find(hash);
        if (it != cache.end())
            return it->second;

        auto& moves = storage[depth-1];
        board.legal_moves(moves);

        if (depth == 1) {
            cache[hash] = moves.size();
            return moves.size();
        }

        size_t nodes = 0;
        for (auto& move : moves) {
            Board b = board;
            b.push_move(move);
            nodes += perft_cache(b, depth - 1, storage, hasher, cache);
        }

        cache[hash] = nodes;
        return nodes;
    }
    inline size_t perft_parallel(const Board& board, size_t depth) {

        if (depth == 0) return 1;

        std::vector<std::future<size_t>> worker_nodes;
        for (auto& move : board.legal_moves()) {
            worker_nodes.emplace_back(std::async([=]() {
                auto b = board;
                b.push_move(move);
                return perft_cache(b, depth-1);
            }));
        }
        size_t nodes = 0;
        for (auto& n : worker_nodes) nodes += n.get();
        return nodes;
    }
}