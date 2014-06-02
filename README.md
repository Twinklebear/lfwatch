lfwatch - A LightFileWatcher
=
A very lightweight file watcher for Windows (in progress), Linux (in progress) and OS X (coming soon). Monitors a
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

Examples
-
The library is very simple, create an `lfw::Watcher` and then select the directories and changes you
want to watch for with the `watch` method. The SDL User event or registered callbacks are passed an
`lfw::EventData` struct containing information about the event.

```c++
struct EventData {
	//dir: the directory being watched
	//fname: the file the event occured on
	std::string dir, fname;
	//filter: the filters you set that matched the event
	//event: the specific event that occured to match your filters
	uint32_t filter, event;
};
```

If we preferred to use SDL events to feed us back information about what files changed so that we
could handle it normally in our event loop it would look like this.

```c++
//Initialize SDL before creating the watcher
lfw::Watcher watcher;
//Watch for file writes in 'some_directory'
watcher.watch("some_directory", lfw::Notify::CHANGE_LAST_WRITE);

//In your update loop notify the watcher to update itself
watcher.update();

//Now file events will show up as SDL user events in our event loop
SDL_Event e;
while (SDL_PollEvent(&e)){
	if (e.type == lfw::Watcher::event()){
		//Retrieve the information about the event from the SDL user event
		lfw::EventData *data = static_cast<lfw::EventData*>(e.user.data1);
		std::cout << "File " << data->fname << " was written to\n";
		//Must delete it since it's passed on the heap
		delete data;
	}
}
```

If instead we wanted to use our own callbacks to get the event information it'd look more like this.

```c++
lfw::Watcher watcher;
//Watch for file writes in 'some_directory' and call our lambda when one happens
watcher.watch("some_directory", lfw::Notify::CHANGE_LAST_WRITE
	[](lfw::EventData e){
		std::cout << "File " << e.fname << " was written to\n";
	});

//In your update loop notify the watcher to update itself
watcher.update();
```

