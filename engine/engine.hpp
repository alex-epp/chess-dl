#pragma once

#include "opening_book.hpp"
#include "endgame_table.hpp"

#include <filesystem>
#include <memory>

#include "../backend/chess.hpp"

namespace engine {

    class Engine {
    public:
        Engine(std::unique_ptr<OpeningBook> opening_book = nullptr,
               std::unique_ptr<EndgameTable> endgame_table = nullptr);

        virtual chess::Move move(const chess::Board& board);

    protected:
        virtual chess::Move move_impl(const chess::Board& board) = 0;

    private:
        std::unique_ptr<OpeningBook> opening_book;
        std::unique_ptr<EndgameTable> endgame_table;
    };

}