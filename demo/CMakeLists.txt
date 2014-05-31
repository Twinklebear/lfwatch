add_executable(demo main.cpp)
if (NOT DEFINED NO_SDL)
	target_link_libraries(demo lfwatch ${SDL2_LIBRARY})
else ()
	target_link_libraries(demo lfwatch)
endif()

install(TARGETS demo DESTINATION "${LFWATCH_SOURCE_DIR}/bin")

