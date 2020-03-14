#include <catch2/catch.hpp>

#include "../chess.hpp"
#include "../fen.hpp"

using namespace chess;


namespace tests
{
	TEST_CASE("TestSAN") {
		SECTION("Initial") {
		    const auto board = chess::load_FEN<Board>(chess::STARTING_FEN);
		    auto move = board.parse_san("d4");
		    REQUIRE(move.from() == S::D2);
		    REQUIRE(move.to() == S::D4);
		    REQUIRE(move.flags() == Move::D_P_PUSH);
		}

        SECTION("Regression_1") {
            auto board = chess::load_FEN<Board>(chess::STARTING_FEN); board.push_san("d4");
            auto move = board.parse_san("d5");
            REQUIRE(move.from() == S::D7);
            REQUIRE(move.to() == S::D5);
            REQUIRE(move.flags() == Move::D_P_PUSH);
        }

        SECTION("Regression_2") {
		    // "1. d4  d5  2. Nf3  Nf6  3. g3  Nc6  4. Bg2  Bf5  5. a3  e5  6. dxe5  Nd7  7. Qxd5  Qe7  8. Bf4  O-O-O  9. Qb5  h6  10. c4  g5  11. Be3  a6  12. Qb3  Na5  13. Qc3  Nc6  14. b4  Ncxe5  15. O-O  Bg7  16. Bd4  Nxf3  17. exf3  Ne5  18. Re1  Rhe8  19. Ra2  Rxd4  20. Rae2  Nxf3  21. Bxf3  Qd7  22. Rxe8  Qxe8  23. Rxe8  Kd7  24. Qe3  Be6  25. Rg8  Bf6  26. Nd2  Rd6  27. Bxb7  Bd4  28. Qf3  Bc3  29. Ne4  1-0"
		    // Errors on "17. exf3 ..."
            const auto board = chess::load_FEN<Board>("2kr3r/1ppnqpb1/p6p/5bp1/1PPB4/P1Q2nP1/4PPBP/RN3RK1 w Qk - 0 1");
            auto move = board.parse_san("exf3");
            REQUIRE(move.from() == S::E2);
            REQUIRE(move.to() == S::F3);
            REQUIRE(move.flags() == Move::CAPTURE);
        }

        SECTION("Regression_3") {
            // "1. e4  b6  2. d4  Bb7  3. c3  e6  4. Nf3  Bxe4  5. Bc4  Nf6  6. Bg5  Be7  7. O-O  Bd5  8. Bd3  Be4  9. Re1  Bxd3  10. Qxd3  Nd5  11. Bxe7  Nxe7  12. Nbd2  Ng6  13. Ne4  f6  14. Ng3  Kf7  15. Rad1  Nc6  16. c4  Nce7  17. d5  exd5  18. cxd5  d6  19. Nf5  Nxf5  20. Qxf5  Qf8  21. Re6  Re8  22. Rde1  Ne7  23. Qd3  g6  24. R1e4  Nf5  25. Qe2  Rxe6  26. Rxe6  Qd8  27. Qe4  Re8  28. Rxe8  Qxe8  29. Qd3  g5  30. h3  Ne7  31. Qd4  Kg7  32. Qd3  Qc8  33. Nd2  Kh6  34. Ne4  Nf5  35. Qc3  Nh4  36. Nxf6  Nxg2  37. Qg3  Ne3  38. Qxe3  Kg6  39. Qf3  Qf5  40. Qd3  Qxd3  41. Kg2  Kxf6  42. Kh2  1-0"
            // Errors on "24. R1e4 ..."
            const auto board = chess::load_FEN<Board>("4rq1r/p1p1nk1p/1p1pRpp1/3P4/8/3Q1N2/PP3PPP/4R1K1 w - - 0 24");
            const auto move = board.parse_san("R1e4");
            REQUIRE(move.from() == S::E1);
            REQUIRE(move.to() == S::E4);
            REQUIRE(move.flags() == Move::QUIET);
        }

        SECTION("Regression_4") {
		    // "1. e4  e5  2. Nf3  Nc6  3. d4  exd4  4. Nxd4  Nxd4  5. Qxd4  c5  6. Qd3  h6  7. Be2  Nf6  8. Nc3  Be7  9. Qg3  g5  10. e5  Nh7  11. O-O  O-O  12. f4  f6  13. f5  d6  14. e6  a6  15. Bh5  b5  16. Nd5  Bb7  17. Bf7  Kg7  18. Nxe7  Qxe7  19. Qh3  Rxf7  20. exf7  Qxf7  21. Be3  Qd5  22. Rad1  Qe5  23. c3  Bc8  24. Rde1  Qd5  25. Qf3  Qxf3  26. Rxf3  Bb7  27. Rf2  Re8  28. Rfe2  Nf8  29. Bf2  Rxe2  30. Rxe2  Kf7  31. b3  Nd7  32. Re6  d5  33. b4  c4  34. Bd4  Ne5  35. Bxe5  fxe5  36. Rxh6  e4  37. Rh7  Kf6  38. Rxb7  Kxf5  39. Kf2  Kf4  40. g3  Ke5  41. Ke3  Kd6  42. Rb6  Kc7  43. Rxa6  Kb7  44. Rg6  Kc7  45. Rxg5  Kd6  46. Rf5  Ke6  47. g4  Kd6  48. h4  Ke6  49. h5  Kd6  50. h6  Ke6  51. h7  Kd6  52. h8=Q  Ke6  53. Qh6  Ke7  54. Qf6  Kd7  55. Rg5  Kc7  56. Rg7  Kc8  57. Qf8  1-0"
		    // Errors on "52. h8=Q ..."
            const auto board = chess::load_FEN<Board>("8/7P/3k4/1p1p1R2/1Pp1p1P1/2P1K3/P7/8 w - - 1 52");
            const auto move = board.parse_san("h8=Q");
            REQUIRE(move.from() == S::H7);
            REQUIRE(move.to() == S::H8);
            REQUIRE(move.flags() == Move::Q_PROMOTION);
		}
	};
}
