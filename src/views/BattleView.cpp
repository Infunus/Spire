#include "BattleView.h"

#include "core/GameManager.h"
#include "data/GameText.h"

#include <QDialog>
#include <QDragEnterEvent>
#include <QDropEvent>
#include <QEvent>
#include <QFrame>
#include <QApplication>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHash>
#include <QHBoxLayout>
#include <QLabel>
#include <QList>
#include <QMouseEvent>
#include <QMimeData>
#include <QMessageBox>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QProgressBar>
#include <QPushButton>
#include <QPointer>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSizePolicy>
#include <QStyle>
#include <QStyleOptionButton>
#include <QStylePainter>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

class BattleCardButton : public QPushButton
{
public:
    explicit BattleCardButton(const QString &text, QWidget *parent = nullptr)
        : QPushButton(text, parent)
    {
    }

    void setPaintAngle(qreal value)
    {
        if (qFuzzyCompare(angle, value)) {
            return;
        }
        angle = value;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QStyleOptionButton option;
        initStyleOption(&option);

        QStylePainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        painter.translate(rect().center());
        painter.rotate(angle);
        painter.translate(-rect().center());
        painter.drawControl(QStyle::CE_PushButton, option);
    }

private:
    qreal angle = 0;
};

BattleView::BattleView(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , playerPortraitLabel(new QLabel(this))
    , enemyPortraitLabel(new QLabel(this))
    , enemyLabel(new QLabel(this))
    , enemyPreviewDamageLabel(new QLabel(this))
    , playerLabel(new QLabel(this))
    , logLabel(new QLabel(this))
    , playerHealthBar(new QProgressBar(this))
    , enemyHealthBar(new QProgressBar(this))
    , playerBlockBadge(new QLabel(this))
    , enemyBlockBadge(new QLabel(this))
    , playerStatusPanel(new QWidget(this))
    , enemyStatusPanel(new QWidget(this))
    , playerStatusLayout(new QHBoxLayout(playerStatusPanel))
    , enemyStatusLayout(new QHBoxLayout(enemyStatusPanel))
    , potionConfirmPanel(new QWidget(this))
    , potionConfirmLabel(new QLabel(potionConfirmPanel))
    , arenaPanel(new QFrame(this))
    , handPanel(new QWidget(this))
    , pileButtonLayout(new QHBoxLayout)
    , endTurnButton(new QPushButton(GameText::Battle::endTurnButton(), this))
    , drawPileButton(new QPushButton(this))
    , discardPileButton(new QPushButton(this))
    , exhaustPileButton(new QPushButton(this))
{
    setAcceptDrops(true);
    setStyleSheet(
        "BattleView {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                              stop:0 #2a1114, stop:0.48 #7b1d26, stop:1 #24100d);"
        "}"
        "QLabel { color: #f5ead2; font-size: 18px; }"
        "QPushButton {"
        "  color: #fff7de;"
        "  background: #8c4b22;"
        "  border: 2px solid #f4c86b;"
        "  border-radius: 12px;"
        "  padding: 10px 16px;"
        "  font-size: 15px;"
        "  font-weight: 800;"
        "}"
        "QPushButton:hover { background: #ad642b; border-color: #ffe0a1; }"
        "QPushButton:disabled { background: #3b201c; border-color: #7f5f47; color: #9d8b7a; }");

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet(
        "background: rgba(46, 30, 22, 125);"
        "border: 1px solid rgba(255, 226, 168, 95);"
        "border-radius: 14px;"
        "font-size: 24px;"
        "font-weight: 900;"
        "padding: 8px 18px;");
    playerPortraitLabel->setAlignment(Qt::AlignCenter);
    playerPortraitLabel->setMinimumSize(210, 250);
    playerPortraitLabel->setStyleSheet(
        "QLabel {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                              stop:0 #d7bd88, stop:0.6 #7b5132, stop:1 #261917);"
        "  border: 3px solid #f0d28a;"
        "  border-radius: 16px;"
        "  color: #fff3d0;"
        "  font-size: 15px;"
        "  font-weight: 900;"
        "  padding: 14px;"
        "}");
    enemyPortraitLabel->setAlignment(Qt::AlignCenter);
    enemyPortraitLabel->setMinimumSize(230, 230);
    enemyPortraitLabel->setStyleSheet(
        "QLabel {"
        "  background: rgba(36, 22, 18, 90);"
        "  border: 2px solid rgba(255, 226, 168, 120);"
        "  border-radius: 14px;"
        "  color: #fff3d0;"
        "  font-size: 14px;"
        "  font-weight: 800;"
        "  padding: 10px;"
        "}");
    enemyLabel->setAlignment(Qt::AlignCenter);
    enemyLabel->setStyleSheet(
        "QLabel {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                              stop:0 #f5e7c5, stop:0.64 #c08a4d, stop:1 #6c3d27);"
        "  border: 3px solid #6b4528;"
        "  border-radius: 16px;"
        "  color: #2b2118;"
        "  font-size: 15px;"
        "  font-weight: 900;"
        "  padding: 14px;"
        "}");
    enemyLabel->installEventFilter(this);
    enemyPortraitLabel->installEventFilter(this);
    enemyPreviewDamageLabel->setAlignment(Qt::AlignCenter);
    enemyPreviewDamageLabel->setStyleSheet(
        "QLabel { color: rgba(255, 70, 55, 150); background: rgba(30, 0, 0, 70);"
        "border-radius: 18px; font-size: 32px; font-weight: 900; padding: 4px 14px; }");
    enemyPreviewDamageLabel->hide();
    const QString healthBarStyle =
        "QProgressBar {"
        "  background: rgba(34, 18, 16, 190);"
        "  border: 2px solid rgba(255, 226, 168, 150);"
        "  border-radius: 8px;"
        "  color: #fff8de;"
        "  font-size: 13px;"
        "  font-weight: 900;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk { background: #b62e2b; border-radius: 6px; }";
    playerHealthBar->setFixedSize(214, 24);
    enemyHealthBar->setFixedSize(214, 24);
    playerHealthBar->setTextVisible(true);
    enemyHealthBar->setTextVisible(true);
    playerHealthBar->setStyleSheet(healthBarStyle);
    enemyHealthBar->setStyleSheet(healthBarStyle);
    auto setupBlockBadge = [](QLabel *badge) {
        badge->setFixedSize(42, 42);
        badge->setAlignment(Qt::AlignCenter);
        badge->setStyleSheet(
            "QLabel { background: #2f7cc5; border: 3px solid #bfe5ff; border-radius: 21px;"
            "color: white; font-size: 16px; font-weight: 900; }");
        badge->hide();
    };
    setupBlockBadge(playerBlockBadge);
    setupBlockBadge(enemyBlockBadge);
    playerStatusLayout->setContentsMargins(0, 0, 0, 0);
    playerStatusLayout->setSpacing(5);
    enemyStatusLayout->setContentsMargins(0, 0, 0, 0);
    enemyStatusLayout->setSpacing(5);
    playerStatusPanel->setStyleSheet("background: transparent;");
    enemyStatusPanel->setStyleSheet("background: transparent;");
    playerLabel->setAlignment(Qt::AlignCenter);
    playerLabel->setStyleSheet(
        "QLabel {"
        "  background: rgba(26, 21, 18, 120);"
        "  border: 2px solid rgba(255, 226, 168, 110);"
        "  border-radius: 18px;"
        "  padding: 10px 18px;"
        "}");
    logLabel->setAlignment(Qt::AlignCenter);
    logLabel->setWordWrap(true);
    logLabel->setMinimumSize(360, 126);
    logLabel->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::MinimumExpanding);
    logLabel->setStyleSheet(
        "QLabel {"
        "  background: rgba(26, 21, 18, 110);"
        "  border: 1px solid rgba(255, 226, 168, 90);"
        "  border-radius: 12px;"
        "  padding: 14px 18px;"
        "  font-size: 17px;"
        "}");

    QHBoxLayout *potionLayout = new QHBoxLayout(potionConfirmPanel);
    potionLayout->setContentsMargins(12, 8, 12, 8);
    potionLayout->setSpacing(8);
    QPushButton *confirmPotionButton = new QPushButton(GameText::Battle::potionConfirmButton(), potionConfirmPanel);
    QPushButton *cancelPotionButton = new QPushButton(GameText::Battle::potionCancelButton(), potionConfirmPanel);
    potionLayout->addWidget(potionConfirmLabel, 1);
    potionLayout->addWidget(confirmPotionButton);
    potionLayout->addWidget(cancelPotionButton);
    potionConfirmPanel->setStyleSheet("QWidget { background: rgba(30, 36, 44, 230); border-radius: 6px; }");
    potionConfirmPanel->hide();
    connect(confirmPotionButton, &QPushButton::clicked, this, &BattleView::useSelectedPotionOnPlayer);
    connect(cancelPotionButton, &QPushButton::clicked, this, &BattleView::hidePotionConfirm);

    pileButtonLayout->setSpacing(6);
    pileButtonLayout->addWidget(drawPileButton);
    pileButtonLayout->addWidget(discardPileButton);
    pileButtonLayout->addWidget(exhaustPileButton);
    connect(drawPileButton, &QPushButton::clicked, this, [this]() { showPileDialog(PileKind::Draw); });
    connect(discardPileButton, &QPushButton::clicked, this, [this]() { showPileDialog(PileKind::Discard); });
    connect(exhaustPileButton, &QPushButton::clicked, this, [this]() { showPileDialog(PileKind::Exhaust); });

    connect(endTurnButton, &QPushButton::clicked, this, &BattleView::endTurn);

    arenaPanel->setStyleSheet(
        "QFrame {"
        "  background: qradialgradient(cx:0.5, cy:0.45, radius:0.72,"
        "                              stop:0 #c8a76a, stop:0.68 #8b4a32, stop:1 #4d1818);"
        "  border: 3px solid rgba(255, 220, 145, 170);"
        "  border-radius: 28px;"
        "}");

    QGridLayout *arenaLayout = new QGridLayout(arenaPanel);
    arenaLayout->setContentsMargins(28, 16, 28, 16);
    arenaLayout->setHorizontalSpacing(22);
    arenaLayout->setVerticalSpacing(10);
    arenaLayout->setColumnStretch(0, 2);
    arenaLayout->setColumnStretch(1, 3);
    arenaLayout->setColumnStretch(2, 2);
    arenaLayout->addWidget(titleLabel, 0, 0, 1, 3);

    QWidget *playerPanel = new QWidget(arenaPanel);
    QVBoxLayout *playerPanelLayout = new QVBoxLayout(playerPanel);
    playerPanelLayout->setContentsMargins(0, 0, 0, 0);
    playerPanelLayout->setSpacing(8);
    playerPanelLayout->addStretch();
    playerPanelLayout->addWidget(playerPortraitLabel, 0, Qt::AlignCenter);
    QHBoxLayout *playerHealthLayout = new QHBoxLayout;
    playerHealthLayout->setContentsMargins(0, 0, 0, 0);
    playerHealthLayout->setSpacing(0);
    playerHealthLayout->addWidget(playerBlockBadge);
    playerHealthLayout->addWidget(playerHealthBar);
    playerPanelLayout->addLayout(playerHealthLayout);
    playerPanelLayout->addWidget(playerStatusPanel, 0, Qt::AlignCenter);
    playerPanelLayout->addStretch();

    QWidget *middlePanel = new QWidget(arenaPanel);
    QVBoxLayout *middleLayout = new QVBoxLayout(middlePanel);
    middleLayout->setContentsMargins(0, 0, 0, 0);
    middleLayout->setSpacing(10);
    middleLayout->addStretch();
    middleLayout->addWidget(logLabel, 0, Qt::AlignCenter);
    middleLayout->addWidget(potionConfirmPanel, 0, Qt::AlignHCenter);
    middleLayout->addStretch();

    QWidget *enemyPanel = new QWidget(arenaPanel);
    QVBoxLayout *enemyPanelLayout = new QVBoxLayout(enemyPanel);
    enemyPanelLayout->setContentsMargins(0, 0, 0, 0);
    enemyPanelLayout->setSpacing(8);
    enemyPanelLayout->addWidget(enemyPortraitLabel, 0, Qt::AlignCenter);
    enemyPanelLayout->addWidget(enemyPreviewDamageLabel, 0, Qt::AlignCenter);
    QHBoxLayout *enemyHealthLayout = new QHBoxLayout;
    enemyHealthLayout->setContentsMargins(0, 0, 0, 0);
    enemyHealthLayout->setSpacing(0);
    enemyHealthLayout->addWidget(enemyBlockBadge);
    enemyHealthLayout->addWidget(enemyHealthBar);
    enemyPanelLayout->addLayout(enemyHealthLayout);
    enemyPanelLayout->addWidget(enemyStatusPanel, 0, Qt::AlignCenter);
    enemyPanelLayout->addWidget(enemyLabel, 0, Qt::AlignCenter);
    enemyPanelLayout->addStretch();

    arenaLayout->addWidget(playerPanel, 1, 0);
    arenaLayout->addWidget(middlePanel, 1, 1);
    arenaLayout->addWidget(enemyPanel, 1, 2);

    QWidget *controlStrip = new QWidget(this);
    controlStrip->setStyleSheet(
        "QWidget { background: rgba(25, 19, 17, 120); border-radius: 10px; }"
        "QPushButton { border-radius: 8px; padding: 6px 10px; font-size: 13px; }");
    QHBoxLayout *controlLayout = new QHBoxLayout(controlStrip);
    controlLayout->setContentsMargins(10, 7, 10, 7);
    controlLayout->setSpacing(8);
    endTurnButton->setFixedSize(92, 34);
    drawPileButton->setFixedHeight(30);
    discardPileButton->setFixedHeight(30);
    exhaustPileButton->setFixedHeight(30);
    controlLayout->addWidget(playerLabel, 1);
    controlLayout->addStretch();
    controlLayout->addWidget(endTurnButton);
    controlLayout->addSpacing(10);
    controlLayout->addWidget(drawPileButton);
    controlLayout->addWidget(discardPileButton);
    controlLayout->addWidget(exhaustPileButton);

    handPanel->setMinimumHeight(240);
    handPanel->setAttribute(Qt::WA_StyledBackground, true);
    handPanel->setStyleSheet("QWidget { background: rgba(18, 12, 10, 80); border-radius: 18px; }");

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(18, 18, 18, 14);
    layout->setSpacing(12);
    layout->addWidget(arenaPanel, 1);
    layout->addWidget(controlStrip);
    layout->addWidget(handPanel);
}

