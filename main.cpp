#include <QGuiApplication>
#include <QQmlApplicationEngine>
#include "threadfilter.h"

int main(int argc, char *argv[])
{
    QGuiApplication app(argc, argv);
    qmlRegisterType<FilterConnector>("filter", 1, 0, "Filter");
    QQmlApplicationEngine engine;    
    engine.load(QUrl(QStringLiteral("qrc:/main.qml")));

    return app.exec();
}
