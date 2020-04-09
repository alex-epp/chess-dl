file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

add_executable(backend_tests ${SOURCE_LIST})
target_link_libraries(backend_tests PRIVATE backend Catch2)