void BattleView::startBattle(const EnemyData &enemyData)
{
    enemy = enemyData;
    enemyHp = enemy.maxHp;
    enemyBlock = 0;
    block = 0;
    maxEnergy = 3;
    energy = 0;
    baseDrawCount = 5;
    firstTurnDrawBonus = 0;
    firstTurnEnergyBonus = 0;
    playerStrength = 0;
    playerDexterity = 0;
    playerWeak = 0;
    playerPoison = 0;
    playerRegen = 0;
    enemyStrength = 0;
    enemyWeak = 0;
    enemyVulnerable = 0;
    enemyPoison = 0;
    enemyIntentIndex = 0;
    turnNumber = 0;
    selectedPotionIndex = -1;
    drawPile.clear();
    discardPile.clear();
    exhaustPile.clear();
    hand.clear();
    hidePotionConfirm();
    endTurnButton->setEnabled(true);

    setupBattlePiles();
    triggerBattleStartRelics();
    startPlayerTurn(true);
    logLabel->setText(QString::fromUtf8(u8"战斗开始。遗物已触发，抽牌堆已洗牌，进入第一回合。"));
}

bool BattleView::selectPotion(int potionIndex)
{
    const RunData &run = GameManager::instance()->runData();
    if (potionIndex < 0 || potionIndex >= run.potionIds.size()) {
        return false;
    }

    selectedPotionIndex = potionIndex;
    selectedPotion = PotionCatalog::byId(run.potionIds[potionIndex]);

    if (selectedPotion.targetsEnemy) {
        hidePotionConfirm();
        logLabel->setText(QString::fromUtf8(u8"已选择 %1：再点击怪物即可使用。\n%2")
                              .arg(selectedPotion.name, selectedPotion.effect));
    } else {
        potionConfirmLabel->setText(QString::fromUtf8(u8"%1\n%2").arg(selectedPotion.name, selectedPotion.effect));
        potionConfirmPanel->show();
        logLabel->setText(QString::fromUtf8(u8"已选择 %1，请确认是否使用。").arg(selectedPotion.name));
    }
    return true;
}

