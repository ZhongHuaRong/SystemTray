#include <QGuiApplication>
#include <QApplication>
#include <QQmlApplicationEngine>
#include <QAbstractNativeEventFilter>
#include <windows.h>
#include <QDebug>
#include <QSystemTrayIcon>
#include <QStyle>
#include <QTimer>
#include "./systemtray.h"
#include "./popupwindow.h"

int main(int argc, char *argv[])
{
	QCoreApplication::setAttribute(Qt::AA_EnableHighDpiScaling);
	
	QApplication app(argc, argv);
	
	QQmlApplicationEngine engine;
	
	const QUrl url(QStringLiteral("qrc:/main.qml"));
	QObject::connect(&engine, &QQmlApplicationEngine::objectCreated,
					 &app, [url](QObject *obj, const QUrl &objUrl) {
		if (!obj && url == objUrl)
			QCoreApplication::exit(-1);
	}, Qt::QueuedConnection);
	
	qmlRegisterType<SystemTray>("an.qt.cxx", 1, 0, "SystemTray");
	qmlRegisterType<PopupWindow>("an.qt.ui", 1, 0, "PopupWindow");
	engine.load(url);
	return app.exec();
}

