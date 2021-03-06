#ifndef EVENTS_H
#define EVENTS_H

#include <string>
#include <functional>

namespace lfw {
//Struct returned to callbacks or in the SDL event containg information
//about the event
struct EventData {
	std::string dir, fname;
	//Filter are the active filters that caught the event
	//event is the event code that was caught
	uint32_t filter, event;

	EventData(const std::string &dir, const std::string &fname, uint32_t filter,
		uint32_t event);
};

typedef std::function<void(const EventData &data)> Callback;

}

#endif

