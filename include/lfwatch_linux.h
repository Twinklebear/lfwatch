#ifdef __linux

#ifndef LFWATCH_LINUX
#define LFWATCH_LINUX

#include <string>
#include <map>
#include <functional>
#include <sys/inotify.h>

namespace lfw {
//Possible events we can notify about
enum Notify {
	//Should we listen for this event type? there's some differences
	//in windows/linux rename event handling need to test
	CHANGE_FILE_NAME = IN_MOVE,
	CHANGE_DIR_NAME = IN_MOVE_SELF,
	CHANGE_ATTRIBUTES = IN_ATTRIB,
	CHANGE_LAST_WRITE = IN_CLOSE_WRITE,
	CHANGE_LAST_ACCESS = IN_ACCESS,
};

typedef std::function<void(const std::string&, const std::string&, unsigned)>
	Callback;

struct WatchData {
	Callback callback;
	int watch_descriptor;
	std::string dir_name;

	WatchData(const Callback &callback, int wd, const std::string dir);
};

class WatchLinux {
	//Active watches { watch descriptor, dir name }
	//Assuming that people will get more updates than they'll
	//call remove, using this map layout will give fast dir name
	//lookups by id but slow name lookups to remove watches
	std::map<int, WatchData> watchers;
	int notify_fd;

public:
	WatchLinux();
	~WatchLinux();
	/**
	 * Start watching some directory for file changes
	 * Filters is a set of the notify flags or'd
	 * together to watch for
	 */
	void watch(const std::string &dir, unsigned filters, const Callback &callback);
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

