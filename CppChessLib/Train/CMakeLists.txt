# CMakeList.txt : CMake project for Train, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_LIST_DIR}/cmake")
find_package(Torch REQUIRED)
message(STATUS "TORCH_CXX_FLAGS: ${TORCH_CXX_FLAGS}")
message(STATUS "TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")
message(STATUS "TORCH_LIBRARIES: ${TORCH_LIBRARIES}")
find_package(STXXL REQUIRED)
message(STATUS "STXXL_CXX_FLAGS: ${STXXL_CXX_FLAGS}")
message(STATUS "STXXL_INCLUDE_DIRS: ${STXXL_INCLUDE_DIRS}")
message(STATUS "STXXL_LIBRARIES: ${STXXL_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

# Add source to this project's executable.
add_executable (Train "Train.cpp")
target_link_libraries(Train "${TORCH_LIBRARIES}")
set_property(TARGET Train PROPERTY CXX_STANDARD 17)

# TODO: Add tests and install targets if needed.
