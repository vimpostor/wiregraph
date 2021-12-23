#include "item.hpp"
#include <QDebug>

namespace Pipewire {

Item::Item(uint32_t id, uint32_t permissions, const char* type, uint32_t version, const struct spa_dict* props) {
	this->id = id;
	this->permissions = permissions;
	this->type = type;
	if (props) {
		const struct spa_dict_item *it;
		spa_dict_for_each(it, props) {
			this->props[it->key] = it->value;
		}
	}
}

}
