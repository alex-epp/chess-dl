file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

add_executable(backend_demo ${SOURCE_LIST})
target_link_libraries(backend_demo PRIVATE backend)
