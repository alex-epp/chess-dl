file(GLOB_RECURSE DEMO_SOURCES
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

file(GLOB BACKEND_SOURCES
        CONFIGURE_DEPENDS
        "../*.cpp"
        "../*.hpp"
        "../*.h")

add_executable(demo ${DEMO_SOURCES} ${BACKEND_SOURCES})
set_property(TARGET demo PROPERTY CXX_STANDARD 17)
set_property(TARGET demo PROPERTY CXX_STANDARD_REQUIRED ON)

set(CMAKE_CXX_FLAGS_RELEASE "-O3")
