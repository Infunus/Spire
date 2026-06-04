#include "MainWindow.h"

#include "BattleView.h"
#include "EventView.h"
#include "MainMenuView.h"
#include "MapView.h"
#include "RewardView.h"
#include "ShopView.h"
#include "StatusBarWidget.h"
#include "core/GameManager.h"
#include "data/CardData.h"
#include "data/EnemyData.h"
#include "data/EventData.h"
#include "data/PotionData.h"
#include "data/RelicData.h"

#include <QCheckBox>
#include <QDialog>
#include <QGridLayout>
#include <QLabel>
#include <QMessageBox>
#include <QPushButton>
#include <QScrollArea>
#include <QSlider>
#include <QStackedWidget>
#include <QVBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , mapView(new MapView(this))
    , mainMenuView(new MainMenuView(this))
    , eventView(new EventView(this))
    , battleView(new BattleView(this))
    , shopView(new ShopView(this))
    , rewardView(new RewardView(this))
    , statusBarWidget(new StatusBarWidget(this))
    , contentStack(new QStackedWidget(this))
    , placeholderWidget(new QWidget(this))
{
    setWindowTitle(tr("Slay The Boya"));
    resize(1600, 960);
    setupUi();

    connect(mainMenuView, &MainMenuView::startRunRequested, this, &MainWindow::startNewRunFromMenu);

    connect(statusBarWidget, &StatusBarWidget::mapButtonClicked, this, &MainWindow::toggleMapVisibility);
    connect(statusBarWidget, &StatusBarWidget::deckButtonClicked, this, &MainWindow::showDeckDialog);
    connect(statusBarWidget, &StatusBarWidget::settingsButtonClicked, this, &MainWindow::showSettingsDialog);
    connect(statusBarWidget, &StatusBarWidget::potionClicked, this, &MainWindow::handlePotionClicked);
    connect(statusBarWidget, &StatusBarWidget::relicClicked, this, &MainWindow::showInfoDialog);

    connect(mapView, &MapView::floorChanged, this, [](int completedFloor) {
        GameManager::instance()->setCompletedFloor(completedFloor);
    });
    connect(mapView, &MapView::eventActivated, this, [this](EventKind eventKind) {
        startSceneForEvent(static_cast<int>(eventKind));
    });
    connect(eventView, &EventView::eventFinished, this, [this]() {
        if (runEnded) {
            showMainMenu();
            return;
        }
        finishActiveScene();
    });
    connect(shopView, &ShopView::shopFinished, this, &MainWindow::finishActiveScene);
    connect(battleView, &BattleView::battleFinished, this, &MainWindow::showRewardScene);
    connect(rewardView, &RewardView::rewardsFinished, this, &MainWindow::finishRewardScene);
    connect(GameManager::instance(), &GameManager::runDataChanged, this, &MainWindow::updateStatusBar);
    connect(GameManager::instance(), &GameManager::itemAcquired, this, [this](const QString &text) {
        showInfoDialog(QString::fromUtf8(u8"获得"), text);
    });
    connect(GameManager::instance(), &GameManager::playerDied, this, [this]() {
        runEnded = true;
        activeSceneWidget = eventView;
        mapVisible = false;
        eventView->setEvent(EventCatalog::eventForKind(EventKind::Death));
        contentStack->setCurrentWidget(eventView);
    });

    showMainMenu();
}

MainWindow::~MainWindow() = default;

void MainWindow::toggleMapVisibility()
{
    if (runEnded || contentStack->currentWidget() == mainMenuView) {
        return;
    }

    if (activeSceneWidget) {
        if (contentStack->currentWidget() == mapView) {
            contentStack->setCurrentWidget(activeSceneWidget);
            mapVisible = false;
        } else {
            contentStack->setCurrentWidget(mapView);
            mapVisible = true;
        }
        return;
    }

    if (contentStack->currentWidget() == mapView) {
        return;
    }

    contentStack->setCurrentWidget(mapView);
    mapVisible = true;
}

void MainWindow::setupUi()
{
    placeholderWidget->setMinimumSize(900, 640);
    placeholderWidget->setStyleSheet("background: transparent;");

    contentStack->addWidget(mainMenuView);
    contentStack->addWidget(placeholderWidget);
    contentStack->addWidget(mapView);
    contentStack->addWidget(eventView);
    contentStack->addWidget(battleView);
    contentStack->addWidget(shopView);
    contentStack->addWidget(rewardView);
    contentStack->setCurrentWidget(mainMenuView);

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

    std::vector<PotionData> potions;
    potions.reserve(data.potionIds.size());
    for (const QString &potionId : data.potionIds) {
        potions.push_back(PotionCatalog::byId(potionId));
    }
    statusBarWidget->setPotionData(potions);

    std::vector<RelicData> relics;
    relics.reserve(data.relicIds.size());
    for (const QString &relicId : data.relicIds) {
        relics.push_back(RelicCatalog::byId(relicId));
    }
    statusBarWidget->setRelics(relics);
}

void MainWindow::startNewRunFromMenu()
{
    runEnded = false;
    activeSceneWidget = nullptr;
    mapVisible = true;
    GameManager::instance()->startNewRun();
    mapView->resetMap();
    statusBarWidget->setVisible(true);
    contentStack->setCurrentWidget(mapView);
}

void MainWindow::showMainMenu()
{
    runEnded = false;
    activeSceneWidget = nullptr;
    mapVisible = false;
    if (deckDialog) {
        deckDialog->close();
    }
    statusBarWidget->setVisible(false);
    contentStack->setCurrentWidget(mainMenuView);
}

