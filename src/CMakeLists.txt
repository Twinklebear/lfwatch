add_library(lfwatch lfwatch_win32.cpp lfwatch_linux.cpp)
install(TARGETS lfwatch DESTINATION "${LFWATCH_SOURCE_DIR}/bin")

