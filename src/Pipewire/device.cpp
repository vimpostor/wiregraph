#include "device.hpp"

namespace Pipewire {

Device::Device(Item& i)
{
	if (i.props.contains("device.name")) {
		this->name = i.props["device.name"];
	}
	if (i.props.contains("device.description")) {
		this->description = i.props["device.description"];
	}
	if (i.props.contains("device.nick")) {
		this->nick = i.props["device.nick"];
	}
	if (i.props.contains("device.api")) {
		this->api = i.props["device.api"];
	}
}

bool Device::is_audio_device(Item& i)
{
	return i.props.contains("media.class") && i.props["media.class"] == "Audio/Device";
}

}
