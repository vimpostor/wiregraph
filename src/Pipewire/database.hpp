#pragma once

#include <vector>

#include "device.hpp"
#include "Models/device_model.hpp"

namespace Pipewire {
class Database {
public:
	SINGLETON(Database);

	void add_item(Item&& n);
	void remove_item(uint32_t id);
private:
	std::vector<Item> items;
	std::vector<Device> devices;
};
}
