#ifdef __linux

#ifndef LFWATCH_LINUX
#define LFWATCH_LINUX

#include <string>
#include <map>
#include <sys/inotify.h>

namespace lfw {
//Possible events we can notify about
enum Notify {
	//Should we listen for this event type? there's some differences
	//in windows/linux rename event handling need to test
	CHANGE_FILE_NAME = IN_MOVED_TO,
	CHANGE_DIR_NAME = IN_MOVE_SELF,
	CHANGE_ATTRIBUTES = IN_ATTRIB,
	//Do we really care about size and last write change?
	//one kind of implies the other if the file changed at all
	CHANGE_SIZE = IN_MODIFY,
	CHANGE_LAST_WRITE = IN_CLOSE_WRITE,
	CHANGE_LAST_ACCESS = IN_ACCESS,
	CHANGE_CREATION = IN_ATTRIB,
	CHANGE_SECURITY = IN_ATTRIB
};

class WatchLinux {
	//Active watches { watch descriptor, dir name }
	//Assuming that people will get more updates than they'll
	//call remove, using this map layout will give fast dir name
	//lookups by id but slow name lookups to remove watches
	std::map<int, std::string> watchers;
	int notify_fd;

public:
	WatchLinux();
	~WatchLinux();
	/**
	 * Start watching some directory for file changes
	 * watch_subtree should be true if we want to watch
	 * all subdirectories as well. Filters is a set of
	 * the notify flags or'd together to watch for
	 * TODO: watch_subtree ignored on linux, should remove
	 */
	void watch(const std::string &dir, bool watch_subtree, unsigned filters);
	void remove(const std::string &dir);
	//Update watchers. I'd really like to put this on some background thread though
	void update();

private:
	WatchLinux(const WatchLinux &w){
		//TODO Rule of 3
	}
	WatchLinux& operator=(const WatchLinux &w){
		//TODO Rule of 3
		return *this;
	}
};
}

#endif
#endif

