add_executable(demo main.cpp)
target_link_libraries(demo lfwatch)
install(TARGETS demo DESTINATION "${LFWATCH_SOURCE_DIR}/bin")

