#include "StatusBarWidget.h"

#include <QApplication>
#include <QDrag>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QPainter>
#include <QPushButton>
#include <QVBoxLayout>
#include <algorithm>

RelicIconWidget::RelicIconWidget(QWidget *parent)
    : QWidget(parent)
    , color(QColor(180, 170, 130))
{
    setFixedSize(30, 30);
    setMouseTracking(true);
}

void RelicIconWidget::setIcon(const QPixmap &pixmap)
{
    icon = pixmap;
    update();
}

void RelicIconWidget::setColor(const QColor &value)
{
    color = value;
    update();
}

void RelicIconWidget::setRelic(const RelicData &relic)
{
    icon = relic.iconPath.isEmpty() ? QPixmap() : QPixmap(relic.iconPath);
    setPopupText(relic.name, relic.effect);
    update();
}

void RelicIconWidget::setPopupText(const QString &title, const QString &description)
{
    popupTitle = title;
    popupDescription = description;
    setToolTip(QStringLiteral("%1\n%2").arg(title, description));
}

void RelicIconWidget::setDragPayload(const QString &mimeType, int index)
{
    dragMimeType = mimeType;
    dragIndex = index;
}

void RelicIconWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event)

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    if (!icon.isNull()) {
        QPixmap scaled = icon.scaled(size(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        const QPoint offset((width() - scaled.width()) / 2, (height() - scaled.height()) / 2);
        painter.drawPixmap(offset, scaled);
        return;
    }

    painter.setPen(QPen(QColor(245, 235, 190), 2));
    painter.setBrush(color);
    painter.drawEllipse(rect().adjusted(3, 3, -3, -3));
}

void RelicIconWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        dragStartPos = event->pos();
    }
    if (event->button() == Qt::LeftButton && (!popupTitle.isEmpty() || !popupDescription.isEmpty())) {
        emit clicked(popupTitle, popupDescription);
    }
    QWidget::mousePressEvent(event);
}

void RelicIconWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (!(event->buttons() & Qt::LeftButton) || dragMimeType.isEmpty() || dragIndex < 0) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    if ((event->pos() - dragStartPos).manhattanLength() < QApplication::startDragDistance()) {
        QWidget::mouseMoveEvent(event);
        return;
    }

    QDrag *drag = new QDrag(this);
    QMimeData *mimeData = new QMimeData;
    mimeData->setData(dragMimeType, QByteArray::number(dragIndex));
    mimeData->setText(popupTitle);
    drag->setMimeData(mimeData);
    if (!icon.isNull()) {
        drag->setPixmap(icon.scaled(34, 34, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        drag->setHotSpot(QPoint(17, 17));
    }
    drag->exec(Qt::MoveAction);
}

StatusBarWidget::StatusBarWidget(QWidget *parent)
    : QWidget(parent)
    , healthLabel(nullptr)
    , floorLabel(nullptr)
    , characterLabel(nullptr)
    , goldLabel(nullptr)
    , potionsLabel(nullptr)
    , potionPanel(new QWidget(this))
    , potionLayout(new QGridLayout(potionPanel))
    , mapButton(nullptr)
    , deckButton(nullptr)
    , settingsButton(nullptr)
    , relicPanel(new QWidget(this))
    , relicLayout(new QGridLayout(relicPanel))
{
    setFixedHeight(118);
    setAutoFillBackground(false);
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "StatusBarWidget { background: rgba(12, 16, 22, 210); }"
        "QLabel { color: #f5ead2; font-weight: 700; font-size: 16px; }"
        "QPushButton { color: #f8edd4; border: 1px solid rgba(255,255,255,80);"
        "              border-radius: 4px; padding: 7px 12px; font-weight: 700; }"
        "QPushButton:hover { border-color: rgba(255,255,255,170); }");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(16, 10, 16, 8);
    rootLayout->setSpacing(6);

    QFrame *topFrame = new QFrame(this);
    topFrame->setObjectName("topFrame");
    topFrame->setStyleSheet(
        "QFrame#topFrame { background: rgba(48, 38, 32, 230);"
        "                   border: 1px solid rgba(255, 230, 180, 90);"
        "                   border-radius: 6px; }");

    QHBoxLayout *topLayout = new QHBoxLayout(topFrame);
    topLayout->setContentsMargins(12, 8, 12, 8);
    topLayout->setSpacing(12);

    healthLabel = createStatusLabel(QStringLiteral("HP"), QStringLiteral("80/80"), QColor(190, 50, 48));
    floorLabel = createStatusLabel(QString::fromUtf8(u8"层数"), QStringLiteral("0"), QColor(80, 150, 220));
    characterLabel = createStatusLabel(QString::fromUtf8(u8"角色"), QString::fromUtf8(u8"铁甲战士"), QColor(150, 90, 210));
    goldLabel = createStatusLabel(QString::fromUtf8(u8"金币"), QStringLiteral("99"), QColor(220, 175, 60));
    potionsLabel = createStatusLabel(QString::fromUtf8(u8"药水"), QStringLiteral("0/3"), QColor(70, 190, 160));

    topLayout->addWidget(healthLabel);
    topLayout->addWidget(floorLabel);
    topLayout->addWidget(characterLabel);
    topLayout->addWidget(goldLabel);
    topLayout->addWidget(potionsLabel);

    potionPanel->setAttribute(Qt::WA_StyledBackground, true);
    potionPanel->setStyleSheet("background: transparent;");
    potionLayout->setContentsMargins(0, 0, 0, 0);
    potionLayout->setHorizontalSpacing(4);
    topLayout->addWidget(potionPanel);
    topLayout->addStretch();

    mapButton = createToolButton(QString::fromUtf8(u8"地图"), QColor(65, 120, 185));
    deckButton = createToolButton(QString::fromUtf8(u8"卡组"), QColor(110, 90, 185));
    settingsButton = createToolButton(QString::fromUtf8(u8"设置"), QColor(105, 110, 120));

    topLayout->addWidget(mapButton);
    topLayout->addWidget(deckButton);
    topLayout->addWidget(settingsButton);

    connect(mapButton, &QPushButton::clicked, this, &StatusBarWidget::mapButtonClicked);
    connect(deckButton, &QPushButton::clicked, this, &StatusBarWidget::deckButtonClicked);
    connect(settingsButton, &QPushButton::clicked, this, &StatusBarWidget::settingsButtonClicked);

    relicPanel->setAttribute(Qt::WA_StyledBackground, true);
    relicPanel->setStyleSheet("background: transparent;");
    relicLayout->setContentsMargins(0, 0, 0, 0);
    relicLayout->setHorizontalSpacing(1);
    relicLayout->setVerticalSpacing(1);
    relicLayout->setAlignment(Qt::AlignLeft | Qt::AlignTop);
    for (int column = 0; column < 15; ++column) {
        relicLayout->setColumnMinimumWidth(column, 31);
        relicLayout->setColumnStretch(column, 0);
    }

    rootLayout->addWidget(topFrame);
    rootLayout->addWidget(relicPanel);

    setRelics({});
    setPotionData({});
}

void StatusBarWidget::setHealth(int current, int maximum)
{
    healthLabel->setText(QStringLiteral("HP %1/%2").arg(current).arg(maximum));
}

void StatusBarWidget::setFloor(int floor)
{
    floorLabel->setText(QString::fromUtf8(u8"层数 %1").arg(floor));
}

void StatusBarWidget::setCharacterName(const QString &name)
{
    characterLabel->setText(QString::fromUtf8(u8"角色 %1").arg(name));
}

void StatusBarWidget::setGold(int gold)
{
    goldLabel->setText(QString::fromUtf8(u8"金币 %1").arg(gold));
}

void StatusBarWidget::setPotions(int count)
{
    potionsLabel->setText(QString::fromUtf8(u8"药水 %1").arg(count));
}