bool BattleView::eventFilter(QObject *watched, QEvent *event)
{
    if ((watched == enemyLabel || watched == enemyPortraitLabel) && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && selectedPotionIndex >= 0 && selectedPotion.targetsEnemy) {
            useSelectedPotionOnEnemy();
            return true;
        }
    }

    if (QWidget *cardWidget = qobject_cast<QWidget*>(watched)) {
        const QVariant cardIndexValue = cardWidget->property("cardIndex");
        if (cardIndexValue.isValid()) {
            const int handIndex = cardIndexValue.toInt();
            if (event->type() == QEvent::Enter && !draggingCardEntity && dragCardIndex < 0) {
                hoveredCardIndex = handIndex;
                layoutHandButtons(true);
                return false;
            } else if (event->type() == QEvent::Leave && hoveredCardIndex == handIndex && dragCardIndex < 0) {
                hoveredCardIndex = -1;
                layoutHandButtons(true);
                return false;
            } else if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    dragCardIndex = handIndex;
                    hoveredCardIndex = -1;
                    dragStartPos = mouseEvent->globalPosition().toPoint();
                    draggingCardEntity = false;
                    dragReturnGeometry = cardWidget->geometry();
                    cardWidget->grabMouse();
                    return true;
                }
            } else if (event->type() == QEvent::MouseMove && dragCardIndex == handIndex) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if ((mouseEvent->globalPosition().toPoint() - dragStartPos).manhattanLength() >= QApplication::startDragDistance()) {
                    QPushButton *cardButton = qobject_cast<QPushButton*>(cardWidget);
                    if (cardButton && !draggingCardEntity) {
                        const QRect globalRect(cardButton->mapToGlobal(QPoint(0, 0)), cardButton->size());
                        cardButton->setParent(this);
                        cardButton->setGeometry(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
                        if (BattleCardButton *battleCard = dynamic_cast<BattleCardButton*>(cardButton)) {
                            battleCard->setPaintAngle(0);
                        }
                        cardButton->show();
                        cardButton->raise();
                        draggingCardEntity = true;
                        layoutHandButtons(true);
                    }
                    if (cardButton) {
                        cardButton->move(mapFromGlobal(mouseEvent->globalPosition().toPoint()) - QPoint(cardButton->width() / 2, cardButton->height() / 2));
                        cardButton->raise();
                    }
                    if (handIndex >= 0 && handIndex < static_cast<int>(hand.size())) {
                        const QPoint globalPoint = mouseEvent->globalPosition().toPoint();
                        const bool hoveringEnemy =
                            enemyLabel->rect().contains(enemyLabel->mapFromGlobal(globalPoint)) ||
                            enemyPortraitLabel->rect().contains(enemyPortraitLabel->mapFromGlobal(globalPoint));
                        const CardData &card = hand[handIndex];
                        if (hoveringEnemy && card.damage > 0) {
                            int totalDamage = 0;
                            for (int hit = 0; hit < qMax(1, card.hitCount); ++hit) {
                                int damage = playerAttackDamage(card.damage + playerStrength * (qMax(1, card.strengthMultiplier) - 1));
                                if (enemyVulnerable > 0) {
                                    damage = damage + damage / 2;
                                }
                                totalDamage += qMax(0, damage);
                            }
                            totalDamage = qMax(0, totalDamage - enemyBlock);
                            enemyPreviewDamageLabel->setText(QStringLiteral("-%1").arg(totalDamage));
                            enemyPreviewDamageLabel->show();
                        } else {
                            enemyPreviewDamageLabel->hide();
                        }
                    }
                    return true;
                }
            } else if (event->type() == QEvent::MouseButtonRelease && dragCardIndex == handIndex) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                const QPoint globalPoint = mouseEvent->globalPosition().toPoint();
                const bool droppedOnEnemy =
                    enemyLabel->rect().contains(enemyLabel->mapFromGlobal(globalPoint)) ||
                    enemyPortraitLabel->rect().contains(enemyPortraitLabel->mapFromGlobal(globalPoint));
                const bool droppedOnBattleArea =
                    arenaPanel && arenaPanel->rect().contains(arenaPanel->mapFromGlobal(globalPoint));
                QPushButton *cardButton = qobject_cast<QPushButton*>(cardWidget);
                dragCardIndex = -1;
                cardWidget->releaseMouse();
                enemyPreviewDamageLabel->hide();
                const bool targetsEnemy = handIndex >= 0
                                          && handIndex < static_cast<int>(hand.size())
                                          && (hand[handIndex].damage > 0
                                              || hand[handIndex].enemyWeak > 0
                                              || hand[handIndex].enemyVulnerable > 0);
                const bool wouldPlay = (droppedOnEnemy && targetsEnemy)
                                       || (droppedOnBattleArea && !targetsEnemy);
                if (wouldPlay && handIndex >= 0 && handIndex < static_cast<int>(hand.size()) && energy < hand[handIndex].cost) {
                    logLabel->setText(QString::fromUtf8(u8"能量不足，无法使用 %1。").arg(hand[handIndex].name));
                    if (cardButton && draggingCardEntity) {
                        const QRect globalRect(cardButton->mapToGlobal(QPoint(0, 0)), cardButton->size());
                        cardButton->setParent(handPanel);
                        cardButton->show();
                        cardButton->setGeometry(QRect(handPanel->mapFromGlobal(globalRect.topLeft()), globalRect.size()));
                        cardButton->raise();
                        layoutHandButtons(true);
                    }
                    draggingCardEntity = false;
                    return true;
                }
                if (droppedOnEnemy && targetsEnemy) {
                    playDraggedCardIfValid(handIndex);
                    draggingCardEntity = false;
                    return true;
                }
                if (droppedOnBattleArea && handIndex >= 0 && handIndex < static_cast<int>(hand.size())) {
                    if (!targetsEnemy) {
                        playCard(handIndex);
                        draggingCardEntity = false;
                        return true;
                    }
                    logLabel->setText(QString::fromUtf8(u8"这张牌需要指定敌人目标。"));
                }
                if (cardButton && draggingCardEntity) {
                    const QRect globalRect(cardButton->mapToGlobal(QPoint(0, 0)), cardButton->size());
                    cardButton->setParent(handPanel);
                    cardButton->show();
                    cardButton->setGeometry(QRect(handPanel->mapFromGlobal(globalRect.topLeft()), globalRect.size()));
                    cardButton->raise();
                    layoutHandButtons(true);
                }
                draggingCardEntity = false;
            }
        }
    }
    return QWidget::eventFilter(watched, event);
}

