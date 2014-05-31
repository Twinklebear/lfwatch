if (NOT DEFINED NO_SDL)
	add_executable(demo sdl_demo.cpp)
	target_link_libraries(demo lfwatch ${SDL2_LIBRARY})
else ()
	add_executable(demo main.cpp)
	target_link_libraries(demo lfwatch)
endif()

install(TARGETS demo DESTINATION "${LFWATCH_SOURCE_DIR}/bin")

