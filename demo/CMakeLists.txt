add_executable(demo main.cpp)
target_link_libraries(demo lfwatch)
install(TARGETS demo DESTINATION "${LFWATCH_SOURCE_DIR}/bin")

add_executable(sdl_demo sdl_demo.cpp)
target_link_libraries(sdl_demo lfwatch ${SDL2_LIBRARY})
install(TARGETS sdl_demo DESTINATION "${LFWATCH_SOURCE_DIR}/bin")

