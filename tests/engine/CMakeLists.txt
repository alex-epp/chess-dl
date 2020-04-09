file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

add_executable(engine_tests ${SOURCE_LIST})
target_link_libraries(engine_tests PRIVATE engine Catch2)
