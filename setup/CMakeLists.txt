add_executable(pgn_minify "pgn_minify.cpp")
target_include_directories(pgn_minify PUBLIC progress-cpp)
target_link_libraries(pgn_minify stdc++fs)