void BattleView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutHandButtons(false);
}

void BattleView::dragEnterEvent(QDragEnterEvent *event)
{
    if (event->mimeData()->hasFormat(QStringLiteral("application/x-spire-potion"))) {
        event->acceptProposedAction();
        return;
    }
    QWidget::dragEnterEvent(event);
}

void BattleView::dropEvent(QDropEvent *event)
{
    if (!event->mimeData()->hasFormat(QStringLiteral("application/x-spire-potion"))) {
        QWidget::dropEvent(event);
        return;
    }

    const int potionIndex = QString::fromUtf8(event->mimeData()->data(QStringLiteral("application/x-spire-potion"))).toInt();
    if (!selectPotion(potionIndex)) {
        return;
    }

    const QPoint dropPoint = event->position().toPoint();
    const bool droppedOnEnemy =
        enemyLabel->geometry().contains(enemyLabel->parentWidget()->mapFrom(this, dropPoint)) ||
        enemyPortraitLabel->geometry().contains(enemyPortraitLabel->parentWidget()->mapFrom(this, dropPoint));

    if (selectedPotion.targetsEnemy && droppedOnEnemy) {
        useSelectedPotionOnEnemy();
        event->acceptProposedAction();
        return;
    }

    if (!selectedPotion.targetsEnemy) {
        potionConfirmPanel->show();
    }
    event->acceptProposedAction();
}

void BattleView::setupBattlePiles()
{
    for (const QString &cardId : GameManager::instance()->runData().deckIds) {
        drawPile.push_back(CardCatalog::byId(cardId));
    }
    shufflePile(drawPile);
}

void BattleView::triggerBattleStartRelics()
{
    const QStringList relics = GameManager::instance()->runData().relicIds;
    if (relics.contains(QStringLiteral("bag_of_preparation"))) {
        firstTurnDrawBonus += 2;
    }
    if (relics.contains(QStringLiteral("lantern")) || relics.contains(QStringLiteral("cracked_core"))) {
        firstTurnEnergyBonus += 1;
    }
    if (relics.contains(QStringLiteral("oddly_smooth_stone"))) {
        playerDexterity += 1;
    }
}

void BattleView::startPlayerTurn(bool firstTurn)
{
    ++turnNumber;

    // Step 1: default block is lost at the start of the player's turn.
    block = 0;

    // Step 2: gain energy. Ice Cream and boss relic rules can hook here later.
    energy = maxEnergy + (firstTurn ? firstTurnEnergyBonus : 0);

    // Step 3: draw cards. Ring/Snecko-style draw modifiers can hook into this count.
    drawCards(baseDrawCount + (firstTurn ? firstTurnDrawBonus : 0));

    // Step 4: start-of-turn effects.
    triggerStartTurnEffects();

    rebuildHandButtons();
    refreshLabels();
    refreshPileButtons();
}

void BattleView::discardHandAtEndTurn()
{
    std::vector<CardData> retainedCards;
    for (int handIndex = 0; handIndex < static_cast<int>(hand.size()); ++handIndex) {
        const CardData &card = hand[handIndex];
        if (card.ethereal) {
            animateCardToPile(handIndex, PileKind::Exhaust);
            exhaustPile.push_back(card);
        } else if (card.retain) {
            retainedCards.push_back(card);
        } else {
            animateCardToPile(handIndex, PileKind::Discard);
            discardPile.push_back(card);
        }
    }
    hand = retainedCards;
}

void BattleView::triggerPlayerEndTurnEffects()
{
    // Extension point: Metallicize, Flex expiration, Double Tap expiration, etc.
}

void BattleView::triggerStartTurnEffects()
{
    if (playerPoison > 0) {
        GameManager::instance()->takeDamage(playerPoison);
    }
    if (playerRegen > 0) {
        GameManager::instance()->heal(playerRegen);
        --playerRegen;
    }
    if (enemyPoison > 0) {
        enemyHp = qMax(0, enemyHp - enemyPoison);
        --enemyPoison;
    }
    if (enemyHp <= 0) {
        finishVictory();
    }
}

void BattleView::runEnemyTurn()
{
    if (enemyHp <= 0) {
        return;
    }

    const EnemyActionData action = enemy.actions.empty()
                                       ? EnemyActionData{EnemyIntent::Attack, enemy.baseAttack, 0}
                                       : enemy.actions[enemyIntentIndex % enemy.actions.size()];
    QStringList steps;

    auto doAttack = [&](int amount) {
        const int damage = qMax(0, enemyAttackDamage(amount) - block);
        block = qMax(0, block - enemyAttackDamage(amount));
        if (damage > 0) {
            GameManager::instance()->takeDamage(damage);
        }
        showDamagePopup(playerPortraitLabel, damage);
        steps << QString::fromUtf8(u8"攻击造成 %1 点伤害").arg(damage);
    };

    if (action.intent == EnemyIntent::Defend) {
        enemyBlock += action.amount;
        steps << QString::fromUtf8(u8"获得 %1 点格挡").arg(action.amount);
    } else if (action.intent == EnemyIntent::Buff) {
        enemyStrength += action.amount;
        steps << QString::fromUtf8(u8"力量 +%1").arg(action.amount);
    } else if (action.intent == EnemyIntent::Heal) {
        enemyHp = qMin(enemy.maxHp, enemyHp + action.amount);
        steps << QString::fromUtf8(u8"回复 %1 点生命").arg(action.amount);
    } else if (action.intent == EnemyIntent::AttackAndBuff) {
        doAttack(action.amount);
        enemyStrength += action.extra;
        steps << QString::fromUtf8(u8"力量 +%1").arg(action.extra);
    } else if (action.intent == EnemyIntent::AttackAndBlock) {
        doAttack(action.amount);
        enemyBlock += action.extra;
        steps << QString::fromUtf8(u8"获得 %1 点格挡").arg(action.extra);
    } else {
        doAttack(action.amount);
    }

    enemyIntentIndex = enemy.actions.empty() ? 0 : (enemyIntentIndex + 1) % enemy.actions.size();
    logLabel->setText(QString::fromUtf8(u8"%1 按顺序行动：%2。").arg(enemy.name, steps.join(QString::fromUtf8(u8"，"))));
}

