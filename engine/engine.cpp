#include "engine.hpp"

#include <utility>

namespace engine {

    Engine::Engine(std::shared_ptr<OpeningBook> opening_book, std::shared_ptr<EndgameTable> endgame_table)
            : opening_book(std::move(opening_book)), endgame_table(std::move(endgame_table)) {}

    chess::Move Engine::move(const chess::Board& board) {
        if (this->opening_book) {
            auto opening_book_move = this->opening_book->query(board);
            if (opening_book_move)
                return *opening_book_move;
        }

        if (this->endgame_table) {
            auto endgame_table_move = this->endgame_table->query(board);
            if (endgame_table_move)
                return *endgame_table_move;
        }

        return this->move_impl(board);
    }

}
