#include "MainMenuView.h"

#include "data/GameText.h"

#include <QApplication>
#include <QGridLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QList>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

class MenuBackgroundLayer : public QWidget
{
public:
    explicit MenuBackgroundLayer(QWidget *parent = nullptr)
        : QWidget(parent)
        , background(GameText::Assets::mainMenuBackground())
    {
        setObjectName(QStringLiteral("MenuBackgroundLayer"));
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        if (!background.isNull()) {
            const double scale = qMax(width() / double(background.width()), height() / double(background.height()));
            const QSize targetSize(background.width() * scale, background.height() * scale);
            const QPoint topLeft((width() - targetSize.width()) / 2, (height() - targetSize.height()) / 2);
            painter.drawPixmap(QRect(topLeft, targetSize), background);
        } else {
            QLinearGradient fallback(rect().topLeft(), rect().bottomRight());
            fallback.setColorAt(0.0, QColor(44, 18, 24));
            fallback.setColorAt(0.55, QColor(116, 34, 36));
            fallback.setColorAt(1.0, QColor(18, 28, 38));
            painter.fillRect(rect(), fallback);
        }

        QLinearGradient shade(rect().topLeft(), rect().bottomRight());
        shade.setColorAt(0.0, QColor(18, 10, 12, 145));
        shade.setColorAt(0.45, QColor(18, 10, 12, 60));
        shade.setColorAt(1.0, QColor(7, 9, 14, 190));
        painter.fillRect(rect(), shade);
    }

private:
    QPixmap background;
};

MainMenuView::MainMenuView(QWidget *parent)
    : QWidget(parent)
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "MainMenuView { background: #141821; }"
        "QLabel { color: #fff7de; }"
        "QPushButton {"
        "  background: rgba(222, 170, 78, 225);"
        "  border: 2px solid rgba(255, 235, 177, 235);"
        "  border-radius: 8px;"
        "  color: #26150d;"
        "  font-size: 20px;"
        "  font-weight: 900;"
        "  padding: 10px 24px;"
        "}"
        "QPushButton:hover { background: rgba(246, 196, 92, 245); }"
        "QPushButton:pressed { background: rgba(174, 119, 48, 235); }"
        "QPushButton#QuitButton {"
        "  background: rgba(22, 24, 31, 165);"
        "  border-color: rgba(255, 235, 177, 150);"
        "  color: #f8e9c2;"
        "}"
        "QPushButton#QuitButton:hover { background: rgba(56, 45, 39, 200); }");

    MenuBackgroundLayer *backgroundLayer = new MenuBackgroundLayer(this);
    backgroundLayer->lower();

    QLabel *schoolLabel = new QLabel(GameText::App::schoolLabel(), this);
    schoolLabel->setStyleSheet("color: rgba(255,239,194,205); font-size: 18px; font-weight: 700;");

    QLabel *titleLabel = new QLabel(GameText::App::title(), this);
    titleLabel->setStyleSheet("font-size: 58px; font-weight: 900;");

    QLabel *subtitleLabel = new QLabel(GameText::App::subtitle(), this);
    subtitleLabel->setStyleSheet("color: rgba(255,248,226,220); font-size: 23px; font-weight: 600;");

    QPushButton *startButton = new QPushButton(GameText::Menu::startButton(), this);
    startButton->setCursor(Qt::PointingHandCursor);
    startButton->setFixedSize(210, 58);

    QPushButton *quitButton = new QPushButton(GameText::Menu::quitButton(), this);
    quitButton->setObjectName("QuitButton");
    quitButton->setCursor(Qt::PointingHandCursor);
    quitButton->setFixedSize(210, 52);

    QWidget *debugPanel = new QWidget(this);
    debugPanel->setStyleSheet(
        "QWidget { background: rgba(18, 22, 29, 140); border-radius: 8px; }"
        "QPushButton { font-size: 13px; padding: 7px 10px; border-radius: 6px; }");
    QGridLayout *debugLayout = new QGridLayout(debugPanel);
    debugLayout->setContentsMargins(10, 10, 10, 10);
    debugLayout->setSpacing(8);

    QPushButton *debugBattleButton = new QPushButton(GameText::Menu::debugBattleButton(), debugPanel);
    QPushButton *debugBossButton = new QPushButton(GameText::Menu::debugBossButton(), debugPanel);
    QPushButton *debugShopButton = new QPushButton(GameText::Menu::debugShopButton(), debugPanel);
    QPushButton *debugEventButton = new QPushButton(GameText::Menu::debugEventButton(), debugPanel);
    QPushButton *debugRewardButton = new QPushButton(GameText::Menu::debugRewardButton(), debugPanel);
    QPushButton *debugRestButton = new QPushButton(GameText::Menu::debugRestButton(), debugPanel);
    const QList<QPushButton*> debugButtons = {
        debugBattleButton, debugBossButton, debugShopButton, debugEventButton, debugRewardButton, debugRestButton
    };
    for (QPushButton *button : debugButtons) {
        button->setCursor(Qt::PointingHandCursor);
        button->setFixedSize(98, 34);
    }
    debugLayout->addWidget(debugBattleButton, 0, 0);
    debugLayout->addWidget(debugBossButton, 0, 1);
    debugLayout->addWidget(debugShopButton, 1, 0);
    debugLayout->addWidget(debugEventButton, 1, 1);
    debugLayout->addWidget(debugRewardButton, 2, 0);
    debugLayout->addWidget(debugRestButton, 2, 1);

    connect(startButton, &QPushButton::clicked, this, &MainMenuView::startRunRequested);
    connect(debugBattleButton, &QPushButton::clicked, this, &MainMenuView::debugBattleRequested);
    connect(debugBossButton, &QPushButton::clicked, this, &MainMenuView::debugBossRequested);
    connect(debugShopButton, &QPushButton::clicked, this, &MainMenuView::debugShopRequested);
    connect(debugEventButton, &QPushButton::clicked, this, &MainMenuView::debugEventRequested);
    connect(debugRewardButton, &QPushButton::clicked, this, &MainMenuView::debugRewardRequested);
    connect(debugRestButton, &QPushButton::clicked, this, &MainMenuView::debugRestRequested);
    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(72, 54, 72, 58);
    layout->setSpacing(8);
    layout->addWidget(schoolLabel);
    layout->addWidget(titleLabel);
    layout->addWidget(subtitleLabel);
    layout->addStretch();
    layout->addWidget(debugPanel);
    layout->addSpacing(10);
    layout->addWidget(startButton);
    layout->addSpacing(10);
    layout->addWidget(quitButton);
}

void MainMenuView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (QWidget *layer = findChild<QWidget*>(QStringLiteral("MenuBackgroundLayer"))) {
        layer->setGeometry(rect());
    }
}