void BattleView::reduceEndOfRoundStatus()
{
    if (playerWeak > 0) {
        --playerWeak;
    }
    if (enemyWeak > 0) {
        --enemyWeak;
    }
    if (enemyVulnerable > 0) {
        --enemyVulnerable;
    }
}

void BattleView::drawCards(int count)
{
    for (int i = 0; i < count; ++i) {
        if (drawPile.empty()) {
            shuffleDiscardIntoDrawPile();
        }
        if (drawPile.empty()) {
            break;
        }

        hand.push_back(drawPile.back());
        drawPile.pop_back();
        ++pendingInsertedCards;
    }
}

void BattleView::shuffleDiscardIntoDrawPile()
{
    if (discardPile.empty()) {
        return;
    }

    drawPile = discardPile;
    discardPile.clear();
    shufflePile(drawPile);
}

void BattleView::shufflePile(std::vector<CardData> &pile)
{
    for (int i = static_cast<int>(pile.size()) - 1; i > 0; --i) {
        const int j = QRandomGenerator::global()->bounded(i + 1);
        std::swap(pile[i], pile[j]);
    }
}

void BattleView::refreshLabels()
{
    const RunData &run = GameManager::instance()->runData();
    const EnemyActionData action = enemy.actions.empty()
                                       ? EnemyActionData{EnemyIntent::Attack, enemy.baseAttack, 0}
                                       : enemy.actions[enemyIntentIndex % enemy.actions.size()];
    QString intentText;
    if (action.intent == EnemyIntent::Attack) {
        intentText = GameText::EnemyText::attackIntentFormat().arg(enemyAttackDamage(action.amount));
    } else if (action.intent == EnemyIntent::Defend) {
        intentText = GameText::EnemyText::blockIntentFormat().arg(action.amount);
    } else if (action.intent == EnemyIntent::Buff) {
        intentText = GameText::EnemyText::buffIntentFormat().arg(action.amount);
    } else if (action.intent == EnemyIntent::Heal) {
        intentText = GameText::EnemyText::healIntentFormat().arg(action.amount);
    } else if (action.intent == EnemyIntent::AttackAndBuff) {
        intentText = GameText::EnemyText::attackAndBuffIntentFormat()
                         .arg(enemyAttackDamage(action.amount))
                         .arg(action.extra);
    } else if (action.intent == EnemyIntent::AttackAndBlock) {
        intentText = GameText::EnemyText::attackAndBlockIntentFormat()
                         .arg(enemyAttackDamage(action.amount))
                         .arg(action.extra);
    } else {
        intentText = EnemyCatalog::intentName(action.intent);
    }

    titleLabel->setText(QString::fromUtf8(u8"战斗：%1").arg(enemy.name));
    playerHealthBar->setRange(0, qMax(1, run.maxHp));
    playerHealthBar->setValue(qMax(0, run.currentHp));
    playerHealthBar->setFormat(QStringLiteral("%1/%2").arg(run.currentHp).arg(run.maxHp));
    enemyHealthBar->setRange(0, qMax(1, enemy.maxHp));
    enemyHealthBar->setValue(qMax(0, enemyHp));
    enemyHealthBar->setFormat(QStringLiteral("%1/%2").arg(enemyHp).arg(enemy.maxHp));
    playerBlockBadge->setVisible(block > 0);
    playerBlockBadge->setText(QString::number(block));
    enemyBlockBadge->setVisible(enemyBlock > 0);
    enemyBlockBadge->setText(QString::number(enemyBlock));

    QList<QPair<QString, int>> playerStatuses;
    if (playerStrength > 0) {
        playerStatuses.append({QString::fromUtf8(u8"力量"), playerStrength});
    }
    if (playerDexterity > 0) {
        playerStatuses.append({QString::fromUtf8(u8"敏捷"), playerDexterity});
    }
    if (playerWeak > 0) {
        playerStatuses.append({QString::fromUtf8(u8"虚弱"), playerWeak});
    }
    if (playerPoison > 0) {
        playerStatuses.append({QString::fromUtf8(u8"中毒"), playerPoison});
    }
    if (playerRegen > 0) {
        playerStatuses.append({QString::fromUtf8(u8"再生"), playerRegen});
    }
    QList<QPair<QString, int>> enemyStatuses;
    if (enemyStrength > 0) {
        enemyStatuses.append({QString::fromUtf8(u8"力量"), enemyStrength});
    }
    if (enemyWeak > 0) {
        enemyStatuses.append({QString::fromUtf8(u8"虚弱"), enemyWeak});
    }
    if (enemyVulnerable > 0) {
        enemyStatuses.append({QString::fromUtf8(u8"易伤"), enemyVulnerable});
    }
    if (enemyPoison > 0) {
        enemyStatuses.append({QString::fromUtf8(u8"中毒"), enemyPoison});
    }
    rebuildStatusIcons(playerStatusLayout, playerStatuses, true);
    rebuildStatusIcons(enemyStatusLayout, enemyStatuses, false);
    const QPixmap enemyPixmap(enemy.imagePath.isEmpty() ? GameText::Assets::enemyFallbackPortrait() : enemy.imagePath);
    if (!enemyPixmap.isNull()) {
        enemyPortraitLabel->setPixmap(enemyPixmap.scaled(190, 190, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        enemyPortraitLabel->setText(QString::fromUtf8(u8"敌人图片\n%1").arg(enemy.imagePath));
    }

    const QPixmap playerPixmap(GameText::Assets::playerBattlePortrait());
    if (!playerPixmap.isNull()) {
        playerPortraitLabel->setPixmap(playerPixmap.scaled(180, 180, Qt::KeepAspectRatio, Qt::SmoothTransformation));
        playerPortraitLabel->setToolTip(QString::fromUtf8(u8"角色图片位置：resources/pictures/player.png，加入 resource.qrc 后修改 GameText::Assets::playerBattlePortrait()"));
    } else {
        playerPortraitLabel->setText(QString::fromUtf8(u8"%1\nHP：%2/%3\n格挡：%4")
                                 .arg(run.characterName)
                                 .arg(run.currentHp)
                                 .arg(run.maxHp)
                                 .arg(block));
    }

    enemyLabel->setText(QString::fromUtf8(u8"%1\n意图：%2")
                            .arg(enemy.name)
                            .arg(intentText));
    playerLabel->setText(QString::fromUtf8(u8"%1  能量：%2/%3")
                             .arg(run.characterName)
                             .arg(energy)
                             .arg(maxEnergy));
}

void BattleView::rebuildHandButtons()
{
    for (QPushButton *button : handButtons) {
        if (button) {
            button->deleteLater();
        }
    }
    handButtons.clear();

    for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
        const CardData card = hand[i];
        const int insertedStart = qMax(0, static_cast<int>(hand.size()) - pendingInsertedCards);
        const bool isNewlyInserted = pendingInsertedCards > 0 && i >= insertedStart;
        const int insertOrder = isNewlyInserted ? (i - insertedStart) : 0;
        BattleCardButton *button = new BattleCardButton(QString::fromUtf8(u8"%1\n\n%2\n\n────────\n%3")
                                                            .arg(card.cost)
                                                            .arg(card.name)
                                                            .arg(card.description), handPanel);
        button->setFixedSize(132, 170);
        button->setCursor(Qt::PointingHandCursor);
        button->setMouseTracking(true);
        button->setProperty("cardIndex", i);
        button->setProperty("newCard", isNewlyInserted);
        button->setProperty("insertDelay", insertOrder * 85);
        button->installEventFilter(this);
        button->setEnabled(true);
        button->setToolTip(card.description);
        const QString cardStyle = QStringLiteral(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "                              stop:0 #305c83, stop:0.18 #f2d38b, stop:0.55 #8b4c2c, stop:1 #f5dfad);"
            "  border: 3px solid %1;"
            "  border-radius: 14px;"
            "  color: #24170f;"
            "  font-size: 8px;"
            "  font-weight: 900;"
            "  text-align: center;"
            "  padding: 5px;"
            "}"
            "QPushButton:hover {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "                              stop:0 #fffaf0, stop:0.58 #ffd979, stop:1 #d28c37);"
            "  border-color: #ffd27a;"
            "}"
            "QPushButton:disabled {"
            "  background: #7b756a;"
            "  border-color: #4e463d;"
            "  color: #d0c4b2;"
            "}").arg(energy >= card.cost ? QStringLiteral("#6f4928") : QStringLiteral("#8b332c"));
        button->setStyleSheet(cardStyle);
        button->show();
        button->raise();
        handButtons.push_back(button);

        if (isNewlyInserted) {
            QGraphicsOpacityEffect *fadeEffect = new QGraphicsOpacityEffect(button);
            fadeEffect->setOpacity(0.0);
            button->setGraphicsEffect(fadeEffect);
            QPointer<QGraphicsOpacityEffect> effectGuard(fadeEffect);
            QTimer::singleShot(insertOrder * 85, button, [effectGuard, button]() {
                if (!effectGuard) {
                    return;
                }
                QPropertyAnimation *fadeIn = new QPropertyAnimation(effectGuard, "opacity", button);
                fadeIn->setDuration(150);
                fadeIn->setStartValue(0.0);
                fadeIn->setEndValue(1.0);
                fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
            });
        }
    }

    layoutHandButtons(handPanel->width() > 260);
    if (handPanel->width() <= 260) {
        QTimer::singleShot(0, this, [this]() {
            layoutHandButtons(true);
        });
    }
    pendingInsertedCards = 0;

}

void BattleView::layoutHandButtons(bool animate)
{
    const int count = static_cast<int>(handButtons.size());
    if (count == 0 || !handPanel) {
        return;
    }
    const int layoutCount = draggingCardEntity ? qMax(0, count - 1) : count;
    if (layoutCount == 0) {
        return;
    }

    const int panelWidth = qMax(qMax(900, width() - 36), handPanel->width());
    const int panelHeight = qMax(240, handPanel->height());
    const int cardWidth = 132;
    const int cardHeight = 170;
    const int spread = qMin(92, qMax(54, panelWidth / qMax(7, layoutCount + 4)));
    const int centerX = panelWidth / 2;
    const int baseY = panelHeight - cardHeight - 18;

    int visibleIndex = 0;
    for (int i = 0; i < count; ++i) {
        QPushButton *button = handButtons[i];
        if (!button) {
            continue;
        }
        if (draggingCardEntity && i == dragCardIndex) {
            continue;
        }

        const double offset = visibleIndex - (layoutCount - 1) / 2.0;
        ++visibleIndex;
        const bool hovered = (i == hoveredCardIndex && !draggingCardEntity);
        const int curveLift = static_cast<int>(offset * offset * 5.0);
        const QSize size = hovered
                               ? QSize(static_cast<int>(cardWidth * 1.2), static_cast<int>(cardHeight * 1.2))
                               : QSize(cardWidth, cardHeight);
        const int x = centerX + static_cast<int>(offset * spread) - size.width() / 2;
        const int y = baseY + curveLift - (hovered ? 34 : 0);
        const QRect target(QPoint(x, y), size);
        if (BattleCardButton *cardButton = dynamic_cast<BattleCardButton*>(button)) {
            const qreal angle = hovered ? 0.0 : qBound(-8.0, offset * 3.0, 8.0);
            cardButton->setPaintAngle(angle);
        }

        const bool isNewCard = button->property("newCard").toBool();
        const int insertDelay = button->property("insertDelay").toInt();
        if (animate && isNewCard) {
            button->setProperty("newCard", false);
            QPoint drawPileStart(centerX, panelHeight + 16);
            if (drawPileButton) {
                drawPileStart = handPanel->mapFromGlobal(drawPileButton->mapToGlobal(QPoint(drawPileButton->width() / 2, drawPileButton->height() / 2)));
            }
            QRect startRect(drawPileStart - QPoint(cardWidth / 2, cardHeight / 2), QSize(cardWidth, cardHeight));
            button->setGeometry(startRect);
            QPointer<QPushButton> buttonGuard(button);
            QTimer::singleShot(insertDelay, button, [buttonGuard, startRect, target]() {
                if (!buttonGuard) {
                    return;
                }
                QPropertyAnimation *move = new QPropertyAnimation(buttonGuard, "geometry", buttonGuard);
                move->setDuration(210);
                move->setStartValue(startRect);
                move->setEndValue(target);
                move->start(QAbstractAnimation::DeleteWhenStopped);
            });
        } else if (animate) {
            const QRect startRect = button->geometry().isValid() ? button->geometry() : target;
            QPropertyAnimation *move = new QPropertyAnimation(button, "geometry", button);
            move->setDuration(130);
            move->setStartValue(startRect);
            move->setEndValue(target);
            move->start(QAbstractAnimation::DeleteWhenStopped);
        } else {
            button->setGeometry(target);
        }

        if (!hovered) {
            button->raise();
        }
    }

    if (hoveredCardIndex >= 0 && hoveredCardIndex < static_cast<int>(handButtons.size()) && handButtons[hoveredCardIndex]) {
        handButtons[hoveredCardIndex]->raise();
    }
}

void BattleView::rebuildStatusIcons(QHBoxLayout *layout, const QList<QPair<QString, int>> &statuses, bool playerSide)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    const QHash<QString, QString> descriptions = {
        {QString::fromUtf8(u8"力量"), QString::fromUtf8(u8"攻击造成的伤害提高。")},
        {QString::fromUtf8(u8"敏捷"), QString::fromUtf8(u8"获得格挡时提高格挡值。")},
        {QString::fromUtf8(u8"虚弱"), QString::fromUtf8(u8"攻击伤害降低 25%，回合结束后层数减少。")},
        {QString::fromUtf8(u8"易伤"), QString::fromUtf8(u8"受到攻击伤害提高 50%，回合结束后层数减少。")},
        {QString::fromUtf8(u8"中毒"), QString::fromUtf8(u8"回合开始时受到毒层数的伤害，然后层数减少。")},
        {QString::fromUtf8(u8"再生"), QString::fromUtf8(u8"回合开始时回复生命，然后层数减少。")}
    };

    const QList<QColor> colors = playerSide
                                     ? QList<QColor>{QColor(88, 154, 215), QColor(92, 184, 122), QColor(150, 108, 210)}
                                     : QList<QColor>{QColor(198, 78, 67), QColor(172, 117, 210), QColor(214, 161, 66)};
    for (int i = 0; i < statuses.size(); ++i) {
        const QString name = statuses[i].first;
        const int amount = statuses[i].second;
        QPushButton *icon = new QPushButton(QStringLiteral("%1\n%2").arg(name.left(1)).arg(amount), layout->parentWidget());
        icon->setFixedSize(34, 34);
        icon->setCursor(Qt::PointingHandCursor);
        icon->setToolTip(QStringLiteral("%1 x%2").arg(name).arg(amount));
        const QColor color = colors[i % colors.size()];
        icon->setStyleSheet(QStringLiteral(
            "QPushButton { background: %1; border: 2px solid rgba(255,255,255,175);"
            "border-radius: 17px; color: white; font-size: 11px; font-weight: 900; padding: 0px; }"
            "QPushButton:hover { border-color: #fff1b0; }").arg(color.name()));
        connect(icon, &QPushButton::clicked, this, [this, name, amount, descriptions]() {
            showStatusDialog(name, QStringLiteral("%1\n层数：%2").arg(descriptions.value(name), QString::number(amount)));
        });
        layout->addWidget(icon);
    }
}

