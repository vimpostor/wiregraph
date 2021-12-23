#pragma once

#include <QJSEngine>
#include <vector>
#include <QAbstractListModel>

#include "Pipewire/device.hpp"

class DeviceModel : public QAbstractListModel {
	Q_OBJECT
	QML_ELEMENT
	QML_SINGLETON
public:
	static DeviceModel* get() {
		static DeviceModel s {true};
		return &s;
	}
	static DeviceModel* create(QQmlEngine*, QJSEngine*) {
		auto* s = DeviceModel::get();
		QJSEngine::setObjectOwnership(s, QJSEngine::CppOwnership);
		return s;
	}

	virtual int rowCount(const QModelIndex&) const override;
	virtual QVariant data(const QModelIndex& index, int role) const override;
	virtual QHash<int, QByteArray> roleNames() const override;

	void add_device(Pipewire::Device d);
private:
	DeviceModel() = delete;
	explicit DeviceModel(const bool cpp) {};
	enum RoleNames {
		NameRole = Qt::UserRole,
		DescriptionRole,
		NickRole,
	};
	QHash<int, QByteArray> role_names {{NameRole, "name"}, {DescriptionRole, "description"}, {NickRole, "nick"}};
	std::vector<Pipewire::Device> devices;
};
