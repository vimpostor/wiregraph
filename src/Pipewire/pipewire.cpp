#include "pipewire.hpp"

namespace Pipewire {

Api::~Api() {
	pw_thread_loop_lock(data.loop);
	pw_proxy_destroy(reinterpret_cast<struct pw_proxy*>(data.device));
	pw_proxy_destroy(reinterpret_cast<struct pw_proxy*>(data.registry));
	pw_core_disconnect(data.core);
	pw_context_destroy(data.context);
	pw_thread_loop_unlock(data.loop);

	pw_thread_loop_stop(data.loop);

	pw_thread_loop_destroy(data.loop);

	pw_deinit();
}

bool Api::init() {
	pw_init(nullptr, nullptr);

	headers_version = pw_get_headers_version();
	library_version = pw_get_library_version();

	data.loop = pw_thread_loop_new("wiregraph", nullptr);

	pw_thread_loop_lock(data.loop);

	auto* l = pw_thread_loop_get_loop(data.loop);

	data.context = pw_context_new(l, nullptr, 0);
	data.core = pw_context_connect(data.context, nullptr, 0);
	data.registry = pw_core_get_registry(data.core, PW_VERSION_REGISTRY, 0);
	spa_zero(data.registry_listener);
	pw_registry_add_listener(data.registry, &data.registry_listener, &registry_events, &data);

	pw_thread_loop_start(data.loop);

	pw_thread_loop_unlock(data.loop);

	return true;
}

void Api::registry_event_global(void* data, uint32_t id, uint32_t permissions, const char* type, uint32_t version, const struct spa_dict* props) {
	struct Data* d = static_cast<struct Data*>(data);
	if (spa_streq(type, PW_TYPE_INTERFACE_Device)) {
		Database::get()->add_item({id, permissions, type, version, props});
		if (d->device == nullptr) {
			d->device = static_cast<pw_device*>(pw_registry_bind(d->registry, id, type, PW_VERSION_DEVICE, 0));
			pw_device_add_listener(d->device, &d->device_listener, &device_events, data);
			// pw_device_subscribe_params(d->device, );
			qDebug() << "added device";
		}
	} else {
		qDebug() << "global registry event" << type;
	}
}

void Api::registry_event_global_remove(void *object, uint32_t id) {
	Database::get()->remove_item(id);
}

void Api::device_event_info(void *object, const struct pw_device_info *info) {
	struct Data* d = static_cast<struct Data*>(object);
	qDebug() << "device info changed" << info->id;

	if (info->change_mask & PW_DEVICE_CHANGE_MASK_PARAMS) {
		for (uint32_t i = 0; i < info->n_params; ++i) {
			const auto id = info->params[i].id;
			pw_device_enum_params(d->device, 0, id, 0, -1, nullptr);
		}
	}
}

void Api::device_event_param(void *object, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param) {
	qDebug() << "param changed" << param->type;
}

std::string Api::get_headers_version()
{
	return headers_version;
}

std::string Api::get_library_version()
{
	return library_version;
}

}