#include "BattleView.h"

#include "core/GameManager.h"

#include <QDialog>
#include <QEvent>
#include <QHBoxLayout>
#include <QLabel>
#include <QMouseEvent>
#include <QPushButton>
#include <QRandomGenerator>
#include <QScrollArea>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>
#include <algorithm>

BattleView::BattleView(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , enemyLabel(new QLabel(this))
    , playerLabel(new QLabel(this))
    , logLabel(new QLabel(this))
    , potionConfirmPanel(new QWidget(this))
    , potionConfirmLabel(new QLabel(potionConfirmPanel))
    , pileButtonLayout(new QHBoxLayout)
    , handLayout(new QHBoxLayout)
    , endTurnButton(new QPushButton(QString::fromUtf8(u8"结束回合"), this))
    , drawPileButton(new QPushButton(QString::fromUtf8(u8"抽牌堆"), this))
    , discardPileButton(new QPushButton(QString::fromUtf8(u8"弃牌堆"), this))
    , exhaustPileButton(new QPushButton(QString::fromUtf8(u8"消耗堆"), this))
{
    setStyleSheet(
        "BattleView { background: #20171b; }"
        "QLabel { color: #f5ead2; font-size: 18px; }"
        "QPushButton { color: #f8edd4; background: #6e4b4b; border: 1px solid rgba(255,255,255,90);"
        "              border-radius: 4px; padding: 10px 16px; font-size: 15px; font-weight: 700; }"
        "QPushButton:hover { background: #875b5b; }"
        "QPushButton:disabled { background: #444; color: #999; }");

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: 800;");
    enemyLabel->setAlignment(Qt::AlignCenter);
    enemyLabel->setStyleSheet("QLabel { border: 1px solid rgba(255,255,255,70); padding: 12px; border-radius: 6px; }");
    enemyLabel->installEventFilter(this);
    playerLabel->setAlignment(Qt::AlignCenter);
    logLabel->setAlignment(Qt::AlignCenter);
    logLabel->setWordWrap(true);

    QHBoxLayout *potionLayout = new QHBoxLayout(potionConfirmPanel);
    potionLayout->setContentsMargins(12, 8, 12, 8);
    potionLayout->setSpacing(8);
    QPushButton *confirmPotionButton = new QPushButton(QString::fromUtf8(u8"确认使用"), potionConfirmPanel);
    QPushButton *cancelPotionButton = new QPushButton(QString::fromUtf8(u8"取消"), potionConfirmPanel);
    potionLayout->addWidget(potionConfirmLabel, 1);
    potionLayout->addWidget(confirmPotionButton);
    potionLayout->addWidget(cancelPotionButton);
    potionConfirmPanel->setStyleSheet("QWidget { background: rgba(30, 36, 44, 230); border-radius: 6px; }");
    potionConfirmPanel->hide();
    connect(confirmPotionButton, &QPushButton::clicked, this, &BattleView::useSelectedPotionOnPlayer);
    connect(cancelPotionButton, &QPushButton::clicked, this, &BattleView::hidePotionConfirm);

    pileButtonLayout->setSpacing(8);
    pileButtonLayout->addStretch();
    pileButtonLayout->addWidget(drawPileButton);
    pileButtonLayout->addWidget(discardPileButton);
    pileButtonLayout->addWidget(exhaustPileButton);
    pileButtonLayout->addStretch();
    connect(drawPileButton, &QPushButton::clicked, this, [this]() { showPileDialog(PileKind::Draw); });
    connect(discardPileButton, &QPushButton::clicked, this, [this]() { showPileDialog(PileKind::Discard); });
    connect(exhaustPileButton, &QPushButton::clicked, this, [this]() { showPileDialog(PileKind::Exhaust); });

    handLayout->setSpacing(10);
    connect(endTurnButton, &QPushButton::clicked, this, &BattleView::endTurn);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(60, 50, 60, 50);
    layout->setSpacing(16);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(enemyLabel);
    layout->addWidget(playerLabel);
    layout->addWidget(logLabel);
    layout->addWidget(potionConfirmPanel, 0, Qt::AlignHCenter);
    layout->addLayout(pileButtonLayout);
    layout->addLayout(handLayout);
    layout->addWidget(endTurnButton, 0, Qt::AlignHCenter);
    layout->addStretch();
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
    enemyPoison = 0;
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
    if (watched == enemyLabel && event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent*>(event);
        if (mouseEvent->button() == Qt::LeftButton && selectedPotionIndex >= 0 && selectedPotion.targetsEnemy) {
            useSelectedPotionOnEnemy();
            return true;
        }
    }
    return QWidget::eventFilter(watched, event);
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

    // Single-enemy version of the left-to-right enemy phase. Multi-enemy can iterate the same action block.
    const int damage = qMax(0, enemyAttackDamage() - block);
    block = 0;
    if (damage > 0) {
        GameManager::instance()->takeDamage(damage);
    }

    logLabel->setText(QString::fromUtf8(u8"%1 按顺序行动：攻击，造成 %2 点伤害。").arg(enemy.name).arg(damage));
}

void BattleView::reduceEndOfRoundStatus()
{
    if (playerWeak > 0) {
        --playerWeak;
    }
    if (enemyWeak > 0) {
        --enemyWeak;
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
    titleLabel->setText(QString::fromUtf8(u8"战斗：%1").arg(enemy.name));
    enemyLabel->setText(QString::fromUtf8(u8"%1 HP：%2/%3，格挡：%4\n意图：攻击 %5\n状态：力量 %6，虚弱 %7，毒 %8")
                            .arg(enemy.name)
                            .arg(enemyHp)
                            .arg(enemy.maxHp)
                            .arg(enemyBlock)
                            .arg(enemyAttackDamage())
                            .arg(enemyStrength)
                            .arg(enemyWeak)
                            .arg(enemyPoison));
    playerLabel->setText(QString::fromUtf8(u8"%1 HP：%2/%3，格挡：%4，能量：%5/%6\n状态：力量 %7，敏捷 %8，虚弱 %9，毒 %10，再生 %11")
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
    while (QLayoutItem *item = handLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for (int i = 0; i < static_cast<int>(hand.size()); ++i) {
        const CardData card = hand[i];
        QPushButton *button = new QPushButton(QStringLiteral("%1\n%2").arg(card.name, card.description), this);
        button->setMinimumSize(150, 100);
        button->setEnabled(energy >= card.cost);
        button->setToolTip(card.description);
        connect(button, &QPushButton::clicked, this, [this, i]() {
            playCard(i);
        });
        handLayout->addWidget(button);
    }
}

void BattleView::refreshPileButtons()
{
    drawPileButton->setText(QString::fromUtf8(u8"抽牌堆 %1").arg(drawPile.size()));
    discardPileButton->setText(QString::fromUtf8(u8"弃牌堆 %1").arg(discardPile.size()));
    exhaustPileButton->setText(QString::fromUtf8(u8"消耗堆 %1").arg(exhaustPile.size()));
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
        lines << QString::fromUtf8(u8"这里暂时没有牌。");
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
            int damage = playerAttackDamage(card.damage);
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
    if (card.strengthGain > 0) {
        playerStrength += card.strengthGain;
        effects << QString::fromUtf8(u8"获得 %1 点力量").arg(card.strengthGain);
    }
    if (card.enemyWeak > 0 && enemyHp > 0) {
        enemyWeak += card.enemyWeak;
        effects << QString::fromUtf8(u8"给予 %1 层虚弱").arg(card.enemyWeak);
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

int BattleView::enemyAttackDamage() const
{
    int damage = qMax(0, enemy.baseAttack + enemyStrength);
    if (enemyWeak > 0) {
        damage = damage * 3 / 4;
    }
    return qMax(0, damage);
}
