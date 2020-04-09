file(GLOB HEADER_LIST
        CONFIGURE_DEPENDS
        "${chess_SOURCE_DIR}/include/chess/engine/*.hpp")

file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "${chess_SOURCE_DIR}/src/engine/*.cpp")

add_library(engine ${SOURCE_LIST} ${HEADER_LIST})
target_compile_features(engine PUBLIC cxx_std_17)
target_include_directories(engine PUBLIC "${chess_SOURCE_DIR}/include")
target_link_libraries(engine PUBLIC backend mio)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_link_libraries(engine PUBLIC stdc++fs)
endif()
