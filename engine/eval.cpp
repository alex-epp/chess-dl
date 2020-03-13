#include "eval.hpp"


namespace engine {
    static bool is_game_over(const chess::Board& board) {
        return board.is_draw_50_move() || board.is_draw_insufficient_material() ||
            board.legal_moves().empty();
    }


    EvalResult eval(Engine& engine, Engine& opponent, size_t trials, std::string_view starting_fen) {
        EvalResult result = {0, 0, 0};
        for (size_t i = 0; i < trials; ++i) {
            auto board = chess::load_FEN<chess::Board>(starting_fen);
            while (!is_game_over(board)) {
                auto move = engine.move(board);
                board.push_move(move);
                if (is_game_over(board)) break;
                move = opponent.move(board);
                board.push_move(move);
            }
            if (board.is_draw_50_move() || board.is_draw_insufficient_material()) ++result.draws;
            else if (board.is_check(chess::Colour::WHITE)) ++result.wins;
            else if (board.is_check(chess::Colour::BLACK)) ++result.losses;
            else ++result.draws;  // Stalemate
        }
        return result;
    }

}