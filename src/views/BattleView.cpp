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
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QMimeData>
#include <QPropertyAnimation>
#include <QPixmap>
#include <QPushButton>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QScrollArea>
#include <QSizePolicy>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

BattleView::BattleView(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , playerPortraitLabel(new QLabel(this))
    , enemyPortraitLabel(new QLabel(this))
    , enemyLabel(new QLabel(this))
    , playerLabel(new QLabel(this))
    , logLabel(new QLabel(this))
    , potionConfirmPanel(new QWidget(this))
    , potionConfirmLabel(new QLabel(potionConfirmPanel))
    , arenaPanel(new QFrame(this))
    , handPanel(new QWidget(this))
    , pileButtonLayout(new QHBoxLayout)
    , endTurnButton(new QPushButton(GameText::Battle::endTurnButton(), this))
    , confirmCardButton(new QPushButton(QString::fromUtf8(u8"确认出牌"), this))
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

    confirmCardButton->setEnabled(false);
    connect(confirmCardButton, &QPushButton::clicked, this, &BattleView::confirmSelectedCard);
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
    confirmCardButton->setFixedSize(92, 34);
    endTurnButton->setFixedSize(92, 34);
    drawPileButton->setFixedHeight(30);
    discardPileButton->setFixedHeight(30);
    exhaustPileButton->setFixedHeight(30);
    controlLayout->addWidget(playerLabel, 1);
    controlLayout->addStretch();
    controlLayout->addWidget(confirmCardButton);
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
    selectedCardIndex = -1;
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
            if (event->type() == QEvent::MouseButtonPress) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if (mouseEvent->button() == Qt::LeftButton) {
                    dragCardIndex = handIndex;
                    dragStartPos = mouseEvent->globalPosition().toPoint();
                }
            } else if (event->type() == QEvent::MouseMove && dragCardIndex == handIndex) {
                QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
                if ((mouseEvent->globalPosition().toPoint() - dragStartPos).manhattanLength() >= QApplication::startDragDistance()) {
                    if (!dragPreviewLabel) {
                        dragPreviewLabel = new QLabel(this);
                        dragPreviewLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
                        dragPreviewLabel->setStyleSheet(
                            "QLabel { background: rgba(255, 247, 223, 220); border: 2px solid #ffd27a;"
                            "border-radius: 12px; color: #2b2118; font-size: 13px; font-weight: 800; padding: 8px; }");
                        dragPreviewLabel->resize(132, 96);
                        dragPreviewLabel->show();
                        dragPreviewLabel->raise();
                    }
                    if (handIndex >= 0 && handIndex < static_cast<int>(hand.size())) {
                        dragPreviewLabel->setText(hand[handIndex].name);
                    }
                    dragPreviewLabel->move(mapFromGlobal(mouseEvent->globalPosition().toPoint()) + QPoint(12, 12));
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
                if (dragPreviewLabel) {
                    dragPreviewLabel->deleteLater();
                    dragPreviewLabel = nullptr;
                }
                dragCardIndex = -1;
                if (droppedOnEnemy) {
                    playDraggedCardIfValid(handIndex);
                    return true;
                }
                if (droppedOnBattleArea && handIndex >= 0 && handIndex < static_cast<int>(hand.size())) {
                    const CardData &card = hand[handIndex];
                    const bool targetsEnemy = card.damage > 0 || card.enemyWeak > 0 || card.enemyVulnerable > 0;
                    if (!targetsEnemy) {
                        selectedCardIndex = -1;
                        confirmCardButton->setEnabled(false);
                        playCard(handIndex);
                        return true;
                    }
                }
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
    for (const CardData &card : hand) {
        if (card.ethereal) {
            exhaustPile.push_back(card);
        } else if (card.retain) {
            retainedCards.push_back(card);
        } else {
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

    enemyLabel->setText(QString::fromUtf8(u8"%1\nHP：%2/%3  格挡：%4\n意图：%5\n力量 %6  虚弱 %7  易伤 %8  毒 %9")
                            .arg(enemy.name)
                            .arg(enemyHp)
                            .arg(enemy.maxHp)
                            .arg(enemyBlock)
                            .arg(intentText)
                            .arg(enemyStrength)
                            .arg(enemyWeak)
                            .arg(enemyVulnerable)
                            .arg(enemyPoison));
    playerLabel->setText(QString::fromUtf8(u8"%1  HP：%2/%3  格挡：%4  能量：%5/%6  力量：%7  敏捷：%8  虚弱：%9  毒：%10  再生：%11")
                             .arg(run.characterName)
                             .arg(run.currentHp)
                             .arg(run.maxHp)
                             .arg(block)
                             .arg(energy)
                             .arg(maxEnergy)
                             .arg(playerStrength)
                             .arg(playerDexterity)
                             .arg(playerWeak)
                             .arg(playerPoison)
                             .arg(playerRegen));
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
        QPushButton *button = new QPushButton(GameText::CardText::buttonTextFormat().arg(card.name).arg(card.cost).arg(card.description), handPanel);
        button->setFixedSize(138, 178);
        button->setCursor(Qt::PointingHandCursor);
        button->setProperty("cardIndex", i);
        button->installEventFilter(this);
        const bool selected = (i == selectedCardIndex);
        button->setEnabled(energy >= card.cost);
        button->setToolTip(card.description);
        const QString cardStyle = QStringLiteral(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "                              stop:0 #fff7df, stop:0.62 #f0d18b, stop:1 #b87938);"
            "  border: 3px solid %1;"
            "  border-radius: 14px;"
            "  color: #2b2118;"
            "  font-size: 14px;"
            "  font-weight: 700;"
            "  padding: 10px;"
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
            "}").arg(selected ? QStringLiteral("#fff0b7") : QStringLiteral("#6f4928"));
        button->setStyleSheet(cardStyle);
        if (selected) {
            button->resize(148, 190);
        }
        connect(button, &QPushButton::clicked, this, [this, i]() {
            selectCard(i);
        });
        button->show();
        button->raise();
        handButtons.push_back(button);

        QGraphicsOpacityEffect *fadeEffect = new QGraphicsOpacityEffect(button);
        fadeEffect->setOpacity(0.0);
        button->setGraphicsEffect(fadeEffect);
        QPropertyAnimation *fadeIn = new QPropertyAnimation(fadeEffect, "opacity", button);
        fadeIn->setDuration(140);
        fadeIn->setStartValue(0.0);
        fadeIn->setEndValue(1.0);
        fadeIn->start(QAbstractAnimation::DeleteWhenStopped);
    }

    layoutHandButtons(true);

    confirmCardButton->setEnabled(selectedCardIndex >= 0
                                  && selectedCardIndex < static_cast<int>(hand.size())
                                  && energy >= hand[selectedCardIndex].cost);
}

void BattleView::layoutHandButtons(bool animate)
{
    const int count = static_cast<int>(handButtons.size());
    if (count == 0 || !handPanel) {
        return;
    }

    const int panelWidth = qMax(1, handPanel->width());
    const int panelHeight = qMax(1, handPanel->height());
    const int cardWidth = 138;
    const int cardHeight = 178;
    const int selectedLift = 26;
    const int spread = qMin(112, qMax(58, panelWidth / qMax(6, count + 2)));
    const int centerX = panelWidth / 2;
    const int baseY = panelHeight - cardHeight - 18;

    for (int i = 0; i < count; ++i) {
        QPushButton *button = handButtons[i];
        if (!button) {
            continue;
        }

        const double offset = i - (count - 1) / 2.0;
        const int curveLift = static_cast<int>(offset * offset * 5.0);
        const bool selected = (i == selectedCardIndex);
        const QSize size(selected ? QSize(148, 190) : QSize(cardWidth, cardHeight));
        const int x = centerX + static_cast<int>(offset * spread) - size.width() / 2;
        const int y = baseY + curveLift - (selected ? selectedLift : 0);
        const QRect target(QPoint(x, y), size);

        if (animate) {
            QRect startRect(centerX - cardWidth / 2, panelHeight + 16, cardWidth, cardHeight);
            if (button->geometry().isValid() && button->geometry().width() > 0) {
                startRect = button->geometry();
            }
            button->setGeometry(startRect);
            QPropertyAnimation *move = new QPropertyAnimation(button, "geometry", button);
            move->setDuration(190);
            move->setStartValue(startRect);
            move->setEndValue(target);
            move->start(QAbstractAnimation::DeleteWhenStopped);
        } else {
            button->setGeometry(target);
        }

        button->raise();
    }
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

void BattleView::selectCard(int handIndex)
{
    if (handIndex < 0 || handIndex >= static_cast<int>(hand.size())) {
        selectedCardIndex = -1;
        confirmCardButton->setEnabled(false);
        rebuildHandButtons();
        return;
    }

    if (energy < hand[handIndex].cost) {
        logLabel->setText(QString::fromUtf8(u8"能量不足，无法使用 %1。").arg(hand[handIndex].name));
        return;
    }

    selectedCardIndex = handIndex;
    confirmCardButton->setEnabled(true);
    rebuildHandButtons();
}

void BattleView::confirmSelectedCard()
{
    if (selectedCardIndex < 0 || selectedCardIndex >= static_cast<int>(hand.size())) {
        return;
    }

    const int index = selectedCardIndex;
    selectedCardIndex = -1;
    confirmCardButton->setEnabled(false);
    playCard(index);
}

void BattleView::playDraggedCardIfValid(int handIndex)
{
    if (handIndex < 0 || handIndex >= static_cast<int>(hand.size())) {
        return;
    }

    const CardData &card = hand[handIndex];
    const bool targetsEnemy = card.damage > 0 || card.enemyWeak > 0 || card.enemyVulnerable > 0;
    if (!targetsEnemy) {
        selectCard(handIndex);
        logLabel->setText(QString::fromUtf8(u8"%1 不是指向敌人的牌，请用上方“出牌”按钮确认。").arg(card.name));
        return;
    }

    selectedCardIndex = -1;
    confirmCardButton->setEnabled(false);
    playCard(handIndex);
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
        exhaustPile.push_back(card);
    } else {
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
