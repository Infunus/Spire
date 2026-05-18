#include "mainwindow.h"
#include "ui/BattleWidget.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setWindowTitle(QStringLiteral("卷王博雅塔"));
    setCentralWidget(new BattleWidget(this));
}

MainWindow::~MainWindow()
{
    delete ui;
}
