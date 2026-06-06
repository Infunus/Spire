#include "mainwindow.h"
#include "core/EventLibrary.h"
#include "core/GameState.h"
#include "core/GameText.h"
#include "ui/BattleWidget.h"
#include "ui/EventWidget.h"
#include "ui/MapWidget.h"
#include "ui/RewardWidget.h"
#include "ui/ShopWidget.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QLabel>
#include <QLinearGradient>
#include <QMenu>
#include <QMessageBox>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QSaveFile>
#include <QSizePolicy>
#include <QSlider>
#include <QStackedWidget>
#include <QStringList>
#include <QTimer>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QWidgetAction>

namespace
{
const char *RunStatusMoodValue = "RunStatusMoodValue";
const char *RunStatusCoinsValue = "RunStatusCoinsValue";
const char *RunStatusUsualScoreValue = "RunStatusUsualScoreValue";

QFrame *createRunStatusTile(QWidget *parent,
                            const QString &caption,
                            const QString &valueObjectName,
                            const QColor &accentColor)
{
    QFrame *tile = new QFrame(parent);
    tile->setObjectName("RunStatusTile");
    tile->setFixedHeight(34);
    tile->setMinimumWidth(132);
    tile->setStyleSheet(
        "QFrame#RunStatusTile {"
        "  background: rgba(8, 11, 18, 178);"
        "  border: 1px solid rgba(255, 226, 168, 80);"
        "  border-radius: 7px;"
        "}");

    QHBoxLayout *layout = new QHBoxLayout(tile);
    layout->setContentsMargins(10, 5, 12, 5);
    layout->setSpacing(8);

    QFrame *accent = new QFrame(tile);
    accent->setFixedSize(5, 22);
    accent->setStyleSheet(QStringLiteral(
                              "background: %1;"
                              "border-radius: 2px;")
                              .arg(accentColor.name()));

    QLabel *captionLabel = new QLabel(caption, tile);
    captionLabel->setStyleSheet(
        "color: rgba(255, 241, 204, 170);"
        "font-size: 12px;"
        "font-weight: 800;");

    QLabel *valueLabel = new QLabel(tile);
    valueLabel->setObjectName(valueObjectName);
    valueLabel->setMinimumWidth(42);
    valueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    valueLabel->setStyleSheet(
        "color: #fff7de;"
        "font-size: 16px;"
        "font-weight: 900;");

    layout->addWidget(accent);
    layout->addWidget(captionLabel);
    layout->addWidget(valueLabel, 1);

    return tile;
}

QWidget *createRunStatusStrip(QWidget *parent)
{
    QWidget *strip = new QWidget(parent);
    strip->setObjectName("RunStatusStrip");
    strip->setSizePolicy(QSizePolicy::Maximum, QSizePolicy::Fixed);

    QHBoxLayout *layout = new QHBoxLayout(strip);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(10);
    layout->addWidget(createRunStatusTile(strip,
                                          QStringLiteral("心情"),
                                          QString::fromLatin1(RunStatusMoodValue),
                                          QColor(214, 74, 77)));
    layout->addWidget(createRunStatusTile(strip,
                                          QStringLiteral("余额"),
                                          QString::fromLatin1(RunStatusCoinsValue),
                                          QColor(230, 185, 80)));
    layout->addWidget(createRunStatusTile(strip,
                                          QStringLiteral("平时分"),
                                          QString::fromLatin1(RunStatusUsualScoreValue),
                                          QColor(72, 188, 166)));

    return strip;
}

void setRunStatusValue(QWidget *root, const char *objectName, const QString &text)
{
    const QList<QLabel *> labels = root->findChildren<QLabel *>(QString::fromLatin1(objectName));
    for (QLabel *label : labels) {
        label->setText(text);
    }
}

void refreshRunStatusStrip(QWidget *root)
{
    if (!root) {
        return;
    }

    const GameState &state = GameState::instance();
    setRunStatusValue(root,
                      RunStatusMoodValue,
                      QStringLiteral("%1/%2").arg(qMax(0, state.hp())).arg(state.maxHp()));
    setRunStatusValue(root,
                      RunStatusCoinsValue,
                      QString::number(state.coins()));
    setRunStatusValue(root,
                      RunStatusUsualScoreValue,
                      QString::number(state.usualScore()));
}
}

class MenuBackgroundWidget : public QWidget
{
public:
    explicit MenuBackgroundWidget(const QString &imagePath, QWidget *parent = nullptr)
        : QWidget(parent),
          m_background(imagePath)
    {
        setObjectName("MenuBackgroundWidget");
        setAttribute(Qt::WA_StyledBackground, true);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        if (m_background.isNull()) {
            QLinearGradient fallback(rect().topLeft(), rect().bottomRight());
            fallback.setColorAt(0.0, QColor(44, 18, 24));
            fallback.setColorAt(0.55, QColor(116, 34, 36));
            fallback.setColorAt(1.0, QColor(18, 28, 38));
            painter.fillRect(rect(), fallback);
        } else {
            const QSize sourceSize = m_background.size();
            const double scale = qMax(width() / double(sourceSize.width()),
                                      height() / double(sourceSize.height()));
            const QSize targetSize(sourceSize.width() * scale, sourceSize.height() * scale);
            const QPoint targetTopLeft((width() - targetSize.width()) / 2,
                                       (height() - targetSize.height()) / 2);
            painter.drawPixmap(QRect(targetTopLeft, targetSize), m_background);
        }

        QLinearGradient shade(rect().topLeft(), rect().bottomRight());
        shade.setColorAt(0.0, QColor(18, 10, 12, 135));
        shade.setColorAt(0.45, QColor(18, 10, 12, 60));
        shade.setColorAt(1.0, QColor(7, 9, 14, 185));
        painter.fillRect(rect(), shade);
    }

private:
    QPixmap m_background;
};

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent),
      ui(new Ui::MainWindow),
      m_pages(new QStackedWidget(this)),
      m_mapPage(nullptr),
      m_battlePage(nullptr),
      m_eventPage(nullptr),
      m_shopPage(nullptr),
      m_rewardPage(nullptr),
      m_debugPage(nullptr),
      m_mapNodePage(nullptr),
      m_mapWidget(nullptr),
      m_battleWidget(nullptr),
      m_eventWidget(nullptr),
      m_shopWidget(nullptr),
      m_rewardWidget(nullptr),
      m_continueButton(nullptr),
      m_mapTopButton(nullptr),
      m_mapPreviewReturnPage(nullptr),
      m_activeMapNodeType(MapNodeType::None),
      m_musicVolumePercent(45),
      m_bgmPaused(false),
      m_mapPreviewMode(false)
{
    ui->setupUi(this);
    setWindowTitle(GameText::App::title());
    setMinimumSize(1040, 680);
    resize(1180, 760);

    m_pages->addWidget(createMenuPage());
    setCentralWidget(m_pages);
    setMusicVolume(m_musicVolumePercent);
    playWorldMusic();
}

