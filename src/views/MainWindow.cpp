#include "MainWindow.h"
#include "MapView.h"
#include "StatusBarWidget.h"
#include "core/GameManager.h"

#include <QStackedWidget>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mapView(new MapView(this))
    , statusBarWidget(new StatusBarWidget(this))
    , contentStack(new QStackedWidget(this))
    , placeholderWidget(new QWidget(this))
{
    setWindowTitle(tr("Slay The Spire Clone"));
    resize(1600, 960);
    setupUi();

    connect(statusBarWidget, &StatusBarWidget::mapButtonClicked, this, &MainWindow::toggleMapVisibility);
    connect(statusBarWidget, &StatusBarWidget::deckButtonClicked, this, []() {
        qDebug() << "请求打开卡组";
    });
    connect(statusBarWidget, &StatusBarWidget::settingsButtonClicked, this, []() {
        qDebug() << "请求打开设置";
    });

    connect(mapView, &MapView::floorChanged, statusBarWidget, &StatusBarWidget::setFloor);
    connect(GameManager::instance(), &GameManager::runDataChanged, this, &MainWindow::updateStatusBar);
    GameManager::instance()->startNewRun();
}

MainWindow::~MainWindow() = default;

void MainWindow::toggleMapVisibility()
{
    mapVisible = !mapVisible;
    contentStack->setCurrentWidget(mapVisible ? mapView : placeholderWidget);
    qDebug() << (mapVisible ? "打开地图" : "关闭地图");
}

void MainWindow::setupUi()
{
    placeholderWidget->setMinimumSize(900, 640);
    placeholderWidget->setStyleSheet("background: transparent;");

    contentStack->addWidget(placeholderWidget);
    contentStack->addWidget(mapView);
    contentStack->setCurrentWidget(placeholderWidget);

    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QVBoxLayout *layout = new QVBoxLayout(centralWidget);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(0);
    layout->addWidget(statusBarWidget);
    layout->addWidget(contentStack, 1);
}

void MainWindow::updateStatusBar()
{
    const RunData &data = GameManager::instance()->runData();
    statusBarWidget->setHealth(data.currentHp, data.maxHp);
    statusBarWidget->setFloor(data.currentFloor);
    statusBarWidget->setCharacterName(data.characterName);
    statusBarWidget->setGold(data.gold);
    statusBarWidget->setPotions(data.potions);
}
