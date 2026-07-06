#include "mainwindow.h"
#include "core/Card.h"
#include "core/EventLibrary.h"
#include "core/GameState.h"
#include "core/GameText.h"
#include "ui/BattleWidget.h"
#include "ui/EventWidget.h"
#include "ui/MapWidget.h"
#include "ui/RewardWidget.h"
#include "ui/RestWidget.h"
#include "ui/ShopWidget.h"
#include "ui_mainwindow.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QCoreApplication>
#include <QDialog>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QInputDialog>
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
#include <QTextEdit>
#include <QTimer>
#include <QtGlobal>
#include <QVBoxLayout>
#include <QWidgetAction>

#include <limits>

namespace
{
const char *RunStatusMoodValue = "RunStatusMoodValue";
const char *RunStatusCoinsValue = "RunStatusCoinsValue";
const char *RunStatusUsualScoreValue = "RunStatusUsualScoreValue";
const char *RunStatusDeckButton = "RunStatusDeckButton";

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

QPushButton *createRunStatusDeckButton(QWidget *parent)
{
    QPushButton *button = new QPushButton(QStringLiteral("查看卡组"), parent);
    button->setObjectName(QString::fromLatin1(RunStatusDeckButton));
    button->setCursor(Qt::PointingHandCursor);
    button->setFixedHeight(34);
    button->setMinimumWidth(104);
    button->setStyleSheet(
        "QPushButton#RunStatusDeckButton {"
        "  background: rgba(255, 226, 168, 42);"
        "  border: 1px solid rgba(255, 226, 168, 125);"
        "  border-radius: 7px;"
        "  color: #fff7de;"
        "  font-size: 14px;"
        "  font-weight: 900;"
        "  padding: 6px 12px;"
        "}"
        "QPushButton#RunStatusDeckButton:hover {"
        "  background: rgba(255, 226, 168, 78);"
        "  border-color: rgba(255, 238, 190, 180);"
        "}");
    return button;
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
    layout->addWidget(createRunStatusDeckButton(strip));

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

void startRunStatusRefreshTimer(QWidget *root)
{
    if (!root) {
        return;
    }

    QTimer *timer = new QTimer(root);
    timer->setInterval(200);
    QObject::connect(timer, &QTimer::timeout, root, [root]() {
        refreshRunStatusStrip(root);
    });
    timer->start();
}

QString viewMapButtonText()
{
    return QStringLiteral("查看地图");
}

double settlementGpaForScore(int score)
{
    return GameBalance::CourseGrade::gpaForTotalScore(score);
}

QString settlementRatingForScore(int score)
{
    if (score < 60) {
        return QStringLiteral("挂科");
    }
    if (score < 75) {
        return QStringLiteral("一般");
    }
    if (score < 85) {
        return QStringLiteral("良好");
    }
    if (score < 95) {
        return QStringLiteral("优秀");
    }
    if (score < 100) {
        return QStringLiteral("超优");
    }
    return QStringLiteral("彩虹");
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
      m_mapNodePage(nullptr),
      m_deathPage(nullptr),
      m_settlementPage(nullptr),
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

void MainWindow::closeEvent(QCloseEvent *event)
{
    saveRunToDisk();
    QMainWindow::closeEvent(event);
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

    QHBoxLayout *middleLayout = new QHBoxLayout;
    middleLayout->addLayout(titleLayout, 1);
    middleLayout->addStretch(1);

    rootLayout->addLayout(middleLayout);
    rootLayout->addStretch(1);
    rootLayout->addLayout(buttonLayout);

    connect(startButton, &QPushButton::clicked, this, &MainWindow::startNewRunFromMenu);
    connect(continueButton, &QPushButton::clicked, this, [this]() {
        if (!loadRunFromDisk()) {
            updateContinueButtonVisibility();
        }
    });
    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);

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

    if (m_pages && m_pages->currentIndex() != 0) {
        QAction *mainMenuAction = menu->addAction(QStringLiteral("返回主界面"));
        connect(mainMenuAction, &QAction::triggered, this, [this]() {
            returnToMainMenuFromSettings();
        });
    }

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

    topLayout->addWidget(m_mapTopButton);
    topLayout->addSpacing(14);
    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);

    m_mapWidget = new MapWidget(page);
    m_mapWidget->setBackgroundImage(assetPath(GameText::Assets::mapBackground()));
    m_mapWidget->setNodeHandler([this](MapNodeType nodeType) {
        openMapNode(nodeType);
    });

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_mapWidget, 1);
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

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

