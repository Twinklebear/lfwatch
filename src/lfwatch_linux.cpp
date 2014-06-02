#ifdef __linux

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cassert>
#include <functional>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>

#ifndef NO_SDL
#include <SDL.h>
#endif

#include "lfwatch_linux.h"

namespace lfw {

#ifdef NO_SDL
WatchData::WatchData(int wd, const std::string &dir, uint32_t filter,
	const Callback &cb)
	: watch_descriptor(wd), dir_name(dir), filter(filter), callback(cb)
{}
#else
WatchData::WatchData(int wd, const std::string &dir, uint32_t filter)
	: watch_descriptor(wd), dir_name(dir), filter(filter)
{}
#endif

#ifndef NO_SDL
//SDL uses (Uint32)-1 for invalid event code
uint32_t WatchLinux::event_code = -1;
#endif

WatchLinux::WatchLinux() : notify_fd(inotify_init1(IN_NONBLOCK)){
	if (notify_fd == -1){
		perror("Failed to initialize inotify");
		assert(false);
	}
#ifndef NO_SDL
	//Check if we've initialized the SDL event code, invalid ids are
	//(Uint32)-1
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
WatchLinux::~WatchLinux(){
	close(notify_fd);
}
#ifdef NO_SDL
void WatchLinux::watch(const std::string &dir, uint32_t filters,
	const Callback &callback)
#else
void WatchLinux::watch(const std::string &dir, uint32_t filters)
#endif
{
	int wd = inotify_add_watch(notify_fd, dir.c_str(), filters);
	if (wd == -1){
		std::string msg = "Failed to watch " + dir;
		perror(msg.c_str());
		return;
	}
#ifdef NO_SDL
	watchers.emplace(std::make_pair(wd, WatchData{wd, dir, filters, callback}));
#else
	watchers.emplace(std::make_pair(wd, WatchData{wd, dir, filters}));
#endif
}
void WatchLinux::remove(const std::string &dir){
	auto it = std::find_if(watchers.begin(), watchers.end(),
		[&dir](const std::pair<const int, WatchData> &pair){
			return dir == pair.second.dir_name;
		});
	if (it != watchers.end()){
		inotify_rm_watch(notify_fd, it->first);
		watchers.erase(it);
	}
}
void WatchLinux::update(){
	int len;
	do {
		char buf[4 * 1024] = { 0 };
		len = read(notify_fd, buf, sizeof(buf));
		if (len == -1 && errno != EAGAIN){
			perror("Error reading");
		}
		else if (len > 0){
			emit_events(buf, len);
		}
	}
	while (len > 0);
}
#ifndef NO_SDL
uint32_t WatchLinux::event(){
	return event_code;
}
#endif
void WatchLinux::emit_events(const char *buf, int len){
	const struct inotify_event *event;
	for (const char *evt = buf; evt < buf + len;
		evt += sizeof(struct inotify_event) + event->len)
	{
		event = reinterpret_cast<const struct inotify_event*>(evt);
		//Check that the listener hasn't been removed between when
		//we got this event and now
		auto it = watchers.find(event->wd);
		if (it != watchers.end()){
#ifdef NO_SDL
			it->second.callback(EventData{it->second.dir_name, event->name,
					it->second.filter, event->mask});
#else
			SDL_Event sdl_evt;
			SDL_zero(sdl_evt);
			sdl_evt.type = event_code;
			sdl_evt.user.code = event->mask;
			sdl_evt.user.data1 = new EventData{it->second.dir_name,
					event->name, it->second.filter, event->mask};
			sdl_evt.user.data2 = nullptr;
			SDL_PushEvent(&sdl_evt);
#endif
		}
	}
}
}

#endif

