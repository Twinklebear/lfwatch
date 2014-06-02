add_library(lfwatch lfwatch_win32.cpp lfwatch_linux.cpp events.cpp)

if (NOT DEFINED NO_SDL)
	target_link_libraries(lfwatch ${SDL2_LIBRARY})
endif()

install(FILES "${LFWATCH_SOURCE_DIR}/include/lfwatch.h"
	"${LFWATCH_SOURCE_DIR}/include/lfwatch_win32.h" "${LFWATCH_SOURCE_DIR}/include/lfwatch_linux.h"
	"${LFWATCH_SOURCE_DIR}/include/events.h"
	DESTINATION include)
install(TARGETS lfwatch
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib)