MainWindow::~MainWindow()
{
    stopAllMusic();
    delete ui;
}

QWidget *MainWindow::createMenuPage()
{
    MenuBackgroundWidget *page = new MenuBackgroundWidget(assetPath(GameText::Assets::mainMenuBackground()), this);

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(72, 54, 72, 58);
    rootLayout->setSpacing(0);

    QLabel *schoolLabel = new QLabel(GameText::Menu::schoolLabel(), page);
    schoolLabel->setStyleSheet(
        "color: rgba(255, 239, 194, 205);"
        "font-size: 18px;"
        "font-weight: 700;");

    QLabel *titleLabel = new QLabel(GameText::App::title(), page);
    titleLabel->setStyleSheet(
        "color: #fff7de;"
        "font-size: 58px;"
        "font-weight: 900;");

    QLabel *subtitleLabel = new QLabel(GameText::Menu::subtitle(), page);
    subtitleLabel->setStyleSheet(
        "color: rgba(255, 248, 226, 220);"
        "font-size: 23px;"
        "font-weight: 600;");

    QVBoxLayout *titleLayout = new QVBoxLayout;
    titleLayout->setSpacing(8);
    titleLayout->addWidget(schoolLabel);
    titleLayout->addWidget(titleLabel);
    titleLayout->addWidget(subtitleLabel);

    QPushButton *continueButton = new QPushButton(QString::fromUtf8("\xE7\xBB\xA7\xE7\xBB\xAD\xE4\xBF\xAE\xE8\xAF\xBB"), page);
    continueButton->setObjectName("ContinueButton");
    continueButton->setCursor(Qt::PointingHandCursor);
    continueButton->setFixedSize(210, 52);
    m_continueButton = continueButton;

    QPushButton *startButton = new QPushButton(GameText::Menu::startButton(), page);
    startButton->setObjectName("StartButton");
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setFixedSize(210, 58);

    QPushButton *eventButton = new QPushButton(GameText::Menu::eventPreviewButton(), page);
    eventButton->setObjectName("EventButton");
    eventButton->setCursor(Qt::PointingHandCursor);
    eventButton->setFixedSize(210, 52);

    QPushButton *settingsButton = createSettingsButton(page);
    settingsButton->setFixedSize(210, 52);

    QPushButton *quitButton = new QPushButton(GameText::Menu::quitButton(), page);
    quitButton->setObjectName("QuitButton");
    quitButton->setCursor(Qt::PointingHandCursor);
    quitButton->setFixedSize(210, 52);

    const QString buttonStyle =
        "QPushButton {"
        "  background: rgba(222, 170, 78, 225);"
        "  border: 2px solid rgba(255, 235, 177, 235);"
        "  border-radius: 8px;"
        "  color: #26150d;"
        "  font-size: 20px;"
        "  font-weight: 900;"
        "}"
        "QPushButton:hover { background: rgba(246, 196, 92, 245); }"
        "QPushButton:pressed { background: rgba(174, 119, 48, 235); }"
        "QPushButton#ContinueButton {"
        "  background: rgba(54, 112, 91, 215);"
        "  border-color: rgba(177, 237, 197, 190);"
        "  color: #ecfff4;"
        "}"
        "QPushButton#ContinueButton:hover { background: rgba(73, 146, 116, 235); }"
        "QPushButton#EventButton {"
        "  background: rgba(40, 54, 78, 180);"
        "  border-color: rgba(185, 215, 255, 155);"
        "  color: #eaf2ff;"
        "}"
        "QPushButton#EventButton:hover { background: rgba(67, 86, 123, 220); }"
        "QPushButton#SettingsButton {"
        "  background: rgba(40, 54, 78, 180);"
        "  border-color: rgba(185, 215, 255, 155);"
        "  color: #eaf2ff;"
        "}"
        "QPushButton#SettingsButton:hover { background: rgba(67, 86, 123, 220); }"
        "QPushButton#SettingsButton::menu-indicator { image: none; width: 0; }"
        "QPushButton#QuitButton {"
        "  background: rgba(22, 24, 31, 165);"
        "  border-color: rgba(255, 235, 177, 150);"
        "  color: #f8e9c2;"
        "}"
        "QPushButton#QuitButton:hover { background: rgba(56, 45, 39, 200); }";
    continueButton->setStyleSheet(buttonStyle);
    startButton->setStyleSheet(buttonStyle);
    eventButton->setStyleSheet(buttonStyle);
    settingsButton->setStyleSheet(buttonStyle);
    quitButton->setStyleSheet(buttonStyle);

    QMenu *eventMenu = new QMenu(eventButton);
    eventMenu->setStyleSheet(
        "QMenu {"
        "  background: rgba(13, 18, 28, 245);"
        "  border: 1px solid rgba(185, 215, 255, 150);"
        "  color: #eaf2ff;"
        "  font-size: 14px;"
        "  font-weight: 800;"
        "  padding: 6px;"
        "}"
        "QMenu::item { padding: 8px 18px 8px 14px; border-radius: 5px; }"
        "QMenu::item:selected { background: rgba(67, 86, 123, 220); }"
        "QMenu::separator { height: 1px; background: rgba(185, 215, 255, 80); margin: 6px 4px; }");

    QAction *randomEventAction = eventMenu->addAction(QStringLiteral("随机事件"));
    connect(randomEventAction, &QAction::triggered, this, [this]() {
        showEventPreviewPage();
    });
    eventMenu->addSeparator();

    const QList<RandomEventData> previewEvents = EventLibrary::allEvents();
    for (const RandomEventData &eventData : previewEvents) {
        QAction *eventAction = eventMenu->addAction(eventData.title);
        connect(eventAction, &QAction::triggered, this, [this, eventData]() {
            showEventPreviewPage(eventData);
        });
    }
    eventButton->setMenu(eventMenu);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->setSpacing(14);
    buttonLayout->addWidget(continueButton);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(eventButton);
    buttonLayout->addWidget(settingsButton);
    buttonLayout->addWidget(quitButton);

    QFrame *debugPanel = new QFrame(page);
    debugPanel->setObjectName("DebugPanel");
    debugPanel->setFixedWidth(455);
    debugPanel->setStyleSheet(
        "QFrame#DebugPanel {"
        "  background: rgba(8, 11, 18, 145);"
        "  border: 1px solid rgba(185, 215, 255, 115);"
        "  border-radius: 8px;"
        "}"
        "QLabel {"
        "  color: rgba(234, 242, 255, 215);"
        "  font-size: 14px;"
        "  font-weight: 900;"
        "}"
        "QPushButton {"
        "  background: rgba(40, 54, 78, 150);"
        "  border: 1px solid rgba(185, 215, 255, 125);"
        "  border-radius: 6px;"
        "  color: #eaf2ff;"
        "  font-size: 14px;"
        "  font-weight: 800;"
        "  padding: 8px 10px;"
        "}"
        "QPushButton:hover { background: rgba(67, 86, 123, 210); }");

    QVBoxLayout *debugLayout = new QVBoxLayout(debugPanel);
    debugLayout->setContentsMargins(12, 10, 12, 12);
    debugLayout->setSpacing(8);

    QLabel *debugTitleLabel = new QLabel(GameText::Menu::debugTitle(), debugPanel);
    debugLayout->addWidget(debugTitleLabel);

    QGridLayout *debugGrid = new QGridLayout;
    debugGrid->setHorizontalSpacing(8);
    debugGrid->setVerticalSpacing(8);

    auto createDebugButton = [debugPanel](const QString &text) {
        QPushButton *button = new QPushButton(text, debugPanel);
        button->setCursor(Qt::PointingHandCursor);
        button->setMinimumHeight(36);
        return button;
    };

    QPushButton *debugBattleButton = createDebugButton(GameText::Menu::debugBattleButton());
    QPushButton *debugBossButton = createDebugButton(GameText::Menu::debugBossButton());
    QPushButton *debugEventButton = createDebugButton(GameText::Menu::debugEventButton());
    QPushButton *debugMapButton = createDebugButton(GameText::Menu::debugMapButton());
    QPushButton *debugRewardButton = createDebugButton(GameText::Menu::debugRewardButton());
    QPushButton *debugShopButton = createDebugButton(GameText::Menu::debugShopButton());
    QPushButton *debugRestButton = createDebugButton(GameText::Menu::debugRestButton());

    debugGrid->addWidget(debugBattleButton, 0, 0);
    debugGrid->addWidget(debugBossButton, 0, 1);
    debugGrid->addWidget(debugEventButton, 0, 2);
    debugGrid->addWidget(debugMapButton, 1, 0);
    debugGrid->addWidget(debugRewardButton, 1, 1);
    debugGrid->addWidget(debugShopButton, 1, 2);
    debugGrid->addWidget(debugRestButton, 2, 0);
    debugLayout->addLayout(debugGrid);

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addLayout(titleLayout, 1);
    middleLayout->addStretch(1);

    rootLayout->addLayout(middleLayout);
    rootLayout->addStretch(1);
    rootLayout->addLayout(buttonLayout);
    rootLayout->addSpacing(14);
    rootLayout->addWidget(debugPanel, 0, Qt::AlignLeft);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startNewRunFromMenu);
    connect(continueButton, &QPushButton::clicked, this, [this]() {
        if (!loadRunFromDisk()) {
            updateContinueButtonVisibility();
        }
    });
    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(debugBattleButton, &QPushButton::clicked, this, &MainWindow::showBattlePage);
    connect(debugBossButton, &QPushButton::clicked, this, &MainWindow::showBossBattlePage);
    connect(debugEventButton, &QPushButton::clicked, this, [this]() {
        showEventPreviewPage();
    });
    connect(debugMapButton, &QPushButton::clicked, this, [this]() {
        showMapPage(false);
    });
    connect(debugRewardButton, &QPushButton::clicked, this, [this]() {
        showRewardPage(false);
    });
    connect(debugShopButton, &QPushButton::clicked, this, [this]() {
        if (m_shopPage) {
            m_pages->removeWidget(m_shopPage);
            m_shopPage->deleteLater();
            m_shopPage = nullptr;
            m_shopWidget = nullptr;
        }
        m_shopPage = createShopPage(false);
        m_pages->addWidget(m_shopPage);
        m_pages->setCurrentWidget(m_shopPage);
    });
    connect(debugRestButton, &QPushButton::clicked, this, [this]() {
        showDebugPlaceholderPage(GameText::DebugText::restTitle(), GameText::DebugText::restBody());
    });

    updateContinueButtonVisibility();
    return page;
}

