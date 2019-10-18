#include "events.h"
#include "lfwatch_linux.h"
#include "lfwatch_win32.h"
#include "lfwatch_osx.h"

namespace lfw {
#ifdef __linux
typedef WatchLinux Watcher;
#elif defined(_WIN32)
typedef WatchWin32 Watcher;
#elif defined(__APPLE__)
typedef WatchOSX Watcher;
#endif
}

