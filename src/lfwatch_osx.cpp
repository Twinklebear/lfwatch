#ifdef __APPLE__

#include <iostream>
#include <string>
#include <map>
#include <functional>
#include <CoreServices/CoreServices.h>

#ifndef NO_SDL
#include <SDL.h>
#endif

#include "lfwatch_osx.h"

namespace lfw {
void watch_callback(ConstFSEventStreamRef stream, void *data, size_t n_events,
	void *event_paths, const FSEventStreamEventFlags flags[], const FSEventStreamEventId ids[])
{
	WatchData *watch = static_cast<WatchData*>(data);
	char **paths = static_cast<char**>(event_paths);
	for (size_t i = 0; i < n_events; ++i){
		std::cout << "Event in " << watch->dir_name
			<< " on path " << paths[i] << "\n";
	}
}
void cancel(WatchData &watch){
	//Probably should just do this in destructor
	FSEventStreamStop(watch.stream);
	FSEventStreamInvalidate(watch.stream);
	FSEventStreamRelease(watch.stream);
}

#ifdef NO_SDL
WatchData::WatchData(const std::string &dir, uint32_t filter, const Callback &cb)
	: dir_name(dir), filter(filter), callback(cb)
{
	CFStringRef cfdir = CFStringCreateWithCString(NULL, dir_name.c_str(), kCFStringEncodingUTF8);
	CFArrayRef cfdirs = CFArrayCreate(NULL, (const void**)&cfdir, 1, NULL);
	stream = FSEventStreamCreate(NULL, &watch_callback, NULL, cfdirs,
		kFSEventStreamEventIdSinceNow, 3.0, kFSEventStreamCreateFlagFileEvents);
	FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	FSEventStreamStart(stream);
	CFRelease(cfdir);
	CFRelease(cfdirs);
}
#else
WatchData::WatchData(const std::string &dir, uint32_t filter) : dir_name(dir), filter(filter){
	CFStringRef cfdir = CFStringCreateWithCString(NULL, dir_name.c_str(), kCFStringEncodingUTF8);
	CFArrayRef cfdirs = CFArrayCreate(NULL, (const void**)&cfdir, 1, NULL);
	stream = FSEventStreamCreate(NULL, &watch_callback, NULL, cfdirs,
		kFSEventStreamEventIdSinceNow, 3.0, kFSEventStreamCreateFlagFileEvents);
	FSEventStreamScheduleWithRunLoop(stream, CFRunLoopGetCurrent(), kCFRunLoopDefaultMode);
	FSEventStreamStart(stream);
	CFRelease(cfdir);
	CFRelease(cfdirs);
}
#endif

#ifndef NO_SDL
//SDL uses (Uint32)-1 for invalid event code
uint32_t WatchOSX::event_code = -1;
#endif

WatchOSX::WatchOSX(){
#ifndef NO_SDL
	//Check if we've intialized the SDL event code, invalid ids are (Uint32)-1
	if (event_code == static_cast<uint32_t>(-1)){
		event_code = SDL_RegisterEvents(1);
		if (event_code == static_cast<uint32_t>(-1)){
			std::cerr << "Failed to get event code from SDL: "
				<< SDL_GetError() << std::endl;
			assert(false);
		}
	}
#endif
}
WatchOSX::~WatchOSX(){
	for (auto &pair : watchers){
		cancel(pair.second);
	}
}
#ifdef NO_SDL
void WatchOSX::watch(const std::string &dir, uint32_t filters, const Callback &callback)
#else
void WatchOSX::watch(const std::string &dir, uint32_t filters)
#endif
{
	auto fnd = watchers.find(dir);
	if (fnd != watchers.end()){
		fnd->second.filter = filters;
#ifdef NO_SDL
		fnd->second.callback = callback;
#endif
		return;
	}
#ifdef NO_SDL
	watchers.emplace(std::make_pair(dir, WatchData{dir, filters, callback}));
#else
	watchers.emplace(std::make_pair(dir, WatchData{dir, filters}));
#endif
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
#ifndef NO_SDL
uint32_t WatchOSX::event(){
	return event_code;
}
#endif

}

#endif

