#include <ostream>
#include "events.h"
#include "lfwatch_linux.h"
#include "lfwatch_win32.h"

namespace lfw {
#ifdef __linux
typedef WatchLinux Watcher;
#elif defined(__WIN32)
typedef WatchWin32 Watcher;
#elif defined(__APPLE__)
typedef WatchOSX Watcher;
#endif
}