void BattleView::showStatusDialog(const QString &name, const QString &description)
{
    QMessageBox *box = new QMessageBox(QMessageBox::Information, name, description, QMessageBox::Ok, this);
    box->setAttribute(Qt::WA_DeleteOnClose);
    box->show();
}

void BattleView::showDamagePopup(QWidget *anchor, int damage)
{
    if (!anchor) {
        return;
    }

    QLabel *popup = new QLabel(QStringLiteral("-%1").arg(damage), this);
    popup->setAlignment(Qt::AlignCenter);
    popup->setStyleSheet(
        "QLabel { color: #ff3f33; background: rgba(25, 5, 5, 95); border-radius: 12px;"
        "font-size: 30px; font-weight: 900; padding: 3px 10px; }");
    popup->adjustSize();
    const QPoint start = mapFromGlobal(anchor->mapToGlobal(QPoint(anchor->width() / 2, anchor->height() / 2)));
    popup->move(start + QPoint(24, -12));
    popup->show();
    popup->raise();

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(popup);
    popup->setGraphicsEffect(effect);

    QPropertyAnimation *fade = new QPropertyAnimation(effect, "opacity", popup);
    fade->setDuration(850);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, popup, &QLabel::deleteLater);
    fade->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *move = new QPropertyAnimation(popup, "pos", popup);
    move->setDuration(850);
    move->setStartValue(popup->pos());
    move->setEndValue(popup->pos() + QPoint(0, -54));
    move->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattleView::refreshPileButtons()
{
    drawPileButton->setText(GameText::Battle::drawPileButton().arg(drawPile.size()));
    discardPileButton->setText(GameText::Battle::discardPileButton().arg(discardPile.size()));
    exhaustPileButton->setText(GameText::Battle::exhaustPileButton().arg(exhaustPile.size()));
}

