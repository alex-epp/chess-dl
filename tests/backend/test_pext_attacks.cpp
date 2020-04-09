#include <catch2/catch.hpp>
#include "tests.hpp"

#include <chess/backend/chess.hpp>

using namespace chess;


namespace tests
{
    TEST_CASE("pext") {
        REQUIRE(PEXT_ATTACKS.rook_attacks(S::A1, 0) == ((BB::A | BB::R1) & ~BB::A1));
        REQUIRE(PEXT_ATTACKS.rook_attacks(S::A1, BB::B2) == ((BB::A | BB::R1) & ~BB::A1));
    };
}