QPushButton *MainWindow::createSettingsButton(QWidget *parent)
{
    QPushButton *button = new QPushButton(QStringLiteral("设置"), parent);
    button->setObjectName("SettingsButton");
    button->setCursor(Qt::PointingHandCursor);
    button->setStyleSheet(
        "QPushButton#SettingsButton {"
        "  background: rgba(42, 55, 78, 190);"
        "  border: 1px solid rgba(185, 215, 255, 145);"
        "  border-radius: 6px;"
        "  color: #eaf2ff;"
        "  font-size: 15px;"
        "  font-weight: 850;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#SettingsButton:hover { background: rgba(67, 86, 123, 225); }"
        "QPushButton#SettingsButton::menu-indicator { image: none; width: 0; }");

    QMenu *settingsMenu = new QMenu(button);
    settingsMenu->setStyleSheet(
        "QMenu {"
        "  background: rgba(13, 18, 28, 248);"
        "  border: 1px solid rgba(185, 215, 255, 150);"
        "  color: #eaf2ff;"
        "  font-size: 14px;"
        "  font-weight: 800;"
        "  padding: 6px;"
        "}"
        "QMenu::item { padding: 8px 26px 8px 14px; border-radius: 5px; }"
        "QMenu::item:selected { background: rgba(67, 86, 123, 220); }"
        "QMenu::separator { height: 1px; background: rgba(185, 215, 255, 80); margin: 6px 4px; }");
    connect(settingsMenu, &QMenu::aboutToShow, this, [this, settingsMenu]() {
        populateSettingsMenu(settingsMenu);
    });
    button->setMenu(settingsMenu);

    return button;
}