void BattleView::showPileDialog(PileKind pileKind)
{
    std::vector<CardData> cards;
    QString title;
    if (pileKind == PileKind::Draw) {
        cards = drawPile;
        title = QString::fromUtf8(u8"抽牌堆");
        std::sort(cards.begin(), cards.end(), [](const CardData &a, const CardData &b) {
            return a.name < b.name;
        });
    } else if (pileKind == PileKind::Discard) {
        cards = discardPile;
        title = QString::fromUtf8(u8"弃牌堆");
    } else {
        cards = exhaustPile;
        title = QString::fromUtf8(u8"消耗堆");
    }

    QDialog *dialog = new QDialog(this);
    dialog->setWindowTitle(title);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->resize(520, 420);

    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);
    QStringList lines;
    if (cards.empty()) {
        lines << GameText::Battle::emptyPileText();
    } else {
        for (const CardData &card : cards) {
            lines << cardLine(card);
        }
    }
    textEdit->setText(lines.join(QStringLiteral("\n\n")));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->addWidget(textEdit);
    dialog->show();
}

QString BattleView::cardLine(const CardData &card) const
{
    QStringList flags;
    if (card.exhaust) {
        flags << QString::fromUtf8(u8"消耗");
    }
    if (card.retain) {
        flags << QString::fromUtf8(u8"保留");
    }
    if (card.ethereal) {
        flags << QString::fromUtf8(u8"虚无");
    }

    const QString suffix = flags.isEmpty() ? QString() : QStringLiteral(" [%1]").arg(flags.join(QStringLiteral(", ")));
    return QStringLiteral("%1%2\n费用：%3\n%4").arg(card.name, suffix).arg(card.cost).arg(card.description);
}

void BattleView::playDraggedCardIfValid(int handIndex)
{
    if (handIndex < 0 || handIndex >= static_cast<int>(hand.size())) {
        return;
    }

    const CardData &card = hand[handIndex];
    const bool targetsEnemy = card.damage > 0 || card.enemyWeak > 0 || card.enemyVulnerable > 0;
    if (!targetsEnemy) {
        playCard(handIndex);
        return;
    }

    playCard(handIndex);
}

