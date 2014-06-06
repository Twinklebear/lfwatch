#ifdef __APPLE__

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <CoreServices/CoreServices.h>
#include "lfwatch_osx.h"

namespace lfw {
//Remap our notify enums to match OS X event flags, we really just need to
//convert our old/new name values but to avoid any future conflicts we remap all of it
uint32_t remap_file_notify(uint32_t mask){
	uint32_t remap = 0;
	if (mask & Notify::FILE_MODIFIED){
		remap |= Notify::FILE_MODIFIED;
	}
	if (mask & Notify::FILE_CREATED){
		remap |= Notify::FILE_CREATED;
	}
	if (mask & Notify::FILE_REMOVED){
		remap |= Notify::FILE_REMOVED;
	}
	if (mask & FILE_RENAMED_OLD_NAME || mask & FILE_RENAMED_NEW_NAME){
		remap |= kFSEventStreamEventFlagItemRenamed;
	}
	return remap;
}
void watch_callback(ConstFSEventStreamRef stream, void *data, size_t n_events,
	void *event_paths, const FSEventStreamEventFlags flags[],
	const FSEventStreamEventId ids[])
{
	//Tracks if we're looking for a new name event or not
	bool renaming = false;
	WatchData *watch = static_cast<WatchData*>(data);
	char **paths = static_cast<char**>(event_paths);
	for (size_t i = 0; i < n_events; ++i){
		//OS X just sends all events so we filter here
		if (flags[i] & remap_file_notify(watch->filter)){
			//OS X sends full path so get the filename out
			std::string fname{paths[i]};
			fname = fname.substr(fname.find_last_of('/') + 1);
			uint32_t action = flags[i];
			//Check if it's a rename event and what type of rename we're expecting, ie. old/new name
			if (flags[i] & kFSEventStreamEventFlagItemRenamed){
				if (!renaming && watch->filter & Notify::FILE_RENAMED_OLD_NAME){
					renaming = true;
					action = Notify::FILE_RENAMED_OLD_NAME;
				}
				else if (renaming && watch->filter & Notify::FILE_RENAMED_NEW_NAME){
					renaming = false;
					action = Notify::FILE_RENAMED_NEW_NAME;

				}
			}
			watch->callback(EventData{watch->dir_name, fname, watch->filter, action});
		}
	}
}
//Make and schedule a stream, the stream will use the watch as its context info
void make_stream(WatchData &watch){
	CFStringRef cfdir = CFStringCreateWithCString(nullptr,
		watch.dir_name.c_str(), kCFStringEncodingUTF8);
	CFArrayRef cfdirs = CFArrayCreate(nullptr, (const void**)&cfdir, 1, nullptr);

	FSEventStreamContext ctx = { 0, &watch, nullptr, nullptr, nullptr };
	watch.stream = FSEventStreamCreate(NULL, &watch_callback, &ctx, cfdirs,
		kFSEventStreamEventIdSinceNow, 1.0,
		kFSEventStreamCreateFlagFileEvents | kFSEventStreamCreateFlagNoDefer);

	FSEventStreamScheduleWithRunLoop(watch.stream, CFRunLoopGetCurrent(),
		kCFRunLoopDefaultMode);
	FSEventStreamStart(watch.stream);

	CFRelease(cfdir);
	CFRelease(cfdirs);
}
void cancel(WatchData &watch){
	//Probably should just do this in destructor
	FSEventStreamStop(watch.stream);
	FSEventStreamInvalidate(watch.stream);
	FSEventStreamRelease(watch.stream);
}

WatchData::WatchData(const std::string &dir, uint32_t filter, const Callback &cb)
	: dir_name(dir), filter(filter), callback(cb)
{}

WatchOSX::WatchOSX(){}
WatchOSX::~WatchOSX(){
	for (auto &pair : watchers){
		cancel(pair.second);
	}
}
void WatchOSX::watch(const std::string &dir, uint32_t filters, const Callback &callback){
	auto fnd = watchers.find(dir);
	if (fnd != watchers.end()){
		fnd->second.filter = filters;
		fnd->second.callback = callback;
		return;
	}
	auto it = watchers.emplace(std::make_pair(dir, WatchData{dir, filters, callback}));
	make_stream(it.first->second);
}
void WatchOSX::remove(const std::string &dir){
	auto fnd = watchers.find(dir);
	if (fnd != watchers.end()){
		cancel(fnd->second);
		watchers.erase(fnd);
	}
}
void WatchOSX::update(){
	CFRunLoopRunInMode(kCFRunLoopDefaultMode, 0, true);
}
}

#endif