void StatusBarWidget::setPotionData(const std::vector<PotionData> &potions)
{
    potionsLabel->setText(QString::fromUtf8(u8"药水 %1/3").arg(potions.size()));
    rebuildPotions(potions);
}

void StatusBarWidget::setRelicCount(int count)
{
    rebuildRelics(std::vector<QPixmap>(std::max(0, count)));
}

void StatusBarWidget::setRelicIcons(const std::vector<QPixmap> &icons)
{
    rebuildRelics(icons);
}

void StatusBarWidget::setRelics(const std::vector<RelicData> &relics)
{
    rebuildRelics(relics);
}

QLabel* StatusBarWidget::createStatusLabel(const QString &title, const QString &value, const QColor &color)
{
    QLabel *label = new QLabel(QStringLiteral("%1 %2").arg(title, value), this);
    label->setMinimumWidth(98);
    label->setStyleSheet(QStringLiteral(
        "QLabel { color: #f5ead2; padding: 5px 9px 5px 28px;"
        "         border-radius: 4px; background: rgba(0,0,0,55);"
        "         border-left: 16px solid %1; }").arg(color.name()));
    return label;
}

QPushButton* StatusBarWidget::createToolButton(const QString &text, const QColor &color)
{
    QPushButton *button = new QPushButton(text, this);
    button->setMinimumWidth(74);
    button->setStyleSheet(QStringLiteral(
        "QPushButton { background: %1; }"
        "QPushButton:hover { background: %2; }").arg(color.name(), color.lighter(118).name()));
    return button;
}

void StatusBarWidget::rebuildRelics(const std::vector<QPixmap> &icons)
{
    while (QLayoutItem *item = relicLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    const QList<QColor> colors = {
        QColor(185, 72, 58),
        QColor(210, 155, 60),
        QColor(92, 145, 205),
        QColor(95, 175, 115),
        QColor(150, 95, 195),
    };

    for (int i = 0; i < static_cast<int>(icons.size()); ++i) {
        RelicIconWidget *relic = new RelicIconWidget(relicPanel);
        relic->setIcon(icons[i]);
        relic->setColor(colors[i % colors.size()]);
        relicLayout->addWidget(relic, i / 15, i % 15);
    }
}

void StatusBarWidget::rebuildRelics(const std::vector<RelicData> &relics)
{
    while (QLayoutItem *item = relicLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    const QList<QColor> colors = {
        QColor(185, 72, 58),
        QColor(210, 155, 60),
        QColor(92, 145, 205),
        QColor(95, 175, 115),
        QColor(150, 95, 195),
    };

    for (int i = 0; i < static_cast<int>(relics.size()); ++i) {
        RelicIconWidget *relicWidget = new RelicIconWidget(relicPanel);
        relicWidget->setColor(colors[i % colors.size()]);
        relicWidget->setRelic(relics[i]);
        connect(relicWidget, &RelicIconWidget::clicked, this, &StatusBarWidget::relicClicked);
        relicLayout->addWidget(relicWidget, i / 15, i % 15);
    }
}

void StatusBarWidget::rebuildPotions(const std::vector<PotionData> &potions)
{
    while (QLayoutItem *item = potionLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for (int i = 0; i < static_cast<int>(potions.size()) && i < 3; ++i) {
        RelicIconWidget *potionWidget = new RelicIconWidget(potionPanel);
        potionWidget->setIcon(QPixmap(potions[i].iconPath));
        potionWidget->setColor(QColor(80, 170, 210));
        potionWidget->setPopupText(potions[i].name, potions[i].effect);
        potionWidget->setDragPayload(QStringLiteral("application/x-spire-potion"), i);
        connect(potionWidget, &RelicIconWidget::clicked, this, [this, i]() {
            emit potionClicked(i);
        });
        potionLayout->addWidget(potionWidget, 0, i, Qt::AlignLeft | Qt::AlignVCenter);
    }
}