void BattleView::animateCardToPile(int handIndex, PileKind pileKind)
{
    if (handIndex < 0 || handIndex >= static_cast<int>(handButtons.size())) {
        return;
    }

    QPushButton *sourceButton = handButtons[handIndex];
    if (!sourceButton) {
        return;
    }

    QPushButton *targetButton = discardPileButton;
    if (pileKind == PileKind::Draw) {
        targetButton = drawPileButton;
    } else if (pileKind == PileKind::Exhaust) {
        targetButton = exhaustPileButton;
    }

    const QRect sourceRect(mapFromGlobal(sourceButton->mapToGlobal(QPoint(0, 0))), sourceButton->size());
    const QPoint targetCenter = mapFromGlobal(targetButton->mapToGlobal(QPoint(targetButton->width() / 2, targetButton->height() / 2)));
    const QSize flySize(84, 110);
    const QRect targetRect(targetCenter - QPoint(flySize.width() / 2, flySize.height() / 2), flySize);

    QLabel *flyingCard = new QLabel(sourceButton->text(), this);
    flyingCard->setAlignment(Qt::AlignCenter);
    flyingCard->setWordWrap(true);
    flyingCard->setStyleSheet(
        "QLabel { background: rgba(255, 247, 223, 230); border: 2px solid #ffd27a;"
        "border-radius: 10px; color: #2b2118; font-size: 10px; font-weight: 900; padding: 6px; }");
    flyingCard->setGeometry(sourceRect);
    flyingCard->show();
    flyingCard->raise();

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(flyingCard);
    flyingCard->setGraphicsEffect(effect);

    QPropertyAnimation *move = new QPropertyAnimation(flyingCard, "geometry", flyingCard);
    move->setDuration(260);
    move->setStartValue(sourceRect);
    move->setEndValue(targetRect);
    move->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *fade = new QPropertyAnimation(effect, "opacity", flyingCard);
    fade->setDuration(260);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, flyingCard, &QLabel::deleteLater);
    fade->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattleView::playCard(int handIndex)
{
    if (handIndex < 0 || handIndex >= static_cast<int>(hand.size())) {
        return;
    }

    const CardData card = hand[handIndex];
    if (energy < card.cost) {
        logLabel->setText(QString::fromUtf8(u8"能量不足，无法使用 %1。").arg(card.name));
        return;
    }

    energy -= card.cost;
    QStringList effects;

    for (int hit = 0; hit < qMax(1, card.hitCount); ++hit) {
        if (enemyHp <= 0) {
            break;
        }

        if (card.damage > 0) {
            int damage = playerAttackDamage(card.damage + playerStrength * (qMax(1, card.strengthMultiplier) - 1));
            if (enemyVulnerable > 0) {
                damage = damage + damage / 2;
            }
            const int absorbed = qMin(enemyBlock, damage);
            enemyBlock -= absorbed;
            damage -= absorbed;
            enemyHp = qMax(0, enemyHp - damage);
            showDamagePopup(enemyPortraitLabel, damage);
            effects << QString::fromUtf8(u8"第 %1 击造成 %2 点伤害").arg(hit + 1).arg(damage);
        }

        if (enemyHp <= 0) {
            finishVictory();
            break;
        }
    }

    if (card.block > 0) {
        const int gainedBlock = card.block + playerDexterity;
        block += qMax(0, gainedBlock);
        effects << QString::fromUtf8(u8"获得 %1 点格挡").arg(qMax(0, gainedBlock));
    }
    if (card.energyGain > 0) {
        energy += card.energyGain;
        effects << QString::fromUtf8(u8"获得 %1 点能量").arg(card.energyGain);
    }
    if (card.heal > 0) {
        GameManager::instance()->heal(card.heal);
        effects << QString::fromUtf8(u8"回复 %1 点生命").arg(card.heal);
    }
    if (card.selfDamage > 0) {
        GameManager::instance()->takeDamage(card.selfDamage);
        showDamagePopup(playerPortraitLabel, card.selfDamage);
        effects << QString::fromUtf8(u8"受到 %1 点自伤").arg(card.selfDamage);
    }
    if (card.strengthGain > 0) {
        playerStrength += card.strengthGain;
        effects << QString::fromUtf8(u8"获得 %1 点力量").arg(card.strengthGain);
    }
    if (card.enemyWeak > 0 && enemyHp > 0) {
        enemyWeak += card.enemyWeak;
        effects << QString::fromUtf8(u8"给予 %1 层虚弱").arg(card.enemyWeak);
    }
    if (card.enemyVulnerable > 0 && enemyHp > 0) {
        enemyVulnerable += card.enemyVulnerable;
        effects << QString::fromUtf8(u8"给予 %1 层易伤").arg(card.enemyVulnerable);
    }
    if (card.draw > 0) {
        drawCards(card.draw);
        effects << QString::fromUtf8(u8"抽 %1 张牌").arg(card.draw);
    }

    if (card.exhaust) {
        animateCardToPile(handIndex, PileKind::Exhaust);
        exhaustPile.push_back(card);
    } else {
        animateCardToPile(handIndex, PileKind::Discard);
        discardPile.push_back(card);
    }
    hand.erase(hand.begin() + handIndex);

    if (enemyHp > 0) {
        logLabel->setText(QString::fromUtf8(u8"使用 %1：%2。").arg(card.name, effects.join(QString::fromUtf8(u8"，"))));
    }

    rebuildHandButtons();
    refreshLabels();
    refreshPileButtons();
}

void BattleView::endTurn()
{
    discardHandAtEndTurn();
    triggerPlayerEndTurnEffects();

    // Step 4: energy is cleared. Ice Cream can hook here later.
    energy = 0;
    rebuildHandButtons();
    refreshPileButtons();

    runEnemyTurn();
    refreshLabels();
    if (GameManager::instance()->runData().currentHp <= 0) {
        endTurnButton->setEnabled(false);
        logLabel->setText(QString::fromUtf8(u8"你被击败了。"));
        return;
    }

    reduceEndOfRoundStatus();
    startPlayerTurn(false);
}

void BattleView::finishVictory()
{
    endTurnButton->setEnabled(false);
    GameManager::instance()->finishBattleVictory();
    logLabel->setText(QString::fromUtf8(u8"战斗胜利！正在进入奖励界面。"));
    refreshLabels();
    refreshPileButtons();
    QTimer::singleShot(650, this, &BattleView::battleFinished);
}

void BattleView::hidePotionConfirm()
{
    potionConfirmPanel->hide();
}

void BattleView::useSelectedPotionOnEnemy()
{
    if (selectedPotionIndex < 0 || !selectedPotion.targetsEnemy) {
        return;
    }

    int damage = selectedPotion.damage;
    const int absorbed = qMin(enemyBlock, damage);
    enemyBlock -= absorbed;
    damage -= absorbed;
    enemyHp = qMax(0, enemyHp - damage);
    showDamagePopup(enemyPortraitLabel, damage);
    GameManager::instance()->removePotionAt(selectedPotionIndex);
    logLabel->setText(QString::fromUtf8(u8"使用 %1，对 %2 造成 %3 点伤害。")
                          .arg(selectedPotion.name, enemy.name)
                          .arg(damage));
    selectedPotionIndex = -1;
    refreshLabels();

    if (enemyHp <= 0) {
        finishVictory();
    }
}

void BattleView::useSelectedPotionOnPlayer()
{
    if (selectedPotionIndex < 0 || selectedPotion.targetsEnemy) {
        return;
    }

    if (selectedPotion.block > 0) {
        block += selectedPotion.block;
    }
    if (selectedPotion.heal > 0) {
        GameManager::instance()->heal(selectedPotion.heal);
    }
    GameManager::instance()->removePotionAt(selectedPotionIndex);
    logLabel->setText(QString::fromUtf8(u8"使用 %1。").arg(selectedPotion.name));
    selectedPotionIndex = -1;
    hidePotionConfirm();
    refreshLabels();
}

int BattleView::playerAttackDamage(int baseDamage) const
{
    int damage = qMax(0, baseDamage + playerStrength);
    if (playerWeak > 0) {
        damage = damage * 3 / 4;
    }
    return qMax(0, damage);
}

int BattleView::enemyAttackDamage(int baseDamage) const
{
    const int base = baseDamage >= 0 ? baseDamage : enemy.baseAttack;
    int damage = qMax(0, base + enemyStrength);
    if (enemyWeak > 0) {
        damage = damage * 3 / 4;
    }
    return qMax(0, damage);
}
