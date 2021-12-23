#include "database.hpp"

namespace Pipewire {

void Database::add_item(Item &&n) {
	items.emplace_back(n);
	if (Device::is_audio_device(n)) {
		devices.emplace_back(n);
		DeviceModel::get()->add_device(n);
	}
}

void Database::remove_item(uint32_t id) {
	for (auto it = items.begin(); it != items.end(); ++it) {
		if (it->id == id) {
			items.erase(it);
			return;
		}
	}
}

}
