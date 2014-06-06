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
WatchData::WatchData(int wd, const std::string &dir, uint32_t filter,
	const Callback &cb)
	: watch_descriptor(wd), dir_name(dir), filter(filter), callback(cb)
{}

WatchLinux::WatchLinux() : notify_fd(inotify_init1(IN_NONBLOCK)){
	if (notify_fd == -1){
		perror("lfw Error: Failed to initialize inotify");
		assert(false);
	}
}
WatchLinux::~WatchLinux(){
	close(notify_fd);
}
void WatchLinux::watch(const std::string &dir, uint32_t filters, const Callback &callback){
	auto fnd = find_dir(dir);
	if (fnd != watchers.end() && fnd->second.filter != filters){
		fnd->second.filter = filters;
		fnd->second.callback = callback;
	}
	//When modifying an existing watch we get back the existing descriptor, so no
	//need to update it
	int wd = inotify_add_watch(notify_fd, dir.c_str(), filters);
	if (wd == -1){
		std::string msg = "lfw Error: Failed to watch " + dir;
		perror(msg.c_str());
		return;
	}
	if (fnd == watchers.end()){
		watchers.emplace(std::make_pair(wd, WatchData{wd, dir, filters, callback}));
	}
}
void WatchLinux::remove(const std::string &dir){
	auto it = find_dir(dir);
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
			perror("lfw Error: error reading event information");
		}
		else if (len > 0){
			emit_events(buf, len);
		}
	}
	while (len > 0);
}
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
			it->second.callback(EventData{it->second.dir_name, event->name,
				it->second.filter, event->mask});
		}
	}
}
std::map<int, WatchData>::iterator WatchLinux::find_dir(const std::string &dir){
	return std::find_if(watchers.begin(), watchers.end(),
		[&dir](const std::pair<const int, WatchData> &pair){
			return dir == pair.second.dir_name;
		});
}
}

#endif

