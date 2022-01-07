#include "pipewire.hpp"
#include <pipewire/core.h>
#include <spa/utils/dict.h>
#include <spa/debug/pod.h>

namespace Pipewire {

Api::~Api() {
	pw_thread_loop_lock(data.loop);
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
	uint32_t client_version;
	const void* events = nullptr;
	pw_destroy_t destroy;

	if (spa_streq(type, PW_TYPE_INTERFACE_Device)) {
		Database::get()->add_item({id, permissions, type, version, props});

		events = &device_events;
		client_version = PW_VERSION_DEVICE;
		destroy = reinterpret_cast<pw_destroy_t>(pw_device_info_free);
	} else if (spa_streq(type, PW_TYPE_INTERFACE_Node)) {
		events = &node_events;
		client_version = PW_VERSION_NODE;
		destroy = reinterpret_cast<pw_destroy_t>(pw_node_info_free);
	} else {
		qDebug() << "unhandled global registry event" << type;
		return;
	}

	struct pw_proxy* proxy = static_cast<pw_proxy*>(pw_registry_bind(d->registry, id, type, client_version, sizeof(struct Proxydata)));
	struct Proxydata* pd = static_cast<Proxydata*>(pw_proxy_get_user_data(proxy));
	pd->data = d;
	pd->proxy = proxy;
	pd->id = id;
	pd->permissions = permissions;
	pd->version = version;
	pd->type = strdup(type);
	pd->destroy = destroy;
	spa_list_init(&pd->param_list);
	pw_proxy_add_object_listener(proxy, &pd->object_listener, events, pd);
	pw_proxy_add_listener(proxy, &pd->proxy_listener, &proxy_events, pd);
}

void Api::registry_event_global_remove(void *object, uint32_t id) {
	Database::get()->remove_item(id);
}

void Api::destroy_proxy(void *data) {
	struct Proxydata* pd = static_cast<Proxydata*>(data);
	free(pd->type);

	if (pd->info == nullptr)
		return;
	if (pd->destroy)
		pd->destroy(pd->info);
	pd->info = nullptr;
}

void Api::removed_proxy(void *data) {
	struct Proxydata* pd = static_cast<Proxydata*>(data);
	pw_proxy_destroy(pd->proxy);
}

void Api::device_event_info(void *object, const struct pw_device_info *info) {
	auto* data = static_cast<struct Proxydata*>(object);
	info = pw_device_info_update(static_cast<pw_device_info*>(data->info), info);
	data->info = const_cast<pw_device_info*>(info);

	if (info->change_mask & PW_DEVICE_CHANGE_MASK_PARAMS) {
		for (uint32_t i = 0; i < info->n_params; ++i) {
			pw_device_enum_params(data->proxy, 0, info->params[i].id, 0, 0, nullptr);
		}
	}
}

void Api::device_event_param(void *object, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param) {
	qDebug() << "--------------";
	if (param->type == SPA_TYPE_Object) {
		auto* b = static_cast<spa_pod_object_body*>(SPA_POD_BODY(param));


		const spa_type_info* info = SPA_TYPE_ROOT;
		const spa_type_info* ti = spa_debug_type_find(SPA_TYPE_ROOT, b->type);
		info = ti ? ti->values : info;

		struct spa_pod_prop* p;
		SPA_POD_OBJECT_BODY_FOREACH(b, param->size, p) {
			const spa_type_info* ii = spa_debug_type_find(info, p->key);
			const char* name = ii ? spa_debug_type_short_name(ii->name) : nullptr;
			qDebug() << "Property:" << name;
			spa_debug_pod_value(0, nullptr, p->value.type, SPA_POD_CONTENTS(struct spa_pod_prop, p), p->value.size);
		}
	}
}

void Api::node_event_info(void *object, const struct pw_node_info *info) {
	auto* data = static_cast<struct Proxydata*>(object);
	info = pw_node_info_update(static_cast<pw_node_info*>(data->info), info);
	data->info = const_cast<pw_node_info*>(info);

	if (info->change_mask & PW_NODE_CHANGE_MASK_PARAMS) {
		for (uint32_t i = 0; i < info->n_params; ++i) {
			pw_node_enum_params(data->proxy, 0, info->params[i].id, 0, 0, nullptr);
		}
	}
}

void Api::node_event_param(void *object, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param) {
	// qDebug() << "node param changed" << param->type;
}

std::string Api::get_headers_version()
{
	return pw_get_headers_version();
}

std::string Api::get_library_version()
{
	return pw_get_library_version();
}

}
