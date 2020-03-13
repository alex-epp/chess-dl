cmake_minimum_required (VERSION 3.8)

project(chess CXX)

find_package(Torch REQUIRED PATHS /home/ubuntu/libtorch/share/cmake/Torch)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
include_directories(train "${TORCH_INCLUDE_DIRS}")
message(STATUS "TORCH_CXX_FLAGS: ${TORCH_CXX_FLAGS}")
message(STATUS "TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")
message(STATUS "TORCH_LIBRARIES: ${TORCH_LIBRARIES}")

add_subdirectory("backend")
add_subdirectory("engine")
add_subdirectory("setup")
add_subdirectory("train")

