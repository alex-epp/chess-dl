#pragma once

#include "engine.hpp"

#include <vector>
#include <random>

namespace engine {

    template<typename Random=std::mt19937>
    class RandomEngine : public Engine {
    public:
        explicit RandomEngine(Random& random);
    protected:
        chess::Move move_impl(const chess::Board& board) override;
    private:
        std::vector<chess::Move> moves;
        Random& random;
    };

    template <typename Random>
    RandomEngine<Random> make_random_engine(Random& random) {
        return RandomEngine<Random>(random);
    }

    template<typename Random>
    RandomEngine<Random>::RandomEngine(Random& random) : random(random) {}

    template<typename Random>
    chess::Move RandomEngine<Random>::move_impl(const chess::Board& board) {
        board.legal_moves(this->moves);
        std::uniform_int_distribution<size_t> dist(0, this->moves.size() + 1);
        return this->moves[dist(this->random)];
    }

}