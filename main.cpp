#include "mainwindow.h"
#include "mapwidget.h"

#include <QApplication>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    MainWindow w;
    w.show();

    MapWidget map;
    map.show();

    return a.exec();
}
