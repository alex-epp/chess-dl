#pragma once

#include "../backend/chess.h"

#include <optional>


namespace engine {

    class EndgameTable {
    public:
        EndgameTable() = default;
        virtual ~EndgameTable() = default;

        virtual std::optional<chess::Move> query(const chess::Board& board) = 0;
    };

}