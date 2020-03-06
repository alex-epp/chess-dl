cmake_minimum_required (VERSION 3.8)

project(chess CXX)

# LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/libtorch)

# set(Torch_DIR ${CMAKE_SOURCE_DIR}/lib/libtorch/share/cmake/Torch)

add_subdirectory("backend")
add_subdirectory("setup")
add_subdirectory("train")
