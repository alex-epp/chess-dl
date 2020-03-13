#pragma once

#include "engine.hpp"

#include <vector>
#include <random>

namespace engine {

    template<typename Random=std::mt19937>
    class RandomEngine : public Engine {
    public:
        explicit RandomEngine(Random& random,
                              std::unique_ptr<OpeningBook> opening_book = nullptr,
                              std::unique_ptr<EndgameTable> endgame_table = nullptr);
    protected:
        chess::Move move_impl(const chess::Board& board) override;
    private:
        std::vector<chess::Move> moves;
        Random& random;
    };

    template <typename Random, typename... Args>
    RandomEngine<Random> make_random_engine(Random& random, Args&&... args) {
        return RandomEngine<Random>(random, std::forward<Args>(args)...);
    }

    template<typename Random>
    RandomEngine<Random>::RandomEngine(
            Random& random,
            std::unique_ptr<OpeningBook> opening_book,
            std::unique_ptr<EndgameTable> endgame_table)
            : Engine(std::move(opening_book), std::move(endgame_table)),
              random(random) {}

    template<typename Random>
    chess::Move RandomEngine<Random>::move_impl(const chess::Board& board) {
        board.legal_moves(this->moves);
        std::uniform_int_distribution<size_t> dist(0, this->moves.size() - 1);
        auto i = dist(this->random);
        assert(0 <= i);
        assert(i < this->moves.size());
        return this->moves[i];
    }

}