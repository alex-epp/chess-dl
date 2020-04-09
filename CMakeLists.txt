cmake_minimum_required (VERSION 3.15)


project(chess VERSION 0.1
        DESCRIPTION "C++ chess engine"
        LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()


find_package(Torch REQUIRED)
message(STATUS "TORCH_CXX_FLAGS: ${TORCH_CXX_FLAGS}")
message(STATUS "TORCH_INCLUDE_DIRS: ${TORCH_INCLUDE_DIRS}")
message(STATUS "TORCH_LIBRARIES: ${TORCH_LIBRARIES}")
add_library(Torch INTERFACE IMPORTED)
target_include_directories(Torch INTERFACE "${TORCH_INCLUDE_DIRS}")
target_link_libraries(Torch INTERFACE "${TORCH_LIBRARIES}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${TORCH_CXX_FLAGS}")
include_directories("${TORCH_INCLUDE_DIRS}")  # Including globally like this is bad practice afaik, but required in this
                                              # case so that Clion's autocomplete works correctly in remote mode.
                                              # Alternatively, I could download libtorch using FetchContent.

include(FetchContent)
set(FETCHCONTENT_QUIET off)

FetchContent_Declare(
        mio
        GIT_REPOSITORY https://github.com/mandreyel/mio
)
if(NOT mio_POPULATED)
    FetchContent_MakeAvailable(mio)
endif()

FetchContent_Declare(
        Catch2
        GIT_REPOSITORY https://github.com/catchorg/Catch2.git
        GIT_TAG        v2.11.3
)
if(NOT Catch2_POPULATED)
    FetchContent_Populate(Catch2)
    add_subdirectory("${catch2_SOURCE_DIR}" "${catch2_BINARY_DIR}")
endif()

add_subdirectory("src")
add_subdirectory("demos")
add_subdirectory("tests")


