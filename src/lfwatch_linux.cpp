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
#include "lfwatch_linux.h"

namespace lfw {
WatchData::WatchData(const Callback &callback, int wd, const std::string dir)
	: callback(callback), watch_descriptor(wd), dir_name(dir)
{}

WatchLinux::WatchLinux() : notify_fd(inotify_init1(IN_NONBLOCK)){
	if (notify_fd == -1){
		perror("Failed to initialize inotify");
		assert(false);
	}
}
WatchLinux::~WatchLinux(){
	close(notify_fd);
}
void WatchLinux::watch(const std::string &dir, unsigned filters,
	const Callback &callback)
{
	int wd = inotify_add_watch(notify_fd, dir.c_str(), filters);
	if (wd == -1){
		std::string msg = "Failed to watch " + dir;
		perror(msg.c_str());
		return;
	}
	watchers.emplace(std::make_pair(wd, WatchData{callback, wd, dir}));
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
	char buf[4 * 1024];
	//Should we use a bigger buffer like the example?
	int len = read(notify_fd, buf, sizeof(buf));
	if (len == -1 && errno != EAGAIN){
		perror("Error reading");
	}
	else if (len > 0){
		struct inotify_event *event;
		for (char *evt = buf; evt < buf + len;
				evt += sizeof(struct inotify_event) + event->len)
		{
			event = reinterpret_cast<struct inotify_event*>(evt);
			//Check that the listener hasn't been removed between when
			//we got this event and now
			auto it = watchers.find(event->wd);
			if (it != watchers.end()){
				it->second.callback(it->second.dir_name, event->name, event->mask);
			}
		}
	}
}
}

#endif

