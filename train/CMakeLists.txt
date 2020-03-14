file(GLOB TRAIN_SOURCES
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp"
        "./datasets/*.cpp"
        "./datasets/*.hpp")

file(GLOB BACKEND_SOURCES
        CONFIGURE_DEPENDS
        "../backend/*.cpp"
        "../backend/*.hpp")

add_executable (train ${TRAIN_SOURCES} ${BACKEND_SOURCES})
target_link_libraries(train "${TORCH_LIBRARIES}" stdc++fs)
set_property(TARGET train PROPERTY CXX_STANDARD 17)
set_property(TARGET train PROPERTY CXX_STANDARD_REQUIRED ON)
