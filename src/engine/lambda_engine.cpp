#include <chess/engine/lambda_engine.hpp>

#include <utility>

namespace engine {

    LambdaEngine::LambdaEngine(Lambda lambda) : lambda(std::move(lambda)) {}

    chess::Move LambdaEngine::move_impl(const chess::Board& board) {
        return this->lambda(board);
    }

}