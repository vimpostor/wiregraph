#pragma once

#include <map>
#include <pipewire/pipewire.h>
#include <spa/utils/dict.h>
#include <string>

#include "Util/util.hpp"

namespace Pipewire {
class Item {
public:
	Item(uint32_t id, uint32_t permissions, const char* type, uint32_t version, const struct spa_dict* props);

	uint32_t id = 0;
	uint32_t permissions = 0;
	std::string type;
	std::map<std::string, std::string> props;
};
}
