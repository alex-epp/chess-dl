#pragma once

#include "../backend/chess.h"

#include <optional>


namespace engine {

    class OpeningBook {
    public:
        OpeningBook() = default;
        virtual ~OpeningBook() = default;

        virtual std::optional<chess::Move> query(const chess::Board& board) = 0;
    };

}