void MainWindow::addSettingsButton(QHBoxLayout *layout, QWidget *parent)
{
    if (!layout) {
        return;
    }
    layout->addWidget(createSettingsButton(parent));
}

void MainWindow::populateSettingsMenu(QMenu *menu)
{
    if (!menu) {
        return;
    }

    menu->clear();

    QWidget *volumeWidget = new QWidget(menu);
    QHBoxLayout *volumeLayout = new QHBoxLayout(volumeWidget);
    volumeLayout->setContentsMargins(10, 6, 10, 6);
    volumeLayout->setSpacing(10);

    QLabel *volumeLabel = new QLabel(QStringLiteral("音量"), volumeWidget);
    volumeLabel->setStyleSheet("color: #eaf2ff; font-weight: 900;");
    QSlider *volumeSlider = new QSlider(Qt::Horizontal, volumeWidget);
    volumeSlider->setRange(0, 100);
    volumeSlider->setFixedWidth(150);
    volumeSlider->setValue(m_musicVolumePercent);
    QLabel *volumeValueLabel = new QLabel(QStringLiteral("%1%").arg(m_musicVolumePercent), volumeWidget);
    volumeValueLabel->setMinimumWidth(38);
    volumeValueLabel->setAlignment(Qt::AlignRight | Qt::AlignVCenter);
    volumeValueLabel->setStyleSheet("color: rgba(255, 241, 204, 220); font-weight: 900;");

    volumeLayout->addWidget(volumeLabel);
    volumeLayout->addWidget(volumeSlider);
    volumeLayout->addWidget(volumeValueLabel);

    QWidgetAction *volumeAction = new QWidgetAction(menu);
    volumeAction->setDefaultWidget(volumeWidget);
    menu->addAction(volumeAction);
    connect(volumeSlider, &QSlider::valueChanged, this, [this, volumeValueLabel](int value) {
        setMusicVolume(value);
        volumeValueLabel->setText(QStringLiteral("%1%").arg(m_musicVolumePercent));
    });

    QAction *bgmAction = menu->addAction(m_bgmPaused ? QStringLiteral("播放 BGM")
                                                     : QStringLiteral("暂停 BGM"));
    connect(bgmAction, &QAction::triggered, this, [this]() {
        setBgmPaused(!m_bgmPaused);
    });

    QAction *mainMenuAction = menu->addAction(QStringLiteral("返回主界面"));
    connect(mainMenuAction, &QAction::triggered, this, [this]() {
        returnToMainMenuFromSettings();
    });

    const bool runContext =
        m_pages
        && m_pages->currentIndex() != 0
        && GameState::instance().hasRunSeed()
        && (m_pages->currentWidget() == m_mapPage || m_activeMapNodeType != MapNodeType::None);
    if (runContext) {
        QAction *seedAction = menu->addAction(QStringLiteral("查看种子"));
        connect(seedAction, &QAction::triggered, this, [this]() {
            QMessageBox::information(this,
                                     QStringLiteral("当前种子"),
                                     QStringLiteral("本局种子：%1").arg(GameState::instance().runSeed()));
        });
    }

    if (canViewMapFromCurrentPage()) {
        menu->addSeparator();
        QAction *mapAction = menu->addAction(QStringLiteral("查看地图"));
        connect(mapAction, &QAction::triggered, this, &MainWindow::showMapPreviewFromCurrentNode);
    }
}

