#pragma once

#include "opening_book.hpp"
#include "endgame_table.hpp"
#include <chess/backend/chess.hpp>

#include <filesystem>
#include <memory>


namespace engine {

    class Engine {
    public:
        explicit Engine(std::shared_ptr<OpeningBook> opening_book = nullptr,
                        std::shared_ptr<EndgameTable> endgame_table = nullptr);

        virtual chess::Move move(const chess::Board& board);

    protected:
        virtual chess::Move move_impl(const chess::Board& board) = 0;

    private:
        std::shared_ptr<OpeningBook> opening_book;
        std::shared_ptr<EndgameTable> endgame_table;
    };

}