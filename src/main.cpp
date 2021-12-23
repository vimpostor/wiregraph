#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QQuickStyle>
#include <QIcon>
#include <QQmlContext>

#include "Pipewire/pipewire.hpp"

int main(int argc, char* argv[])
{
	QCoreApplication::setOrganizationName("wiregraph");
	QCoreApplication::setApplicationName("wiregraph");
	QQuickStyle::setStyle(QLatin1String("Material"));
	constexpr const char* materialVariantName = "QT_QUICK_CONTROLS_MATERIAL_VARIANT";
	if (!qEnvironmentVariableIsSet(materialVariantName)) {
		// dense Material style, more suited for desktop targets
		qputenv(materialVariantName, "Dense");
	}
	QGuiApplication app(argc, argv);

	QGuiApplication::setWindowIcon(QIcon::fromTheme("wiregraph", QIcon(":/wiregraph")));

	Pipewire::Api::get()->init();

	QQmlApplicationEngine engine;
	engine.addImportPath(QStringLiteral(":/"));

	// engine.rootContext()->setContextProperty("c_devices", DeviceModel::get());

	engine.load(QUrl(QLatin1String("qrc:/Backend/src/qml/main.qml")));
	if (engine.rootObjects().isEmpty())
		return -1;
	return QGuiApplication::exec();
}
