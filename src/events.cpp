#include <string>
#include "events.h"

namespace lfw {
EventData::EventData(const std::string &dir, const std::string &fname,
	uint32_t filter, uint32_t event)
	: dir(dir), fname(fname), filter(filter), event(event)
{}
}

