#pragma once

#include "item.hpp"
#include <qqml.h>

namespace Pipewire {
class Device {
	QML_INTERFACE
public:
	Device(Item& i);

	static bool is_audio_device(Item& i);

	std::string name;
	std::string description;
	std::string nick;
	std::string api;
};
}
