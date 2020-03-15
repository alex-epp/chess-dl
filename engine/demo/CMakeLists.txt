file(GLOB DEMO_SOURCES
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp")

file(GLOB ENGINE_SOURCES
        CONFIGURE_DEPENDS
        "../*.cpp"
        "../*.hpp")

file(GLOB BACKEND_SOURCES
        CONFIGURE_DEPENDS
        "../../backend/*.cpp")

add_executable(engine_demo ${DEMO_SOURCES} ${ENGINE_SOURCES} ${BACKEND_SOURCES})
target_link_libraries(engine_demo stdc++fs)
set_property(TARGET engine_demo PROPERTY CXX_STANDARD 17)
set_property(TARGET engine_demo PROPERTY CXX_STANDARD_REQUIRED ON)
