file(GLOB HEADER_LIST
        CONFIGURE_DEPENDS
        "${chess_SOURCE_DIR}/include/chess/backend/*.hpp")

file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "${chess_SOURCE_DIR}/src/backend/*.cpp")

add_library(backend ${SOURCE_LIST} ${HEADER_LIST})
target_include_directories(backend PUBLIC "${chess_SOURCE_DIR}/include")
target_compile_features(backend PUBLIC cxx_std_17)
if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(backend PUBLIC -mbmi2)
endif()