QWidget *MainWindow::createMapPage()
{
    QWidget *page = new QWidget(this);
    page->setObjectName("MapPage");
    page->setStyleSheet(
        "#MapPage { background: #07101a; }"
        "QFrame#TopBar {"
        "  background: #10131d;"
        "  border-bottom: 1px solid rgba(255, 226, 168, 95);"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(GameText::MapText::pageTitle(), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QWidget *statusStrip = createRunStatusStrip(topBar);

    m_mapTopButton = new QPushButton(GameText::Menu::backToMenuButton(), topBar);
    m_mapTopButton->setObjectName("MenuButton");
    m_mapTopButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(m_mapTopButton);

    m_mapWidget = new MapWidget(page);
    m_mapWidget->setBackgroundImage(assetPath(GameText::Assets::mapBackground()));
    m_mapWidget->setNodeHandler([this](MapNodeType nodeType) {
        openMapNode(nodeType);
    });

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_mapWidget, 1);
    refreshRunStatusStrip(page);

    connect(m_mapTopButton, &QPushButton::clicked, this, &MainWindow::handleMapTopButton);

    return page;
}

QWidget *MainWindow::createBattlePage(bool bossBattle, bool fromMap)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("BattlePage");
    page->setStyleSheet(
        "#BattlePage { background: #170f12; }"
        "QFrame#TopBar {"
        "  background: #1f1716;"
        "  border-bottom: 1px solid rgba(255, 226, 168, 95);"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(GameText::App::title(), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(fromMap ? GameText::MapText::backToMapButton()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    m_battleWidget = new BattleWidget(page);
    if (fromMap) {
        m_battleWidget->startMapBattle(bossBattle, [this](bool victory) {
            stopMusic();
            if (victory && m_activeMapNodeType == MapNodeType::Battle) {
                showRewardPage(true);
            } else {
                finishMapNode(victory);
            }
        });
    } else {
        m_battleWidget->startDebugBattle(bossBattle);
    }

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_battleWidget, 1);
    refreshRunStatusStrip(page);

    QTimer *statusRefreshTimer = new QTimer(page);
    statusRefreshTimer->setInterval(150);
    connect(statusRefreshTimer, &QTimer::timeout, page, [page]() {
        refreshRunStatusStrip(page);
    });
    statusRefreshTimer->start();

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        pauseMusic();
        if (fromMap) {
            showMapPage(false);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    playBattleMusic();

    return page;
}

QWidget *MainWindow::createEventPreviewPage(bool fromMap)
{
    return createEventPreviewPage(EventLibrary::randomEvent(), fromMap);
}

QWidget *MainWindow::createEventPreviewPage(const RandomEventData &eventData, bool fromMap)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("EventPreviewPage");
    page->setStyleSheet(
        "#EventPreviewPage { background: #090b12; }"
        "QFrame#TopBar {"
        "  background: #10131d;"
        "  border-bottom: 1px solid rgba(255, 226, 168, 95);"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(GameText::EventText::pageTitle(), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QPushButton *menuButton = new QPushButton(GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    m_eventWidget = new EventWidget(page);
    m_eventWidget->setBackgroundImage(assetPath(GameText::Assets::eventBackground()));
    m_eventWidget->setShowCompletionRewards(fromMap);

    m_eventWidget->setEvent(eventData);

    m_eventWidget->setChoiceHandler([this, fromMap](int choiceIndex, const RandomEventChoice &choice) {
        Q_UNUSED(choiceIndex);
        // 事件内容本身请优先写在 core/EventLibrary.h。
        if (EventLibrary::choiceHasCardReward(choice)) {
            if (fromMap) {
                EventLibrary::applyChoiceEffect(choice, false);
                GameState::instance().recordEventFinished();
            }
            showEventCardRewardPage(choice, fromMap);
            return;
        }

        if (fromMap) {
            EventLibrary::applyChoiceEffect(choice);
            GameState::instance().recordEventFinished();
            finishMapNode(true);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_eventWidget, 1);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            finishMapNode(false);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    return page;
}

QWidget *MainWindow::createShopPage(bool fromMap)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("ShopPage");
    page->setStyleSheet(
        "#ShopPage { background: #17130f; }"
        "QFrame#TopBar {"
        "  background: #1f1716;"
        "  border-bottom: 1px solid rgba(255, 226, 168, 95);"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(GameText::ShopText::pageTitle(), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QPushButton *menuButton = new QPushButton(fromMap ? GameText::MapText::backToMapButton()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    m_shopWidget = new ShopWidget(page);
    m_shopWidget->setFinishHandler([this, fromMap]() {
        if (fromMap) {
            finishMapNode(true);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_shopWidget, 1);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            finishMapNode(false);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    return page;
}

QWidget *MainWindow::createRewardPage(bool fromMap)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("RewardPage");
    page->setStyleSheet(
        "#RewardPage { background: #10131d; }"
        "QFrame#TopBar {"
        "  background: #1b1f2b;"
        "  border-bottom: 1px solid rgba(185, 215, 255, 100);"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(GameText::RewardText::pageTitle(), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QPushButton *menuButton = new QPushButton(fromMap ? GameText::MapText::backToMapButton()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    m_rewardWidget = new RewardWidget(page);
    m_rewardWidget->setFinishHandler([this, fromMap]() {
        if (fromMap) {
            finishMapNode(true);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });
    m_rewardWidget->openRewards();

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_rewardWidget, 1);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            finishMapNode(false);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    return page;
}

QWidget *MainWindow::createEventCardRewardPage(const RandomEventChoice &choice,
                                               bool fromMap,
                                               int rewardIndex,
                                               int rewardCount)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("RewardPage");
    page->setStyleSheet(
        "#RewardPage { background: #10131d; }"
        "QFrame#TopBar {"
        "  background: #1b1f2b;"
        "  border-bottom: 1px solid rgba(185, 215, 255, 100);"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(QStringLiteral("卡牌奖励"), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QPushButton *menuButton = new QPushButton(fromMap ? GameText::MapText::backToMapButton()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    const int totalRewards = qMax(1, rewardCount);
    const int currentRewardIndex = qBound(0, rewardIndex, totalRewards - 1);

    m_rewardWidget = new RewardWidget(page);
    m_rewardWidget->setFinishHandler([this, choice, fromMap, currentRewardIndex, totalRewards]() {
        if (currentRewardIndex + 1 < totalRewards) {
            showEventCardRewardPage(choice, fromMap, currentRewardIndex + 1);
        } else if (fromMap) {
            finishMapNode(true);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    QString rewardMessage;
    if (currentRewardIndex == 0) {
        rewardMessage = EventLibrary::effectSummaryLine(choice);
    }
    if (rewardMessage.isEmpty()) {
        rewardMessage = QStringLiteral("获得一张卡牌。");
    }
    m_rewardWidget->openEventCardReward(EventLibrary::cardRewardTargetId(choice), rewardMessage);

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_rewardWidget, 1);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            finishMapNode(true);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    return page;
}

QWidget *MainWindow::createMapNodePlaceholderPage(const QString &title, const QString &body)
{
    QWidget *page = createDebugPlaceholderPage(title, body);
    const QList<QPushButton *> buttons = page->findChildren<QPushButton *>(QStringLiteral("MenuButton"));
    for (QPushButton *button : buttons) {
        button->setText(GameText::MapText::backToMapButton());
        button->disconnect();
        connect(button, &QPushButton::clicked, this, [this]() {
            finishMapNode(false);
        });
    }

    QPushButton *completeButton = new QPushButton(GameText::MapText::completeNodeButton(), page);
    completeButton->setCursor(Qt::PointingHandCursor);
    completeButton->setFixedSize(150, 42);
    completeButton->setStyleSheet(
        "background: rgba(222, 170, 78, 225);"
        "border: 2px solid rgba(255, 235, 177, 235);"
        "border-radius: 8px;"
        "color: #26150d;"
        "font-size: 16px;"
        "font-weight: 900;");
    completeButton->move(width() / 2, height() / 2);
    completeButton->raise();

    QVBoxLayout *layout = qobject_cast<QVBoxLayout *>(page->layout());
    if (layout) {
        layout->insertWidget(layout->count() - 1, completeButton, 0, Qt::AlignCenter);
    }

    connect(completeButton, &QPushButton::clicked, this, [this]() {
        finishMapNode(true);
    });
    return page;
}

QWidget *MainWindow::createDebugPlaceholderPage(const QString &title, const QString &body)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("DebugPlaceholderPage");
    page->setStyleSheet(
        "#DebugPlaceholderPage { background: #10131d; }"
        "QFrame#TopBar {"
        "  background: #1b1f2b;"
        "  border-bottom: 1px solid rgba(185, 215, 255, 100);"
        "}"
        "QFrame#PlaceholderPanel {"
        "  background: rgba(8, 11, 18, 185);"
        "  border: 1px solid rgba(185, 215, 255, 125);"
        "  border-radius: 10px;"
        "}"
        "QPushButton#MenuButton {"
        "  background: rgba(255, 226, 168, 35);"
        "  border: 1px solid rgba(255, 226, 168, 120);"
        "  border-radius: 6px;"
        "  color: #f7ead0;"
        "  font-size: 15px;"
        "  font-weight: 700;"
        "  padding: 8px 14px;"
        "}"
        "QPushButton#MenuButton:hover { background: rgba(255, 226, 168, 70); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(0, 0, 0, 0);
    rootLayout->setSpacing(0);

    QFrame *topBar = new QFrame(page);
    topBar->setObjectName("TopBar");
    topBar->setFixedHeight(50);

    QHBoxLayout *topLayout = new QHBoxLayout(topBar);
    topLayout->setContentsMargins(18, 6, 18, 6);

    QLabel *titleLabel = new QLabel(title, topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QPushButton *menuButton = new QPushButton(GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    QFrame *panel = new QFrame(page);
    panel->setObjectName("PlaceholderPanel");
    panel->setMaximumWidth(760);

    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(34, 30, 34, 30);
    panelLayout->setSpacing(16);

    QLabel *nodeTitleLabel = new QLabel(title, panel);
    nodeTitleLabel->setAlignment(Qt::AlignCenter);
    nodeTitleLabel->setStyleSheet("color: #fff6dc; font-size: 34px; font-weight: 900;");

    QLabel *bodyLabel = new QLabel(body, panel);
    bodyLabel->setWordWrap(true);
    bodyLabel->setAlignment(Qt::AlignCenter);
    bodyLabel->setStyleSheet("color: rgba(255, 247, 224, 210); font-size: 18px; line-height: 150%;");

    panelLayout->addWidget(nodeTitleLabel);
    panelLayout->addWidget(bodyLabel);

    rootLayout->addWidget(topBar);
    rootLayout->addStretch();
    rootLayout->addWidget(panel, 0, Qt::AlignCenter);
    rootLayout->addStretch();

    connect(menuButton, &QPushButton::clicked, this, [this]() {
        m_pages->setCurrentIndex(0);
    });

    return page;
}

void MainWindow::startNewRunFromMenu()
{
    stopMusic();
    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }
    m_activeMapNodeType = MapNodeType::None;
    GameState::instance().resetForNewRun();
    showMapPage(true);
    saveRunToDisk();
}

void MainWindow::showMapPage(bool resetMap)
{
    if (m_mapPreviewMode) {
        m_mapPreviewMode = false;
        m_mapPreviewReturnPage = nullptr;
    }
    if (!m_mapPage) {
        m_mapPage = createMapPage();
        m_pages->addWidget(m_mapPage);
    }
    if (resetMap && m_mapWidget) {
        m_mapWidget->resetMap();
    }
    setMapPreviewMode(false);
    refreshRunStatusStrip(m_mapPage);
    m_pages->setCurrentWidget(m_mapPage);
}

void MainWindow::showMapPreviewFromCurrentNode()
{
    if (!canViewMapFromCurrentPage()) {
        return;
    }

    m_mapPreviewReturnPage = m_pages->currentWidget();
    if (!m_mapPage) {
        m_mapPage = createMapPage();
        m_pages->addWidget(m_mapPage);
    }

    m_mapPreviewMode = true;
    setMapPreviewMode(true);
    refreshRunStatusStrip(m_mapPage);
    m_pages->setCurrentWidget(m_mapPage);
}

void MainWindow::returnFromMapPreview()
{
    if (!m_mapPreviewMode) {
        return;
    }

    QWidget *returnPage = m_mapPreviewReturnPage;
    m_mapPreviewMode = false;
    m_mapPreviewReturnPage = nullptr;
    setMapPreviewMode(false);

    if (returnPage && m_pages->indexOf(returnPage) >= 0) {
        m_pages->setCurrentWidget(returnPage);
    }
}

void MainWindow::showBattlePage()
{
    stopMusic();
    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }

    m_battlePage = createBattlePage(false, false);
    m_pages->addWidget(m_battlePage);
    m_pages->setCurrentWidget(m_battlePage);
}

void MainWindow::showBossBattlePage()
{
    stopMusic();
    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }

    m_battlePage = createBattlePage(true, false);
    m_pages->addWidget(m_battlePage);
    m_pages->setCurrentWidget(m_battlePage);
}

void MainWindow::showEventPreviewPage()
{
    showEventPreviewPage(EventLibrary::randomEvent());
}

void MainWindow::showEventPreviewPage(const RandomEventData &eventData)
{
    if (m_eventPage) {
        m_pages->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
        m_eventWidget = nullptr;
    }

    m_eventPage = createEventPreviewPage(eventData, false);
    m_pages->addWidget(m_eventPage);
    m_pages->setCurrentWidget(m_eventPage);
}

void MainWindow::showRewardPage(bool fromMap)
{
    if (m_rewardPage) {
        m_pages->removeWidget(m_rewardPage);
        m_rewardPage->deleteLater();
        m_rewardPage = nullptr;
        m_rewardWidget = nullptr;
    }

    m_rewardPage = createRewardPage(fromMap);
    m_pages->addWidget(m_rewardPage);
    m_pages->setCurrentWidget(m_rewardPage);
}

void MainWindow::showEventCardRewardPage(const RandomEventChoice &choice, bool fromMap, int rewardIndex)
{
    if (m_rewardPage) {
        m_pages->removeWidget(m_rewardPage);
        m_rewardPage->deleteLater();
        m_rewardPage = nullptr;
        m_rewardWidget = nullptr;
    }

    m_rewardPage = createEventCardRewardPage(choice, fromMap, rewardIndex, EventLibrary::cardRewardCount(choice));
    m_pages->addWidget(m_rewardPage);
    m_pages->setCurrentWidget(m_rewardPage);
}

void MainWindow::showDebugPlaceholderPage(const QString &title, const QString &body)
{
    if (m_debugPage) {
        m_pages->removeWidget(m_debugPage);
        m_debugPage->deleteLater();
        m_debugPage = nullptr;
    }

    m_debugPage = createDebugPlaceholderPage(title, body);
    m_pages->addWidget(m_debugPage);
    m_pages->setCurrentWidget(m_debugPage);
}

void MainWindow::openMapNode(MapNodeType nodeType)
{
    const bool resumeExistingBattle =
        m_battlePage
        && (nodeType == MapNodeType::Battle || nodeType == MapNodeType::Boss)
        && m_activeMapNodeType == nodeType;

    m_activeMapNodeType = nodeType;
    GameState::instance().setCurrentNode(nodeType);

    if (nodeType == MapNodeType::Battle || nodeType == MapNodeType::Boss) {
        if (resumeExistingBattle) {
            m_pages->setCurrentWidget(m_battlePage);
            playBattleMusic();
            return;
        }
        if (m_battlePage) {
            stopMusic();
            m_pages->removeWidget(m_battlePage);
            m_battlePage->deleteLater();
            m_battlePage = nullptr;
            m_battleWidget = nullptr;
        }
        m_battlePage = createBattlePage(nodeType == MapNodeType::Boss, true);
        m_pages->addWidget(m_battlePage);
        m_pages->setCurrentWidget(m_battlePage);
        return;
    }

    if (nodeType == MapNodeType::Event) {
        if (m_eventPage) {
            m_pages->removeWidget(m_eventPage);
            m_eventPage->deleteLater();
            m_eventPage = nullptr;
            m_eventWidget = nullptr;
        }
        m_eventPage = createEventPreviewPage(true);
        m_pages->addWidget(m_eventPage);
        m_pages->setCurrentWidget(m_eventPage);
        return;
    }

    if (nodeType == MapNodeType::Shop) {
        if (m_shopPage) {
            m_pages->removeWidget(m_shopPage);
            m_shopPage->deleteLater();
            m_shopPage = nullptr;
            m_shopWidget = nullptr;
        }
        m_shopPage = createShopPage(true);
        m_pages->addWidget(m_shopPage);
        m_pages->setCurrentWidget(m_shopPage);
        return;
    }

    if (nodeType == MapNodeType::Reward) {
        showRewardPage(true);
        return;
    }

    if (m_mapNodePage) {
        m_pages->removeWidget(m_mapNodePage);
        m_mapNodePage->deleteLater();
        m_mapNodePage = nullptr;
    }

    if (nodeType == MapNodeType::Rest) {
        m_mapNodePage = createMapNodePlaceholderPage(GameText::DebugText::restTitle(), GameText::DebugText::restBody());
    } else {
        m_mapNodePage = createMapNodePlaceholderPage(GameText::DebugText::mapTitle(), GameText::DebugText::mapBody());
    }

    m_pages->addWidget(m_mapNodePage);
    m_pages->setCurrentWidget(m_mapNodePage);
}

void MainWindow::finishMapNode(bool completed)
{
    if (m_mapPreviewMode) {
        m_mapPreviewMode = false;
        m_mapPreviewReturnPage = nullptr;
        setMapPreviewMode(false);
    }

    if (m_battlePage && (m_activeMapNodeType == MapNodeType::Battle || m_activeMapNodeType == MapNodeType::Boss)) {
        stopMusic();
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }

    if (completed && m_mapWidget) {
        GameState::instance().advanceFloor(m_activeMapNodeType);
        m_mapWidget->completeCurrentNode();
    } else if (m_mapWidget) {
        m_mapWidget->cancelPendingNode();
    }

    m_activeMapNodeType = MapNodeType::None;
    showMapPage(false);
    saveRunToDisk();
}

bool MainWindow::canViewMapFromCurrentPage() const
{
    return m_pages
           && m_activeMapNodeType != MapNodeType::None
           && m_pages->currentWidget() != nullptr
           && m_pages->currentWidget() != m_mapPage;
}

void MainWindow::setMapPreviewMode(bool previewMode)
{
    if (m_mapWidget) {
        m_mapWidget->setInteractionEnabled(!previewMode);
    }
    if (m_mapTopButton) {
        m_mapTopButton->setText(previewMode ? QStringLiteral("返回节点")
                                            : GameText::Menu::backToMenuButton());
    }
}

void MainWindow::handleMapTopButton()
{
    if (m_mapPreviewMode) {
        returnFromMapPreview();
        return;
    }

    stopMusic();
    if (m_mapWidget) {
        m_mapWidget->cancelPendingNode();
    }
    m_activeMapNodeType = MapNodeType::None;
    saveRunToDisk();
    m_pages->setCurrentIndex(0);
}

void MainWindow::returnToMainMenuFromSettings()
{
    if (m_mapPreviewMode) {
        m_mapPreviewMode = false;
        m_mapPreviewReturnPage = nullptr;
        setMapPreviewMode(false);
    }
    if (m_activeMapNodeType != MapNodeType::None && m_mapWidget) {
        m_mapWidget->cancelPendingNode();
    }
    m_activeMapNodeType = MapNodeType::None;
    saveRunToDisk();
    stopMusic();
    m_pages->setCurrentIndex(0);
}

void MainWindow::playBattleMusic()
{
    if (m_bgmPaused) {
        m_battleAudioManager.pause();
        m_worldAudioManager.pause();
        return;
    }
    m_worldAudioManager.pause();
    m_battleAudioManager.playLoop(assetPath(GameText::Assets::battleMusic()));
}

void MainWindow::playWorldMusic()
{
    if (m_bgmPaused) {
        m_worldAudioManager.pause();
        return;
    }
    m_worldAudioManager.playPlaylistLoop(worldMusicFiles());
}

void MainWindow::pauseMusic()
{
    m_battleAudioManager.pause();
    playWorldMusic();
}

void MainWindow::stopMusic()
{
    m_battleAudioManager.stop();
    playWorldMusic();
}

void MainWindow::stopAllMusic()
{
    m_battleAudioManager.stop();
    m_worldAudioManager.stop();
}

void MainWindow::setMusicVolume(int volumePercent)
{
    m_musicVolumePercent = qBound(0, volumePercent, 100);
    const float volume = m_musicVolumePercent / 100.0f;
    m_battleAudioManager.setVolume(volume);
    m_worldAudioManager.setVolume(volume);
}

void MainWindow::setBgmPaused(bool paused)
{
    m_bgmPaused = paused;
    if (m_bgmPaused) {
        m_battleAudioManager.pause();
        m_worldAudioManager.pause();
        return;
    }

    const bool battleMusicContext =
        (m_pages && m_pages->currentWidget() == m_battlePage)
        || (m_mapPreviewMode
            && (m_activeMapNodeType == MapNodeType::Battle || m_activeMapNodeType == MapNodeType::Boss));

    if (battleMusicContext) {
        playBattleMusic();
    } else {
        playWorldMusic();
    }
}

QString MainWindow::saveDirectoryPath() const
{
    return QDir::current().filePath(QStringLiteral("saves"));
}

QString MainWindow::saveFilePath() const
{
    return QDir(saveDirectoryPath()).filePath(QStringLiteral("current_run.json"));
}

bool MainWindow::saveFileExists() const
{
    return QFileInfo(saveFilePath()).isFile();
}

void MainWindow::updateContinueButtonVisibility()
{
    if (m_continueButton) {
        m_continueButton->setVisible(saveFileExists());
    }
}

bool MainWindow::saveRunToDisk()
{
    if (!GameState::instance().hasRunSeed()) {
        updateContinueButtonVisibility();
        return false;
    }

    if (!m_mapWidget) {
        updateContinueButtonVisibility();
        return false;
    }

    QDir dir;
    if (!dir.mkpath(saveDirectoryPath())) {
        updateContinueButtonVisibility();
        return false;
    }

    QJsonObject root;
    root[QStringLiteral("version")] = 1;
    root[QStringLiteral("state")] = GameState::instance().toJson();
    root[QStringLiteral("map")] = m_mapWidget->toJson();

    QSaveFile file(saveFilePath());
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        updateContinueButtonVisibility();
        return false;
    }

    const QJsonDocument document(root);
    file.write(document.toJson(QJsonDocument::Indented));
    const bool saved = file.commit();
    updateContinueButtonVisibility();
    return saved;
}

bool MainWindow::loadRunFromDisk()
{
    QFile file(saveFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::warning(this, QStringLiteral("Save"), QStringLiteral("No save file found."));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        QMessageBox::warning(this, QStringLiteral("Save"), QStringLiteral("The save file could not be read."));
        return false;
    }

    const QJsonObject root = document.object();
    if (root.value(QStringLiteral("version")).toInt(0) != 1) {
        QMessageBox::warning(this, QStringLiteral("Save"), QStringLiteral("The save file version is not supported."));
        return false;
    }

    if (!GameState::instance().loadFromJson(root.value(QStringLiteral("state")).toObject())) {
        QMessageBox::warning(this, QStringLiteral("Save"), QStringLiteral("The saved run state is incomplete."));
        return false;
    }

    if (!m_mapPage) {
        m_mapPage = createMapPage();
        m_pages->addWidget(m_mapPage);
    }
    if (!m_mapWidget || !m_mapWidget->loadFromJson(root.value(QStringLiteral("map")).toObject())) {
        QMessageBox::warning(this, QStringLiteral("Save"), QStringLiteral("The saved map is incomplete."));
        return false;
    }

    m_mapPreviewMode = false;
    m_mapPreviewReturnPage = nullptr;
    m_activeMapNodeType = MapNodeType::None;
    setMapPreviewMode(false);
    stopMusic();
    showMapPage(false);
    updateContinueButtonVisibility();
    return true;
}

QStringList MainWindow::worldMusicFiles() const
{
    return QStringList()
        << assetPath(GameText::Assets::worldMusicBoyaTodo())
        << assetPath(GameText::Assets::worldMusicYanyuanDay())
        << assetPath(GameText::Assets::worldMusicPkuGrinder())
        << assetPath(GameText::Assets::worldMusicLakeWind());
}

QString MainWindow::assetPath(const QString &relativePath) const
{
    QStringList candidates;

    auto addCandidatesFrom = [&candidates, &relativePath](const QDir &startDir) {
        QDir dir(startDir);
        for (int i = 0; i < 8; ++i) {
            candidates << dir.filePath(relativePath);
            candidates << dir.filePath(QStringLiteral("Spire/%1").arg(relativePath));
            if (!dir.cdUp()) {
                break;
            }
        }
    };

    addCandidatesFrom(QDir(QCoreApplication::applicationDirPath()));
    addCandidatesFrom(QDir::current());
    addCandidatesFrom(QDir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath()));

    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return candidate;
        }
    }

    return candidates.first();
}
