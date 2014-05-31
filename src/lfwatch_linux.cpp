#ifdef __linux

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include <cassert>
#include <sys/inotify.h>
#include <unistd.h>
#include <stdio.h>
#include <errno.h>
#include "lfwatch_linux.h"

namespace lfw {
WatchLinux::WatchLinux() : notify_fd(inotify_init1(IN_NONBLOCK)){
	if (notify_fd == -1){
		perror("Failed to initialize inotify");
		assert(false);
	}
}
WatchLinux::~WatchLinux(){
	close(notify_fd);
}
void WatchLinux::watch(const std::string &dir, bool watch_subtree, unsigned filters){
	int wd = inotify_add_watch(notify_fd, dir.c_str(), filters);
	if (wd == -1){
		std::string msg = "Failed to watch " + dir;
		perror(msg.c_str());
		return;
	}
	watchers.emplace(std::make_pair(wd, dir));
}
void WatchLinux::remove(const std::string &dir){
	auto it = std::find_if(watchers.begin(), watchers.end(),
		[&dir](const std::pair<const int, std::string> &pair){
			return dir == pair.second;
		});
	if (it != watchers.end()){
		inotify_rm_watch(notify_fd, it->first);
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
			std::cout << "Got event in dir "
				<< watchers[event->wd]
				<< " on file " << event->name << "\n";
		}
	}
	else {
		std::cout << "No event\n";
	}
}
}

#endif

