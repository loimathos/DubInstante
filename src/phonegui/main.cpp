#include <QCoreApplication> // Added for QCoreApplication::exit
#include <QDebug>           // Added for qCritical()
#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include <QUrl>

using namespace Qt::StringLiterals;

int main(int argc, char *argv[]) {
  QGuiApplication app(argc, argv);
  app.setApplicationName("DubInstante");

  QQmlApplicationEngine engine;
  const QUrl url(u"qrc:/qt/qml/DubInstante/Main.qml"_s);
  QObject::connect(
      &engine, &QQmlApplicationEngine::objectCreated, &app,
      [url](QObject *obj, const QUrl &objUrl) {
        if (!obj && url == objUrl) {
          qCritical() << "Failed to load Main.qml from" << url;
          QCoreApplication::exit(-1);
        }
      },
      Qt::QueuedConnection);
  engine.load(url);

  return app.exec();
}
