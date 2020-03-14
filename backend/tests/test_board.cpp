#include <catch2/catch.hpp>

#include "../chess.hpp"
#include "../fen.hpp"

using namespace chess;


namespace tests
{
    TEST_CASE("move_regression_1") {
        auto board = chess::load_FEN<Board>("rnb1qknr/pppp2pp/7N/3Qp3/1PP2P2/B3P3/P3P1PP/RN2KB1R w KQq - 9 13");
        const auto move = chess::Move(S::A3, S::C1);
        board.push_move(move);
        REQUIRE(board.fen() == "rnb1qknr/pppp2pp/7N/3Qp3/1PP2P2/4P3/P3P1PP/RNB1KB1R b KQq - 10 13");
    }

    TEST_CASE("move_regression_2") {
        auto board = chess::load_FEN<Board>("r1bqkbnr/1pppp1p1/7p/p2B1p2/1n2P3/2N5/PPPP1PPP/R1BQK1NR w KQkq - 0 8");
        board.push_move(chess::Move(S::E1, S::F1));
        REQUIRE(board.fen() == "r1bqkbnr/1pppp1p1/7p/p2B1p2/1n2P3/2N5/PPPP1PPP/R1BQ1KNR b kq - 1 8");
        board.push_move(chess::Move(S::A5, S::A4));
        REQUIRE(board.fen() == "r1bqkbnr/1pppp1p1/7p/3B1p2/pn2P3/2N5/PPPP1PPP/R1BQ1KNR w kq - 0 9");

        auto legal_moves = board.legal_moves();
        REQUIRE(std::find(legal_moves.begin(), legal_moves.end(), chess::Move(S::E1, S::F1)) == legal_moves.end());

        board.push_move(chess::Move(S::F1, S::E1));
        REQUIRE(board.fen() == "r1bqkbnr/1pppp1p1/7p/3B1p2/pn2P3/2N5/PPPP1PPP/R1BQK1NR b kq - 1 9");
    }
}
