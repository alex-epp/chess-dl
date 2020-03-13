#include "eval.hpp"


namespace engine {
    static bool is_game_over(const chess::Board& board) {
        return board.legal_moves().empty();
    }


    EvalResult eval(Engine& engine, Engine& opponent, size_t trials, std::string_view starting_fen) {
        EvalResult result = {0, 0, 0};
        for (size_t i = 0; i < trials; ++i) {
            auto board = chess::load_FEN<chess::Board>(starting_fen);
            while (!is_game_over(board)) {
                board.push_move(engine.move(board));
                if (is_game_over(board)) break;
                board.push_move(opponent.move(board));
            }
            // TODO: the 50-move breaks this, because the game can end in a draw in check. The fix is to check one of the clocks here
            if (board.is_check(chess::Colour::WHITE)) ++result.wins;
            else if (board.is_check(chess::Colour::BLACK)) ++result.losses;
            else ++result.draws;
        }
        return result;
    }

}