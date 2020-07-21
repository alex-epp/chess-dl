cmake_minimum_required (VERSION 3.15)


project(chess_dl VERSION 0.1
        DESCRIPTION "Machine learning with chess"
        LANGUAGES CXX)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
endif()


find_package(Torch REQUIRED)
include_directories("${TORCH_INCLUDE_DIRS}")  # Including globally like this is bad practice afaik, but required in this
                                              # case so that Clion's autocomplete works correctly in remote mode.
                                              # Alternatively, I could download libtorch using FetchContent.
add_library(Torch INTERFACE IMPORTED)
target_include_directories(Torch INTERFACE "${TORCH_INCLUDE_DIRS}")
target_link_libraries(Torch INTERFACE "${TORCH_LIBRARIES}")

include(FetchContent)
set(FETCHCONTENT_QUIET off)

FetchContent_Declare(
        chess
        GIT_REPOSITORY https://github.com/alex-epp/chess
)
if(NOT chess_POPULATED)
    FetchContent_MakeAvailable(chess)
endif()

add_subdirectory("src")


