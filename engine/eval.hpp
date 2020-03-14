#pragma once

#include "engine.hpp"
#include "backend/chess.hpp"

#include <string_view>

namespace engine {

    class EvalResult {
    public:
        int wins, losses, draws;
    };

    EvalResult eval(Engine& engine, Engine& opponent, size_t trials = 1, std::string_view starting_fen = chess::STARTING_FEN);

}