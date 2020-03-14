file(GLOB TESTS_SOURCES
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

file(GLOB BACKEND_SOURCES
        CONFIGURE_DEPENDS
        "../../backend/*.cpp"
        "../../backend/*.hpp")

file(GLOB ENGINE_SOURCES
        CONFIGURE_DEPENDS
        "../*.cpp"
        "../*.hpp")

add_executable(engine_tests ${TESTS_SOURCES} ${BACKEND_SOURCES} ${ENGINE_SOURCES})
target_link_libraries(engine_tests stdc++fs)
set_property(TARGET engine_tests PROPERTY CXX_STANDARD 17)
set_property(TARGET engine_tests PROPERTY CXX_STANDARD_REQUIRED ON)
