file(GLOB SOURCE_LIST
        CONFIGURE_DEPENDS
        "*.cpp"
        "*.hpp"
        "./datasets/*.cpp"
        "./datasets/*.hpp")

add_executable(train ${SOURCE_LIST})
target_link_libraries(train PRIVATE chess Torch)
target_include_directories(train PRIVATE "${chess_dl_SOURCE_DIR}/include")

if (MSVC)
    # This is annoying to have to do. Maybe I could make a custom patch to libtorch
    # that uses install() properly
    file(GLOB TORCH_DLLS "${TORCH_INSTALL_PREFIX}/lib/*.dll")
    add_custom_command(TARGET train
            POST_BUILD
            COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${TORCH_DLLS}
            $<TARGET_FILE_DIR:train>)
endif (MSVC)
