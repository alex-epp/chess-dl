# LIST(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/Modules)

# LIST(APPEND CMAKE_PREFIX_PATH ${CMAKE_SOURCE_DIR}/lib/libtorch)

# list(APPEND CMAKE_FIND_ROOT_PATH ${CMAKE_SOURCE_DIR}/lib/libtorch/share/cmake/Torch)
find_package(Torch REQUIRED PATHS ${CMAKE_SOURCE_DIR}/packages/libtorch/share/cmake/Torch)
message(STATUS "TORCH_CXX_FLAGS: ${TORCH_CXX_FLAGS}")
message(STATUS "TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")
message(STATUS "TORCH_LIBRARIES: ${TORCH_LIBRARIES}")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")

include_directories("${TORCH_INCLUDE_DIRS}")

add_executable (train "train.cpp")
target_include_directories(train PUBLIC "${TORCH_INCLUDE_DIRS}")
target_link_libraries(train "${TORCH_LIBRARIES}")
set_property(TARGET train PROPERTY CXX_STANDARD 17)
set_property(TARGET train PROPERTY CXX_STANDARD_REQUIRED ON)

#target_include_directories(train PUBLIC
#        ${CMAKE_SOURCE_DIR}/lib/libtorch/include
#        ${CMAKE_SOURCE_DIR}/lib/libtorch/include/torch/csrc/api/include)

#message(STATUS ${CMAKE_SOURCE_DIR}/lib/libtorch/include)
#message(STATUS ${CMAKE_SOURCE_DIR}/lib/libtorch/include/torch/csrc/api/include)

#target_link_libraries(train "${TORCH_LIBRARIES}")
