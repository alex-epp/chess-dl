#include <catch2/catch.hpp>

#include <chess/backend/chess.hpp>
#include <chess/engine/engine.hpp>

using namespace chess;
using namespace engine;

namespace tests
{
    TEST_CASE("initial") {
        auto board = load_FEN<Board>(STARTING_FEN);
        MMappedOpeningBook opening_book("../../../assets/gm2001.bin");
        auto move = opening_book.query(board);
        REQUIRE(move.has_value());
        REQUIRE(move->from() == S::E2);
        REQUIRE(move->to() == S::E4);
    }
}
