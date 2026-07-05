#include "mainwindow.h"
#include "core/GameText.h"

#include <QApplication>
#include <QCoreApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    QCoreApplication::setApplicationName(GameText::App::title());
    QCoreApplication::setApplicationVersion(QStringLiteral("1.0.0"));
    QCoreApplication::setOrganizationName(QStringLiteral("Infunus"));
    MainWindow w;
    w.show();
    return a.exec();
}
