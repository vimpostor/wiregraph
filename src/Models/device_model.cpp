#include "device_model.hpp"

int DeviceModel::rowCount(const QModelIndex&) const
{
	return devices.size();
}

QVariant DeviceModel::data(const QModelIndex& index, int role) const
{
	const auto& d = devices[index.row()];
	switch (role) {
	case NameRole: return QString::fromStdString(d.name);
	case DescriptionRole: return QString::fromStdString(d.description);
	case NickRole: return QString::fromStdString(d.nick);
	default: return QVariant();
	};
}

QHash<int, QByteArray> DeviceModel::roleNames() const
{
	return role_names;
}

void DeviceModel::add_device(Pipewire::Device d)
{
	beginInsertRows(QModelIndex(), devices.size(), devices.size() + 1);
	devices.emplace_back(d);
	endInsertRows();
}
