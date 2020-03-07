add_executable(tests "tests.cpp" "test_bb.cpp" "test_bitscan.cpp" "test_mailbox.cpp" "test_move.cpp" "test_perft.cpp"
                     "../bitboard.cpp" "../chess.cpp")
set_property(TARGET tests PROPERTY CXX_STANDARD 17)
set_property(TARGET tests PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
