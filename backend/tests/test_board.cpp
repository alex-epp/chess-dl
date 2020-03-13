#include "catch.hpp"

#include "../chess.h"
#include "../fen.hpp"

using namespace chess;


namespace tests
{
    TEST_CASE("move_regression_1") {
        auto board = chess::load_FEN<Board>("rnb1qknr/pppp2pp/7N/3Qp3/1PP2P2/B3P3/P3P1PP/RN2KB1R w KQq - 9 13");
        const auto move = chess::Move(S::A3, S::C1);
        board.push_move(move);
        board.flip();
        REQUIRE(board.fen() == "rnb1kb1r/p3p1pp/4p3/1pp2p2/3qP3/7n/PPPP2PP/RNB1QKNR w k - 10 14");
    }
}
