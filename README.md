lfwatch - A LightFileWatcher
=
A lightweight file watcher for Windows, Linux and OS X. Monitors a directory for file changes and calls
the callback set for the directory with information about the event.

Building
-
The library uses CMake to build and compiles to a static linked library. If you want to build the demo
pass `-DBUILD_DEMO=1` when you run CMake.

Example
-
To watch for file modification changes in "some_directory" and print out the name of the modified file we
will create a watcher, set a watch on the directory with the filters and callback we want and then call update
in our update loop.

```c++
lfw::Watcher watcher;
//Watch for file writes in 'some_directory' and call our lambda when one happens
watcher.watch("some_directory", lfw::Notify::FILE_MODIFIED
	[](const lfw::EventData &e){
		std::cout << "File " << e.fname << " was written to\n";
	});

//In your update loop notify the watcher to update itself
watcher.update();
```

There's a runnable example in demo which shows usage of the various filters available and
demonstrates adding/removing/updating watches.

Documentation
-
### Callback
The callback set for each watched directory should be a `std::function<void(const lfw::EventData&)>`. This is
also typedef'd as `lfw::Callback`.

### EventData
The `lfw::EventData` struct passed to your callback contains information about the watched directory and
event filters along with the file name and event that was caught.
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

### Notify Filters and Events
There are five types of events you can watch a directory for and receive events about. To watch for multiple event types
you can or the desired flags together.
- `lfw::Notify::FILE_MODIFIED`: A file was modified. This could be an attribute change or file write.
- `lfw::Notify::FILE_CREATED`: A file was created.
- `lfw::Notify::FILE_REMOVED`: A file was removed.
- `lfw::Notify::FILE_RENAMED_OLD_NAME`: A file was renamed and this is its old name
- `lfw::Notify::FILE_RENAMED_NEW_NAME`: A file was renamed and this is its new name

**Note**: For rename events there isn't information to absolutely associate the old name with the new name event. Although
on Linux inotify gives you an associated number with the rename event it doesn't seem like Windows or OS X do that.
Keep this in mind if you might be watching a lot of rename events.

### Watcher
The `lfw::Watcher` is what manages the watched directories and is responsible for adding, removing and updating existing
watches along with cleaning them up when it's destroyed.

To add a new directory to be watched or to update an existing watch with new filters and/or callback call `watch`.
The function takes the directory name, your `lfw::Notify` filters or'd together and the callback to call.
```c++
void watch(const std::string &dir, uint32_t filters, const Callback &callback);
```

To remove a directory from the watch list call `remove` and pass the directory name to be removed.
```c++
void remove(const std::string &dir);
```

To update the watched directories and receive new events that have occured you must call `update`.
```c++
void update();
```

