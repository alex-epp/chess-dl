#pragma once

#include "../backend/chess.hpp"

#include <optional>


namespace engine {

    class OpeningBook {
    public:
        OpeningBook() = default;
        virtual ~OpeningBook() = default;

        virtual std::optional<chess::Move> query(const chess::Board& board) = 0;
    };

}