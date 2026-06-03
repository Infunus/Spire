#include "mainwindow.h"
#include "core/GameState.h"
#include "core/GameText.h"
#include "ui/BattleWidget.h"
#include "ui/EventWidget.h"
#include "ui_mainwindow.h"

#include <QApplication>
#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QStackedWidget>
#include <QStringList>
#include <QtGlobal>
#include <QVBoxLayout>

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
      m_battlePage(nullptr),
      m_eventPage(nullptr),
      m_debugPage(nullptr),
      m_battleWidget(nullptr),
      m_eventWidget(nullptr)
{
    ui->setupUi(this);
    GameState::instance().resetForNewRun();
    setWindowTitle(GameText::App::title());
    setMinimumSize(1040, 680);
    resize(1180, 760);

    m_pages->addWidget(createMenuPage());
    setCentralWidget(m_pages);
}

MainWindow::~MainWindow()
{
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

    QPushButton *startButton = new QPushButton(GameText::Menu::startButton(), page);
    startButton->setObjectName("StartButton");
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setFixedSize(210, 58);

    QPushButton *eventButton = new QPushButton(GameText::Menu::eventPreviewButton(), page);
    eventButton->setObjectName("EventButton");
    eventButton->setCursor(Qt::PointingHandCursor);
    eventButton->setFixedSize(210, 52);

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
        "QPushButton#EventButton {"
        "  background: rgba(40, 54, 78, 180);"
        "  border-color: rgba(185, 215, 255, 155);"
        "  color: #eaf2ff;"
        "}"
        "QPushButton#EventButton:hover { background: rgba(67, 86, 123, 220); }"
        "QPushButton#QuitButton {"
        "  background: rgba(22, 24, 31, 165);"
        "  border-color: rgba(255, 235, 177, 150);"
        "  color: #f8e9c2;"
        "}"
        "QPushButton#QuitButton:hover { background: rgba(56, 45, 39, 200); }";
    startButton->setStyleSheet(buttonStyle);
    eventButton->setStyleSheet(buttonStyle);
    quitButton->setStyleSheet(buttonStyle);

    QVBoxLayout *buttonLayout = new QVBoxLayout;
    buttonLayout->setSpacing(14);
    buttonLayout->addWidget(startButton);
    buttonLayout->addWidget(eventButton);
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

    connect(startButton, &QPushButton::clicked, this, &MainWindow::showBattlePage);
    connect(eventButton, &QPushButton::clicked, this, &MainWindow::showEventPreviewPage);
    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);
    connect(debugBattleButton, &QPushButton::clicked, this, &MainWindow::showBattlePage);
    connect(debugBossButton, &QPushButton::clicked, this, &MainWindow::showBossBattlePage);
    connect(debugEventButton, &QPushButton::clicked, this, &MainWindow::showEventPreviewPage);
    connect(debugMapButton, &QPushButton::clicked, this, [this]() {
        showDebugPlaceholderPage(GameText::DebugText::mapTitle(), GameText::DebugText::mapBody());
    });
    connect(debugRewardButton, &QPushButton::clicked, this, [this]() {
        showDebugPlaceholderPage(GameText::DebugText::rewardTitle(), GameText::DebugText::rewardBody());
    });
    connect(debugShopButton, &QPushButton::clicked, this, [this]() {
        showDebugPlaceholderPage(GameText::DebugText::shopTitle(), GameText::DebugText::shopBody());
    });
    connect(debugRestButton, &QPushButton::clicked, this, [this]() {
        showDebugPlaceholderPage(GameText::DebugText::restTitle(), GameText::DebugText::restBody());
    });

    return page;
}

QWidget *MainWindow::createBattlePage(bool bossBattle)
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

    QPushButton *menuButton = new QPushButton(GameText::Menu::backToMenuButton(), topBar);
    menuButton->setObjectName("MenuButton");
    menuButton->setCursor(Qt::PointingHandCursor);

    topLayout->addWidget(titleLabel);
    topLayout->addStretch();
    topLayout->addWidget(menuButton);

    m_battleWidget = new BattleWidget(page);
    if (bossBattle) {
        m_battleWidget->startDebugBattle(true);
    }

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_battleWidget, 1);

    connect(menuButton, &QPushButton::clicked, this, [this]() {
        m_pages->setCurrentIndex(0);
    });

    return page;
}

QWidget *MainWindow::createEventPreviewPage()
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
    topLayout->addWidget(menuButton);

    m_eventWidget = new EventWidget(page);
    m_eventWidget->setBackgroundImage(assetPath(GameText::Assets::eventBackground()));

    RandomEventData previewEvent;
    previewEvent.title = GameText::EventText::previewTitle();
    previewEvent.body = GameText::EventText::previewBody();
    // 后续不同事件可以在这里填入自己的事件图，例如：
    // previewEvent.imagePath = assetPath(QStringLiteral("resources/events/library.png"));
    previewEvent.choices << RandomEventChoice{GameText::EventText::previewChoiceA(), QString()}
                         << RandomEventChoice{GameText::EventText::previewChoiceB(), QString()}
                         << RandomEventChoice{GameText::EventText::previewChoiceC(), QString()};
    m_eventWidget->setEvent(previewEvent);

    m_eventWidget->setChoiceHandler([](int choiceIndex, const RandomEventChoice &choice) {
        Q_UNUSED(choiceIndex);
        Q_UNUSED(choice);
        // 后续随机事件效果可以从这里接到 GameState / Player / Deck。
    });

    rootLayout->addWidget(topBar);
    rootLayout->addWidget(m_eventWidget, 1);

    connect(menuButton, &QPushButton::clicked, this, [this]() {
        m_pages->setCurrentIndex(0);
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

void MainWindow::showBattlePage()
{
    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }

    m_battlePage = createBattlePage(false);
    m_pages->addWidget(m_battlePage);
    m_pages->setCurrentWidget(m_battlePage);
}

void MainWindow::showBossBattlePage()
{
    if (m_battlePage) {
        m_pages->removeWidget(m_battlePage);
        m_battlePage->deleteLater();
        m_battlePage = nullptr;
        m_battleWidget = nullptr;
    }

    m_battlePage = createBattlePage(true);
    m_pages->addWidget(m_battlePage);
    m_pages->setCurrentWidget(m_battlePage);
}

void MainWindow::showEventPreviewPage()
{
    if (m_eventPage) {
        m_pages->removeWidget(m_eventPage);
        m_eventPage->deleteLater();
        m_eventPage = nullptr;
        m_eventWidget = nullptr;
    }

    m_eventPage = createEventPreviewPage();
    m_pages->addWidget(m_eventPage);
    m_pages->setCurrentWidget(m_eventPage);
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

QString MainWindow::assetPath(const QString &relativePath) const
{
    const QDir sourceDir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath());
    const QStringList candidates = {
        sourceDir.filePath(relativePath),
        QDir(QCoreApplication::applicationDirPath()).filePath(relativePath),
        QDir::current().filePath(relativePath)
    };

    for (const QString &candidate : candidates) {
        if (QFileInfo::exists(candidate)) {
            return candidate;
        }
    }

    return candidates.first();
}
