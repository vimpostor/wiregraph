#pragma once

#include "database.hpp"

namespace Pipewire {

struct Data {
	struct pw_thread_loop* loop = nullptr;
	struct pw_context* context = nullptr;
	struct pw_core_info* info = nullptr;
	struct pw_core* core = nullptr;
	struct pw_registry* registry = nullptr;
	struct spa_hook registry_listener;
};

struct Proxydata {
	struct Data* data;
	struct pw_proxy* proxy;
	uint32_t id;
	uint32_t permissions;
	uint32_t version;
	char* type;
	void* info;
	pw_destroy_t destroy;
	struct spa_hook proxy_listener;
	struct spa_hook object_listener;
	struct spa_list param_list;
};

class Api {
public:
	SINGLETON(Api);

	~Api();

	bool init();

	static void registry_event_global(void* data, uint32_t id, uint32_t permissions, const char* type, uint32_t version, const struct spa_dict* props);
	static void registry_event_global_remove(void* object, uint32_t id);
	constexpr static const struct pw_registry_events registry_events = {
		.version = PW_VERSION_REGISTRY_EVENTS,
		.global = registry_event_global,
		.global_remove = registry_event_global_remove,
	};

	static void destroy_proxy(void* data);
	static void removed_proxy(void* data);
	constexpr static const struct pw_proxy_events proxy_events = {
		.version = PW_VERSION_PROXY_EVENTS,
		.destroy = destroy_proxy,
		.removed = removed_proxy,
	};

	static void device_event_info(void *object, const struct pw_device_info *info);
	static void device_event_param(void *object, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param);
	constexpr static const struct pw_device_events device_events = {
		.version = PW_VERSION_DEVICE_EVENTS,
		.info = device_event_info,
		.param = device_event_param,
	};

	static void node_event_info(void *object, const struct pw_node_info *info);
	static void node_event_param(void *object, int seq, uint32_t id, uint32_t index, uint32_t next, const struct spa_pod *param);
	constexpr static const struct pw_node_events node_events = {
		.version = PW_VERSION_NODE_EVENTS,
		.info = node_event_info,
		.param = node_event_param,
	};

	static void clear_params(struct Proxydata* data);

	std::string get_headers_version();
	std::string get_library_version();
private:
	Data data;
};
};

