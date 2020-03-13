#pragma once

#include "engine.hpp"

namespace engine {

    class LambdaEngine : public Engine {
    public:
        using Lambda = std::function<chess::Move(const chess::Board&)>;
        explicit LambdaEngine(Lambda lambda);
    protected:
        chess::Move move_impl(const chess::Board& board) override;
    private:
        const Lambda lambda;
    };

}