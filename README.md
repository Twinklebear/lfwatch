lfwatch - A LightFileWatcher
=
A very lightweight file watcher for Windows, Linux (coming soon) and OS X (coming soon). Monitors a
directory for file changes and pushes an SDL user event containing information about the change or if
compiled without SDL2 support can call a custom user function for each directory being watched.

Dependencies
-
- [SDL2](http://libsdl.org/) if using the SDL2 event pushing version.

Building
-
Uses CMake to build, to disable the SDL2 event path run CMake with `-DNO_SDL`. This version will
take a `std::function` to be called on an event. This isn't currently implemented so I haven't decided
on what the function signature should be. A different callback can be registered for each directory being
watched.