    QPushButton *menuButton = new QPushButton(fromMap ? viewMapButtonText()
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
            if (victory && m_activeMapNodeType == MapNodeType::Boss) {
                showSettlementPage();
            } else if (victory && m_activeMapNodeType == MapNodeType::Battle) {
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
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            showMapPreviewFromCurrentNode();
        } else {
            pauseMusic();
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

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(fromMap ? viewMapButtonText()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
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
                if (GameState::instance().isDead()) {
                    showDeathPage();
                    return;
                }
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
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            showMapPreviewFromCurrentNode();
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

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(fromMap ? viewMapButtonText()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
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
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            showMapPreviewFromCurrentNode();
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

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(fromMap ? viewMapButtonText()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
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
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            showMapPreviewFromCurrentNode();
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    return page;
}

QWidget *MainWindow::createRestPage(bool fromMap)
{
    QWidget *page = new QWidget(this);
    page->setObjectName("RestPage");
    page->setStyleSheet(
        "#RestPage { background: #111722; }"
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

    QLabel *titleLabel = new QLabel(GameText::MapText::restNodeName(), topBar);
    titleLabel->setStyleSheet("color: #f7ead0; font-size: 18px; font-weight: 900;");

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(fromMap ? viewMapButtonText()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
    topLayout->addStretch();
    addSettingsButton(topLayout, topBar);
    topLayout->addWidget(menuButton);

    RestWidget *restWidget = new RestWidget(page);
    restWidget->setFinishHandler([this, fromMap]() {
        if (fromMap) {
            finishMapNode(true);
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(restWidget, 1);
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            showMapPreviewFromCurrentNode();
        } else {
            m_pages->setCurrentIndex(0);
        }
    });

    return page;
}

QWidget *MainWindow::createDeathPage()
{
    QWidget *page = new QWidget(this);
    page->setObjectName("DeathPage");
    page->setStyleSheet(
        "#DeathPage { background: #100d12; }"
        "QFrame#DeathPanel {"
        "  background: rgba(255, 238, 207, 232);"
        "  border: 2px solid rgba(116, 43, 42, 190);"
        "  border-radius: 8px;"
        "}"
        "QLabel { color: #27150f; }"
        "QPushButton {"
        "  background: #6f2d32;"
        "  border: 1px solid #bd7b66;"
        "  border-radius: 7px;"
        "  color: #fff4df;"
        "  font-size: 16px;"
        "  font-weight: 900;"
        "  padding: 10px 18px;"
        "}"
        "QPushButton:hover { background: #8b3a3f; border-color: #ffd2a3; }");

    QFrame *panel = new QFrame(page);
    panel->setObjectName("DeathPanel");
    panel->setMinimumWidth(720);

    QLabel *titleLabel = new QLabel(QStringLiteral("喜报：您已被退学！"), panel);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 38px; font-weight: 900; color: #501717;");

    QLabel *bodyLabel = new QLabel(QStringLiteral("您无法承受成为卷王的压力，陷入了抑郁"), panel);
    bodyLabel->setAlignment(Qt::AlignCenter);
    bodyLabel->setWordWrap(true);
    bodyLabel->setStyleSheet("font-size: 19px; font-weight: 800; color: #3c2418;");

    const GameState &state = GameState::instance();
    QFrame *statsPanel = new QFrame(panel);
    statsPanel->setStyleSheet(
        "QFrame {"
        "  background: rgba(80, 23, 23, 28);"
        "  border: 1px solid rgba(80, 23, 23, 80);"
        "  border-radius: 8px;"
        "}"
        "QLabel { font-size: 16px; font-weight: 850; color: #2b1911; }");

    QGridLayout *statsLayout = new QGridLayout(statsPanel);
    statsLayout->setContentsMargins(18, 16, 18, 16);
    statsLayout->setHorizontalSpacing(28);
    statsLayout->setVerticalSpacing(10);

    auto addStat = [statsLayout, statsPanel](int row, int column, const QString &name, const QString &value) {
        QLabel *label = new QLabel(QStringLiteral("%1：%2").arg(name, value), statsPanel);
        statsLayout->addWidget(label, row, column);
    };

    addStat(0, 0, QStringLiteral("心情"), QStringLiteral("%1/%2").arg(qMax(0, state.hp())).arg(state.maxHp()));
    addStat(0, 1, QStringLiteral("平时分"), QString::number(state.usualScore()));
    addStat(1, 0, QStringLiteral("余额"), QString::number(state.coins()));
    addStat(1, 1, QStringLiteral("修读层数"), QString::number(state.currentFloor()));
    addStat(2, 0, QStringLiteral("击败敌人"), QString::number(state.defeatedEnemies()));
    addStat(2, 1, QStringLiteral("事件完成"), QString::number(state.eventsFinished()));
    addStat(3, 0, QStringLiteral("总评估分"), QString::number(state.totalScore()));

    QPushButton *deckButton = new QPushButton(QStringLiteral("查看卡组"), panel);
    QPushButton *restartButton = new QPushButton(QStringLiteral("重新来过"), panel);
    QPushButton *menuButton = new QPushButton(QStringLiteral("返回主界面"), panel);
    QPushButton *seedButton = new QPushButton(QStringLiteral("查看种子"), panel);

    connect(deckButton, &QPushButton::clicked, this, [this]() {
        showDeckDialog(QStringLiteral("本局卡组"));
    });
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::startNewRunFromMenu);
    connect(menuButton, &QPushButton::clicked, this, [this]() {
        m_activeMapNodeType = MapNodeType::None;
        stopMusic();
        m_pages->setCurrentIndex(0);
        updateContinueButtonVisibility();
    });
    connect(seedButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this,
                                 QStringLiteral("当前种子"),
                                 QStringLiteral("本局种子：%1").arg(GameState::instance().runSeed()));
    });

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(14);
    buttonLayout->addStretch();
    buttonLayout->addWidget(restartButton);
    buttonLayout->addWidget(menuButton);
    buttonLayout->addWidget(seedButton);
    buttonLayout->addStretch();

    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(40, 34, 40, 36);
    panelLayout->setSpacing(18);
    panelLayout->addWidget(titleLabel);
    panelLayout->addWidget(bodyLabel);
    panelLayout->addWidget(statsPanel);
    panelLayout->addWidget(deckButton, 0, Qt::AlignCenter);
    panelLayout->addLayout(buttonLayout);

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(54, 48, 54, 48);
    rootLayout->addStretch();
    rootLayout->addWidget(panel, 0, Qt::AlignCenter);
    rootLayout->addStretch();

    return page;
}

QWidget *MainWindow::createSettlementPage()
{
    QWidget *page = new QWidget(this);
    page->setObjectName("SettlementPage");
    page->setStyleSheet(
        "#SettlementPage { background: #0f1411; }"
        "QFrame#SettlementPanel {"
        "  background: rgba(245, 248, 221, 235);"
        "  border: 2px solid rgba(63, 116, 82, 190);"
        "  border-radius: 8px;"
        "}"
        "QLabel { color: #182316; }"
        "QPushButton {"
        "  background: #2f704f;"
        "  border: 1px solid #8fc79a;"
        "  border-radius: 7px;"
        "  color: #f4ffed;"
        "  font-size: 16px;"
        "  font-weight: 900;"
        "  padding: 10px 18px;"
        "}"
        "QPushButton:hover { background: #3f8c61; border-color: #d7ffb9; }");

    QFrame *panel = new QFrame(page);
    panel->setObjectName("SettlementPanel");
    panel->setMinimumWidth(760);

    const GameState &state = GameState::instance();
    const int totalScore = state.totalScore();
    const double gpa = settlementGpaForScore(totalScore);
    const QString rating = settlementRatingForScore(totalScore);

    QLabel *titleLabel = new QLabel(QStringLiteral("本学期结算"), panel);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 38px; font-weight: 900; color: #204b31;");

    QLabel *summaryLabel = new QLabel(QStringLiteral("总评：%1    GPA：%2    评级：%3")
                                          .arg(totalScore)
                                          .arg(QString::number(gpa, 'f', 2))
                                          .arg(rating),
                                      panel);
    summaryLabel->setAlignment(Qt::AlignCenter);
    summaryLabel->setWordWrap(true);
    summaryLabel->setStyleSheet("font-size: 21px; font-weight: 900; color: #1e3b27;");

    QFrame *statsPanel = new QFrame(panel);
    statsPanel->setStyleSheet(
        "QFrame {"
        "  background: rgba(32, 75, 49, 26);"
        "  border: 1px solid rgba(32, 75, 49, 80);"
        "  border-radius: 8px;"
        "}"
        "QLabel { font-size: 16px; font-weight: 850; color: #182316; }");

    QGridLayout *statsLayout = new QGridLayout(statsPanel);
    statsLayout->setContentsMargins(18, 16, 18, 16);
    statsLayout->setHorizontalSpacing(28);
    statsLayout->setVerticalSpacing(10);

    auto addStat = [statsLayout, statsPanel](int row, int column, const QString &name, const QString &value) {
        QLabel *label = new QLabel(QStringLiteral("%1：%2").arg(name, value), statsPanel);
        statsLayout->addWidget(label, row, column);
    };

    addStat(0, 0, QStringLiteral("心情"), QStringLiteral("%1/%2").arg(qMax(0, state.hp())).arg(state.maxHp()));
    addStat(0, 1, QStringLiteral("平时分"), QString::number(state.usualScore()));
    addStat(1, 0, QStringLiteral("余额"), QString::number(state.coins()));
    addStat(1, 1, QStringLiteral("修读层数"), QString::number(state.currentFloor()));
    addStat(2, 0, QStringLiteral("击败敌人"), QString::number(state.defeatedEnemies()));
    addStat(2, 1, QStringLiteral("事件完成"), QString::number(state.eventsFinished()));
    addStat(3, 0, QStringLiteral("期末成绩"), QString::number(state.finalExamScore()));

    QPushButton *deckButton = new QPushButton(QStringLiteral("查看卡组"), panel);
    QPushButton *restartButton = new QPushButton(QStringLiteral("再来一学期"), panel);
    QPushButton *seedButton = new QPushButton(QStringLiteral("显示种子"), panel);
    QPushButton *menuButton = new QPushButton(QStringLiteral("返回主菜单"), panel);

    connect(deckButton, &QPushButton::clicked, this, [this]() {
        showDeckDialog(QStringLiteral("结算卡组"));
    });
    connect(restartButton, &QPushButton::clicked, this, &MainWindow::startNewRunFromMenu);
    connect(seedButton, &QPushButton::clicked, this, [this]() {
        QMessageBox::information(this,
                                 QStringLiteral("当前种子"),
                                 QStringLiteral("本局种子：%1").arg(GameState::instance().runSeed()));
    });
    connect(menuButton, &QPushButton::clicked, this, [this]() {
        m_activeMapNodeType = MapNodeType::None;
        stopMusic();
        m_pages->setCurrentIndex(0);
        updateContinueButtonVisibility();
    });

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    buttonLayout->setSpacing(14);
    buttonLayout->addStretch();
    buttonLayout->addWidget(restartButton);
    buttonLayout->addWidget(seedButton);
    buttonLayout->addWidget(menuButton);
    buttonLayout->addStretch();

    QVBoxLayout *panelLayout = new QVBoxLayout(panel);
    panelLayout->setContentsMargins(40, 34, 40, 36);
    panelLayout->setSpacing(18);
    panelLayout->addWidget(titleLabel);
    panelLayout->addWidget(summaryLabel);
    panelLayout->addWidget(statsPanel);
    panelLayout->addWidget(deckButton, 0, Qt::AlignCenter);
    panelLayout->addLayout(buttonLayout);

    QVBoxLayout *rootLayout = new QVBoxLayout(page);
    rootLayout->setContentsMargins(54, 48, 54, 48);
    rootLayout->addStretch();
    rootLayout->addWidget(panel, 0, Qt::AlignCenter);
    rootLayout->addStretch();

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

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(fromMap ? viewMapButtonText()
                                                       : GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
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
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this, fromMap]() {
        if (fromMap) {
            showMapPreviewFromCurrentNode();
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
        button->setText(viewMapButtonText());
        button->disconnect();
        connect(button, &QPushButton::clicked, this, [this]() {
            showMapPreviewFromCurrentNode();
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

    QWidget *statusStrip = createRunStatusStrip(topBar);

    QPushButton *menuButton = new QPushButton(GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addSpacing(14);
    topLayout->addWidget(statusStrip);
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
    connectRunStatusDeckButton(page);
    refreshRunStatusStrip(page);
    startRunStatusRefreshTimer(page);

    connect(menuButton, &QPushButton::clicked, this, [this]() {
        m_pages->setCurrentIndex(0);
    });

    return page;
}

void MainWindow::startNewRunFromMenu()
{
    const bool discardExistingSave = saveFileExists();
    if (discardExistingSave) {
        QMessageBox confirmBox(this);
        confirmBox.setWindowTitle(QStringLiteral("放弃上次存档？"));
        confirmBox.setText(QStringLiteral("检测到上次修读存档。开始新学期会放弃上次进度。"));
        confirmBox.setInformativeText(QStringLiteral("同一时间最多保存一个存档，是否确认开始新学期？"));
        QPushButton *discardButton = confirmBox.addButton(QStringLiteral("放弃存档"), QMessageBox::DestructiveRole);
        QPushButton *cancelButton = confirmBox.addButton(QStringLiteral("取消"), QMessageBox::RejectRole);
        confirmBox.setDefaultButton(cancelButton);
        confirmBox.exec();
        if (confirmBox.clickedButton() != discardButton) {
            updateContinueButtonVisibility();
            return;
        }
    }

    quint32 customSeed = 0;
    while (true) {
        bool accepted = false;
        const QString seedText =
            QInputDialog::getText(this,
                                  QStringLiteral("新学期种子"),
                                  QStringLiteral("输入 1–4294967295 的整数种子；留空则随机生成："),
                                  QLineEdit::Normal,
                                  QString(),
                                  &accepted)
                .trimmed();
        if (!accepted) {
            updateContinueButtonVisibility();
            return;
        }
        if (seedText.isEmpty()) {
            break;
        }

        bool validNumber = false;
        const qulonglong enteredSeed = seedText.toULongLong(&validNumber, 10);
        if (validNumber
            && enteredSeed >= 1
            && enteredSeed <= std::numeric_limits<quint32>::max()) {
            customSeed = static_cast<quint32>(enteredSeed);
            break;
        }

        QMessageBox::warning(this,
                             QStringLiteral("种子格式错误"),
                             QStringLiteral("请输入 1–4294967295 之间的整数，或留空使用随机种子。"));
    }

    if (discardExistingSave) {
        deleteSaveFile();
    }

    stopMusic();
    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }
    if (m_deathPage) {
        m_pages->removeWidget(m_deathPage);
        m_deathPage->deleteLater();
        m_deathPage = nullptr;
    }
    if (m_settlementPage) {
        m_pages->removeWidget(m_settlementPage);
        m_settlementPage->deleteLater();
        m_settlementPage = nullptr;
    }
    m_activeMapNodeType = MapNodeType::None;
    if (customSeed == 0) {
        GameState::instance().resetForNewRun();
    } else {
        GameState::instance().resetForNewRunWithSeed(customSeed);
    }
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

void MainWindow::showDeathPage()
{
    if (m_mapPreviewMode) {
        m_mapPreviewMode = false;
        m_mapPreviewReturnPage = nullptr;
        setMapPreviewMode(false);
    }

    if (m_mapWidget) {
        m_mapWidget->cancelPendingNode();
    }

    m_activeMapNodeType = MapNodeType::None;
    GameState::instance().setCurrentNode(MapNodeType::None);
    deleteSaveFile();
    stopMusic();

    if (m_deathPage) {
        m_pages->removeWidget(m_deathPage);
        m_deathPage->deleteLater();
        m_deathPage = nullptr;
    }

    m_deathPage = createDeathPage();
    m_pages->addWidget(m_deathPage);
    m_pages->setCurrentWidget(m_deathPage);
    updateContinueButtonVisibility();
}

void MainWindow::showSettlementPage()
{
    if (m_mapPreviewMode) {
        m_mapPreviewMode = false;
        m_mapPreviewReturnPage = nullptr;
        setMapPreviewMode(false);
    }

    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }

    m_activeMapNodeType = MapNodeType::None;
    GameState::instance().setCurrentNode(MapNodeType::None);
    deleteSaveFile();
    stopMusic();

    if (m_settlementPage) {
        m_pages->removeWidget(m_settlementPage);
        m_settlementPage->deleteLater();
        m_settlementPage = nullptr;
    }

    m_settlementPage = createSettlementPage();
    m_pages->addWidget(m_settlementPage);
    m_pages->setCurrentWidget(m_settlementPage);
    updateContinueButtonVisibility();
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

    if (nodeType == MapNodeType::Rest) {
        if (m_mapNodePage) {
            m_pages->removeWidget(m_mapNodePage);
            m_mapNodePage->deleteLater();
            m_mapNodePage = nullptr;
        }
        m_mapNodePage = createRestPage(true);
        m_pages->addWidget(m_mapNodePage);
        m_pages->setCurrentWidget(m_mapNodePage);
        return;
    }

    m_activeMapNodeType = MapNodeType::None;
    GameState::instance().setCurrentNode(MapNodeType::None);
    if (m_mapWidget) {
        m_mapWidget->cancelPendingNode();
    }
    showMapPage(false);
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

    if (GameState::instance().isDead()) {
        showDeathPage();
        return;
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
    QFile file(saveFilePath());
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        return false;
    }

    const QJsonObject root = document.object();
    if (root.value(QStringLiteral("version")).toInt(0) != 1) {
        return false;
    }

    const QJsonObject state = root.value(QStringLiteral("state")).toObject();
    const QJsonObject map = root.value(QStringLiteral("map")).toObject();
    return !state.isEmpty()
           && !map.value(QStringLiteral("rooms")).toArray().isEmpty();
}

void MainWindow::updateContinueButtonVisibility()
{
    if (m_continueButton) {
        m_continueButton->setVisible(saveFileExists());
    }
}

void MainWindow::deleteSaveFile()
{
    if (saveFileExists()) {
        QFile::remove(saveFilePath());
    }
    updateContinueButtonVisibility();
}

void MainWindow::connectRunStatusDeckButton(QWidget *root)
{
    if (!root) {
        return;
    }

    const QList<QPushButton *> buttons =
        root->findChildren<QPushButton *>(QString::fromLatin1(RunStatusDeckButton));
    for (QPushButton *button : buttons) {
        connect(button, &QPushButton::clicked, this, [this]() {
            showDeckDialog(QStringLiteral("当前卡组"));
        });
    }
}

void MainWindow::showDeckDialog(const QString &title)
{
    const QList<Card> cards = GameState::instance().ownedCards();
    QStringList lines;
    for (int i = 0; i < cards.size(); ++i) {
        const Card card = cards.at(i);
        lines << QStringLiteral("%1. %2\n费用 %3\n%4")
                     .arg(i + 1)
                     .arg(card.displayName())
                     .arg(card.cost())
                     .arg(card.description());
    }
    if (lines.isEmpty()) {
        lines << QStringLiteral("当前没有卡牌。");
    }

    QDialog *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(title);
    dialog->resize(620, 520);
    dialog->setStyleSheet(
        "QDialog { background: #151922; }"
        "QTextEdit {"
        "  background: #f6ead0;"
        "  border: 2px solid rgba(80, 52, 32, 150);"
        "  border-radius: 8px;"
        "  color: #21160f;"
        "  font-size: 15px;"
        "  font-weight: 750;"
        "  padding: 10px;"
        "}");

    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);
    textEdit->setText(lines.join(QStringLiteral("\n\n")));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(14, 14, 14, 14);
    layout->addWidget(textEdit);
    dialog->show();
}

bool MainWindow::saveRunToDisk()
{
    if (!GameState::instance().hasRunSeed()) {
        updateContinueButtonVisibility();
        return false;
    }

    if (GameState::instance().isDead()) {
        deleteSaveFile();
        return false;
    }

    if (GameState::instance().bossDefeated()) {
        deleteSaveFile();
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
        QMessageBox::warning(this, QStringLiteral("存档"), QStringLiteral("未找到可用存档。"));
        return false;
    }

    QJsonParseError parseError;
    const QJsonDocument document = QJsonDocument::fromJson(file.readAll(), &parseError);
    if (parseError.error != QJsonParseError::NoError || !document.isObject()) {
        QMessageBox::warning(this, QStringLiteral("存档"), QStringLiteral("存档文件损坏，无法读取。"));
        return false;
    }

    const QJsonObject root = document.object();
    if (root.value(QStringLiteral("version")).toInt(0) != 1) {
        QMessageBox::warning(this, QStringLiteral("存档"), QStringLiteral("该存档版本不受支持。"));
        return false;
    }

    if (!GameState::instance().loadFromJson(root.value(QStringLiteral("state")).toObject())) {
        QMessageBox::warning(this, QStringLiteral("存档"), QStringLiteral("存档中的修读数据不完整。"));
        return false;
    }

    if (GameState::instance().isDead()) {
        showDeathPage();
        return true;
    }

    if (GameState::instance().bossDefeated()) {
        showSettlementPage();
        return true;
    }

    if (!m_mapPage) {
        m_mapPage = createMapPage();
        m_pages->addWidget(m_mapPage);
    }
    if (!m_mapWidget || !m_mapWidget->loadFromJson(root.value(QStringLiteral("map")).toObject())) {
        QMessageBox::warning(this, QStringLiteral("存档"), QStringLiteral("存档中的地图数据不完整。"));
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