void MainWindow::showDeckDialog()
{
    if (contentStack->currentWidget() == mainMenuView) {
        return;
    }

    if (deckDialog && deckDialog->isVisible()) {
        deckDialogLarge = !deckDialogLarge;
        deckDialog->resize(deckDialogLarge ? QSize(980, 720) : QSize(560, 420));
        return;
    }

    deckDialogLarge = false;
    deckDialog = new QDialog(this);
    deckDialog->setWindowTitle(QString::fromUtf8(u8"当前卡组"));
    deckDialog->resize(560, 420);
    deckDialog->setAttribute(Qt::WA_DeleteOnClose);
    connect(deckDialog, &QObject::destroyed, this, [this]() {
        deckDialog = nullptr;
        deckDialogLarge = false;
    });

    QScrollArea *scrollArea = new QScrollArea(deckDialog);
    scrollArea->setWidgetResizable(true);
    QWidget *content = new QWidget(scrollArea);
    QGridLayout *grid = new QGridLayout(content);
    grid->setContentsMargins(14, 14, 14, 14);
    grid->setSpacing(10);

    const QStringList deckIds = GameManager::instance()->runData().deckIds;
    for (int i = 0; i < deckIds.size(); ++i) {
        const CardData card = CardCatalog::byId(deckIds[i]);
        QLabel *cardLabel = new QLabel(QStringLiteral("%1\n%2").arg(card.name, card.description), content);
        cardLabel->setMinimumSize(150, 90);
        cardLabel->setWordWrap(true);
        cardLabel->setAlignment(Qt::AlignCenter);
        cardLabel->setStyleSheet("QLabel { color: #f5ead2; background: #433733; border: 1px solid #8f7768; border-radius: 6px; padding: 8px; }");
        grid->addWidget(cardLabel, i / 3, i % 3);
    }

    scrollArea->setWidget(content);
    QVBoxLayout *layout = new QVBoxLayout(deckDialog);
    layout->addWidget(scrollArea);
    deckDialog->show();
}

void MainWindow::showSettingsDialog()
{
    if (contentStack->currentWidget() == mainMenuView) {
        return;
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(QString::fromUtf8(u8"设置"));
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->resize(360, 240);

    QLabel *volumeLabel = new QLabel(QString::fromUtf8(u8"音量"), dialog);
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, dialog);
    volumeSlider->setRange(0, 100);
    volumeSlider->setValue(70);
    QCheckBox *pauseBox = new QCheckBox(QString::fromUtf8(u8"暂停"), dialog);
    QPushButton *mainMenuButton = new QPushButton(QString::fromUtf8(u8"返回主菜单"), dialog);
    QPushButton *closeButton = new QPushButton(QString::fromUtf8(u8"关闭"), dialog);

    connect(mainMenuButton, &QPushButton::clicked, this, [this, dialog]() {
        dialog->close();
        showMainMenu();
    });
    connect(closeButton, &QPushButton::clicked, dialog, &QDialog::close);

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(24, 20, 24, 20);
    layout->setSpacing(12);
    layout->addWidget(volumeLabel);
    layout->addWidget(volumeSlider);
    layout->addWidget(pauseBox);
    layout->addStretch();
    layout->addWidget(mainMenuButton);
    layout->addWidget(closeButton);
    dialog->show();
}

void MainWindow::showInfoDialog(const QString &title, const QString &text)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, title, text, QMessageBox::Ok, this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void MainWindow::handlePotionClicked(int index)
{
    const RunData &run = GameManager::instance()->runData();
    if (index < 0 || index >= run.potionIds.size()) {
        return;
    }

    if (contentStack->currentWidget() == battleView || activeSceneWidget == battleView) {
        battleView->selectPotion(index);
        if (contentStack->currentWidget() == mapView) {
            contentStack->setCurrentWidget(battleView);
            mapVisible = false;
        }
        return;
    }

    const PotionData potion = PotionCatalog::byId(run.potionIds[index]);
    showInfoDialog(potion.name, potion.effect);
}

void MainWindow::startSceneForEvent(int eventKindValue)
{
    const EventKind eventKind = static_cast<EventKind>(eventKindValue);

    if (eventKind == EventKind::NormalBattle) {
        battleView->startBattle(EnemyCatalog::brute());
        activeSceneWidget = battleView;
    } else if (eventKind == EventKind::BossBattle || eventKind == EventKind::FinalBoss) {
        battleView->startBattle(EnemyCatalog::hydraBoss());
        activeSceneWidget = battleView;
    } else if (eventKind == EventKind::Merchant) {
        shopView->openShop();
        activeSceneWidget = shopView;
    } else {
        eventView->setEvent(EventCatalog::eventForKind(eventKind));
        activeSceneWidget = eventView;
    }

    mapVisible = false;
    contentStack->setCurrentWidget(activeSceneWidget);
}

void MainWindow::showRewardScene()
{
    if (runEnded) {
        return;
    }

    rewardView->openRewards();
    activeSceneWidget = rewardView;
    mapVisible = false;
    contentStack->setCurrentWidget(rewardView);
}

void MainWindow::finishRewardScene()
{
    finishActiveScene();
}

void MainWindow::finishActiveScene()
{
    if (!activeSceneWidget || runEnded) {
        return;
    }

    mapView->completeCurrentNode();
    activeSceneWidget = nullptr;
    mapVisible = true;
    contentStack->setCurrentWidget(mapView);
}
