add_executable(pgn_minify pgn_minify.cpp)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(pgn_minify stdc++fs)
endif()
