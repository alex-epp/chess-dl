file(GLOB_RECURSE TESTS_SOURCES
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

file(GLOB BACKEND_SOURCES
        CONFIGURE_DEPENDS
        "../*.cpp"
        "../*.hpp"
        "../*.h")

add_executable(tests ${TESTS_SOURCES} ${BACKEND_SOURCES})
set_property(TARGET tests PROPERTY CXX_STANDARD 17)
set_property(TARGET tests PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
