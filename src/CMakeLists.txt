add_library(lfwatch lfwatch_win32.cpp lfwatch_linux.cpp lfwatch_osx.cpp events.cpp)

install(FILES "${LFWATCH_SOURCE_DIR}/include/lfwatch.h"
	"${LFWATCH_SOURCE_DIR}/include/lfwatch_win32.h" "${LFWATCH_SOURCE_DIR}/include/lfwatch_linux.h"
	"${LFWATCH_SOURCE_DIR}/include/lfwatch_osx.h" "${LFWATCH_SOURCE_DIR}/include/events.h"
	DESTINATION include)
install(TARGETS lfwatch
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

