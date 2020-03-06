add_executable(demo "demo.cpp" "../bitboard.cpp" "../chess.cpp")
set_property(TARGET demo PROPERTY CXX_STANDARD 17)
set_property(TARGET demo PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
