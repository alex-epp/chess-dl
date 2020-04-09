#pragma once

#include <chess/backend/chess.hpp>

#include <optional>


namespace engine {

    class EndgameTable {
    public:
        EndgameTable() = default;
        virtual ~EndgameTable() = default;

        virtual std::optional<chess::Move> query(const chess::Board& board) = 0;
    };

}