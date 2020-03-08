file(GLOB_RECURSE TRAIN_SOURCES
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

file(GLOB BACKEND_SOURCES
        CONFIGURE_DEPENDS
        "../backend/*.cpp"
        "../backend/*.hpp"
        "../backend/*.h")

find_package(Torch REQUIRED PATHS ${CMAKE_SOURCE_DIR}/packages/libtorch/share/cmake/Torch)
message(STATUS "TORCH_CXX_FLAGS: ${TORCH_CXX_FLAGS}")
message(STATUS "TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")
message(STATUS "TORCH_LIBRARIES: ${TORCH_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
add_executable (train ${TRAIN_SOURCES} ${BACKEND_SOURCES})
target_include_directories(train PUBLIC "${TORCH_INCLUDE_DIRS}")
target_link_libraries(train "${TORCH_LIBRARIES}" stdc++fs)
set_property(TARGET train PROPERTY CXX_STANDARD 17)
set_property(TARGET train PROPERTY CXX_STANDARD_REQUIRED ON)
