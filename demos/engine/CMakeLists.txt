file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

add_executable(engine_demo ${SOURCE_LIST})
target_link_libraries(engine_demo PRIVATE engine)
