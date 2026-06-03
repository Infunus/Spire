#include "BattleWidget.h"
#include "../core/GameText.h"

#include <QAbstractAnimation>
#include <QPropertyAnimation>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPixmap>
#include <QProgressBar>
#include <QRandomGenerator>
#include <QResizeEvent>
#include <QVBoxLayout>

ArcHandWidget::ArcHandWidget(QWidget *parent)
    : QWidget(parent),
      m_selectedCardIndex(-1)
{
    setMinimumHeight(208);
    setAttribute(Qt::WA_StyledBackground, true);
}

void ArcHandWidget::setCards(const QStringList &cards)
{
    qDeleteAll(m_cardButtons);
    m_cardButtons.clear();
    m_selectedCardIndex = -1;

    for (int i = 0; i < cards.size(); ++i) {
        const QString &cardText = cards.at(i);
        QPushButton *button = new QPushButton(cardText, this);
        button->setMinimumSize(118, 150);
        button->setMaximumSize(158, 184);
        button->setCursor(Qt::PointingHandCursor);
        button->setStyleSheet(
            "QPushButton {"
            "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
            "                              stop:0 #fff7df, stop:0.62 #f0d18b, stop:1 #b87938);"
            "  border: 3px solid #6f4928;"
            "  border-radius: 14px;"
            "  color: #2b2118;"
            "  font-size: 14px;"
            "  font-weight: 700;"
            "  padding: 10px;"
            "  text-align: center;"
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
            "}");
        connect(button, &QPushButton::clicked, this, [this, i]() {
            emit cardClicked(i);
        });
        m_cardButtons.append(button);
    }

    updateCardLayout();
}

void ArcHandWidget::setCardEnabled(int cardIndex, bool enabled)
{
    if (cardIndex < 0 || cardIndex >= m_cardButtons.size()) {
        return;
    }

    m_cardButtons.at(cardIndex)->setEnabled(enabled);
}

void ArcHandWidget::setSelectedCard(int cardIndex)
{
    m_selectedCardIndex = cardIndex;
    updateCardLayout();
}

void ArcHandWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    updateCardLayout();
}

void ArcHandWidget::updateCardLayout()
{
    if (m_cardButtons.isEmpty()) {
        return;
    }

    const int count = m_cardButtons.size();
    const int cardWidth = qBound(92, width() / qMax(5, count + 1), 158);
    const int cardHeight = qBound(138, height() - 22, 184);
    const int centerX = width() / 2;
    const int baseY = height() - cardHeight - 2;
    int spacing = cardWidth + 14;
    if (count > 1) {
        spacing = qMin(spacing, qMax(cardWidth / 2, (width() - cardWidth) / (count - 1)));
    }

    for (int i = 0; i < count; ++i) {
        const double offset = i - (count - 1) / 2.0;
        const int x = centerX + static_cast<int>(offset * spacing) - cardWidth / 2;
        const int arcLift = static_cast<int>(32 - offset * offset * 5);
        const int selectedLift = (i == m_selectedCardIndex) ? 18 : 0;
        const int y = baseY - qMax(0, arcLift) - selectedLift;

        QPushButton *button = m_cardButtons.at(i);
        button->setGeometry(x, y, cardWidth, cardHeight);
        button->show();
        button->raise();
    }
}

PortraitWidget::PortraitWidget(const QString &name, QWidget *parent)
    : QWidget(parent),
      m_portraitLabel(new QLabel(this)),
      m_nameLabel(new QLabel(name, this)),
      m_hpLabel(new QLabel(this)),
      m_armorLabel(new QLabel(this))
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(204, 236);
    setStyleSheet(
        "PortraitWidget { background: transparent; }"
        "QLabel#PortraitLabel {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                              stop:0 #f9edc8, stop:0.62 #9f6a3e, stop:1 #3b2418);"
        "  border: 5px solid #d8a45f;"
        "  border-radius: 18px;"
        "  color: #2a1a0d;"
        "  font-size: 18px;"
        "  font-weight: 800;"
        "}"
        "QLabel#HeroNameLabel { color: #fff3d4; font-size: 15px; font-weight: 700; }"
        "QLabel#HpLabel {"
        "  background: #a82f22;"
        "  border: 2px solid #ffd5b8;"
        "  border-radius: 16px;"
        "  color: white;"
        "  font-size: 18px;"
        "  font-weight: 900;"
        "}"
        "QLabel#ArmorLabel {"
        "  background: #5d7180;"
        "  border: 2px solid #d9edf4;"
        "  border-radius: 16px;"
        "  color: white;"
        "  font-size: 16px;"
        "  font-weight: 900;"
        "}");

    m_portraitLabel->setObjectName("PortraitLabel");
    m_portraitLabel->setAlignment(Qt::AlignCenter);
    m_portraitLabel->setText(GameText::Battle::portraitPlaceholder());
    m_portraitLabel->setFixedSize(168, 168);

    m_nameLabel->setObjectName("HeroNameLabel");
    m_nameLabel->setAlignment(Qt::AlignCenter);

    m_hpLabel->setObjectName("HpLabel");
    m_hpLabel->setAlignment(Qt::AlignCenter);
    m_hpLabel->setFixedSize(40, 34);

    m_armorLabel->setObjectName("ArmorLabel");
    m_armorLabel->setAlignment(Qt::AlignCenter);
    m_armorLabel->setFixedSize(40, 34);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->setSpacing(4);
    layout->setAlignment(Qt::AlignCenter);

    QWidget *avatarStack = new QWidget(this);
    avatarStack->setFixedSize(192, 182);

    m_portraitLabel->setParent(avatarStack);
    m_portraitLabel->move(12, 0);
    m_armorLabel->setParent(avatarStack);
    m_armorLabel->move(0, 140);
    m_hpLabel->setParent(avatarStack);
    m_hpLabel->move(152, 140);

    layout->addWidget(avatarStack, 0, Qt::AlignCenter);
    layout->addWidget(m_nameLabel);

    setStats(30, 0);
}

void PortraitWidget::setPortraitImage(const QString &imagePath)
{
    const QPixmap portrait(imagePath);
    if (portrait.isNull()) {
        return;
    }

    m_portraitLabel->setText(QString());
    m_portraitLabel->setPixmap(portrait.scaled(m_portraitLabel->size(),
                                               Qt::KeepAspectRatioByExpanding,
                                               Qt::SmoothTransformation));
}

void PortraitWidget::setStats(int hp, int armor)
{
    m_hpLabel->setText(QString::number(hp));
    m_armorLabel->setText(QString::number(armor));
    m_armorLabel->setVisible(armor > 0);
}

EnemyCardWidget::EnemyCardWidget(const QString &name, int hp, QWidget *parent)
    : QWidget(parent),
      m_maxHp(hp),
      m_imageLabel(new QLabel(this)),
      m_nameLabel(new QLabel(name, this)),
      m_intentLabel(new QLabel(this)),
      m_hpBar(new QProgressBar(this))
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(166, 238);
    setStyleSheet(
        "EnemyCardWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
        "                              stop:0 #f5e7c5, stop:0.64 #c08a4d, stop:1 #6c3d27);"
        "  border: 3px solid #6b4528;"
        "  border-radius: 16px;"
        "}"
        "QLabel#EnemyImageLabel {"
        "  background: rgba(84, 51, 38, 135);"
        "  border: 2px solid rgba(255, 226, 168, 150);"
        "  border-radius: 10px;"
        "  color: #fff3d4;"
        "  font-size: 13px;"
        "  font-weight: 700;"
        "}"
        "QLabel#EnemyNameLabel { color: #2b2118; font-size: 14px; font-weight: 800; }"
        "QLabel#EnemyIntentLabel {"
        "  background: rgba(36, 24, 18, 165);"
        "  border: 1px solid rgba(255, 226, 168, 125);"
        "  border-radius: 8px;"
        "  color: #fff3d4;"
        "  font-size: 13px;"
        "  font-weight: 700;"
        "  padding: 2px 6px;"
        "}"
        "QProgressBar#EnemyHpBar {"
        "  background: #4a1b18;"
        "  border: 2px solid #ffd5b8;"
        "  border-radius: 8px;"
        "  color: white;"
        "  font-size: 14px;"
        "  font-weight: 900;"
        "  text-align: center;"
        "}"
        "QProgressBar#EnemyHpBar::chunk {"
        "  background: #b92d25;"
        "  border-radius: 6px;"
        "}");

    m_imageLabel->setObjectName("EnemyImageLabel");
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setText(GameText::Battle::enemyImagePlaceholder());
    m_imageLabel->setFixedSize(132, 88);

    m_nameLabel->setObjectName("EnemyNameLabel");
    m_nameLabel->setAlignment(Qt::AlignCenter);

    m_intentLabel->setObjectName("EnemyIntentLabel");
    m_intentLabel->setAlignment(Qt::AlignCenter);
    m_intentLabel->setFixedHeight(32);
    m_intentLabel->setWordWrap(true);

    m_hpBar->setObjectName("EnemyHpBar");
    m_hpBar->setRange(0, m_maxHp);
    m_hpBar->setFixedHeight(30);
    m_hpBar->setTextVisible(true);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 10);
    layout->setSpacing(6);
    layout->addWidget(m_imageLabel, 0, Qt::AlignCenter);
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_intentLabel);
    layout->addWidget(m_hpBar);
    setHp(hp);
    setIntent(GameText::Battle::pendingIntent());
}

void EnemyCardWidget::setEnemy(const QString &name, int maxHp)
{
    m_nameLabel->setText(name);
    m_maxHp = maxHp;
    m_hpBar->setRange(0, m_maxHp);
    m_imageLabel->clear();
    m_imageLabel->setText(name);
    setHp(maxHp);
}

void EnemyCardWidget::setCardImage(const QString &imagePath)
{
    const QPixmap cardImage(imagePath);
    if (cardImage.isNull()) {
        return;
    }

    m_imageLabel->setText(QString());
    m_imageLabel->setPixmap(cardImage.scaled(m_imageLabel->size(),
                                             Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation));
}

void EnemyCardWidget::setHp(int hp)
{
    m_hpBar->setValue(hp);
    m_hpBar->setFormat(GameText::Battle::hpFormat().arg(hp).arg(m_maxHp));
}

void EnemyCardWidget::setIntent(const QString &intent)
{
    m_intentLabel->setText(intent);
}

EnergyWidget::EnergyWidget(QWidget *parent)
    : QWidget(parent),
      m_layout(new QHBoxLayout(this))
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "EnergyWidget {"
        "  background: rgba(36, 24, 18, 120);"
        "  border: 1px solid rgba(185, 215, 255, 90);"
        "  border-radius: 14px;"
        "}"
        "QLabel {"
        "  background: #294f9f;"
        "  border: 2px solid #b9d7ff;"
        "  border-radius: 12px;"
        "  color: white;"
        "  font-size: 13px;"
        "  font-weight: 800;"
        "}");

    m_layout->setContentsMargins(8, 6, 8, 6);
    m_layout->setSpacing(6);
}

void EnergyWidget::setEnergy(int current, int maximum)
{
    while (QLayoutItem *item = m_layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
    m_energyPips.clear();

    for (int i = 0; i < maximum; ++i) {
        QLabel *pip = new QLabel(QString::number(i + 1), this);
        pip->setAlignment(Qt::AlignCenter);
        pip->setFixedSize(28, 28);
        if (i >= current) {
            pip->setStyleSheet(
                "background: #263142;"
                "border: 2px solid #617086;"
                "border-radius: 12px;"
                "color: #9da9ba;"
                "font-size: 13px;"
                "font-weight: 800;");
        }
        m_layout->addWidget(pip);
        m_energyPips.append(pip);
    }
}

BattleWidget::BattleWidget(QWidget *parent)
    : QWidget(parent),
      m_isBossBattle(false),
      m_arenaTitleLabel(nullptr),
      m_bossPortrait(nullptr),
      m_playerPortrait(nullptr),
      m_enemyCard(nullptr),
      m_enemyField(nullptr),
      m_energyWidget(nullptr),
      m_handWidget(nullptr),
      m_confirmCardButton(nullptr),
      m_endTurnButton(nullptr),
      m_enemy(Enemy::createGaoshuHomework()),
      m_playerHp(70),
      m_playerMaxHp(70),
      m_playerBlock(0),
      m_energy(3),
      m_maxEnergy(3),
      m_turnNumber(1),
      m_battleNumber(1),
      m_enemiesDefeated(0),
      m_selectedCardIndex(-1),
      m_battleEnded(false),
      m_playerTurn(true),
      m_runFinished(false)
{
    setObjectName("BattleWidget");
    setStyleSheet(
        "#BattleWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                              stop:0 #2a1114, stop:0.48 #7b1d26, stop:1 #24100d);"
        "}"
        "QLabel { color: #f7ead0; }"
        "QPushButton#EndTurnButton {"
        "  background: #c99a45;"
        "  border: 2px solid #ffe0a1;"
        "  border-radius: 18px;"
        "  color: #35100f;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  padding: 10px 16px;"
        "}"
        "QPushButton#EndTurnButton:hover { background: #e3bc64; }"
        "QPushButton#EndTurnButton:disabled { background: #5c3d31; border-color: #8f7454; color: #a8906a; }"
        "QPushButton#ConfirmCardButton {"
        "  background: #d9a84c;"
        "  border: 2px solid #ffe0a1;"
        "  border-radius: 18px;"
        "  color: #35100f;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  padding: 10px 16px;"
        "}"
        "QPushButton#ConfirmCardButton:hover { background: #edc66b; }"
        "QPushButton#ConfirmCardButton:disabled { background: #5c3d31; border-color: #8f7454; color: #a8906a; }");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 14);
    rootLayout->setSpacing(12);

    rootLayout->addWidget(createArenaPanel(), 1);

    m_handWidget = new ArcHandWidget(this);
    rootLayout->addWidget(m_handWidget);

    connect(m_handWidget, &ArcHandWidget::cardClicked, this, &BattleWidget::selectCard);
    connect(m_endTurnButton, &QPushButton::clicked, this, &BattleWidget::endTurn);
    connect(m_confirmCardButton, &QPushButton::clicked, this, &BattleWidget::confirmCard);

    setupInitialDemoText();
}

QWidget *BattleWidget::createArenaPanel()
{
    QFrame *panel = new QFrame(this);
    panel->setStyleSheet(
        "QFrame {"
        "  background: qradialgradient(cx:0.5, cy:0.45, radius:0.72,"
        "                              stop:0 #c8a76a, stop:0.68 #8b4a32, stop:1 #4d1818);"
        "  border: 3px solid rgba(255, 220, 145, 170);"
        "  border-radius: 28px;"
        "}");

    QGridLayout *layout = new QGridLayout(panel);
    layout->setContentsMargins(22, 14, 22, 12);
    layout->setHorizontalSpacing(14);
    layout->setVerticalSpacing(8);
    layout->setColumnMinimumWidth(0, 118);
    layout->setColumnMinimumWidth(2, 118);
    layout->setColumnStretch(0, 0);
    layout->setColumnStretch(1, 5);
    layout->setColumnStretch(2, 0);
    layout->setRowMinimumHeight(2, 330);
    layout->setRowMinimumHeight(3, 246);
    layout->setRowStretch(2, 4);
    layout->setRowStretch(3, 2);

    m_arenaTitleLabel = new QLabel(panel);
    m_arenaTitleLabel->setAlignment(Qt::AlignCenter);
    m_arenaTitleLabel->setStyleSheet(
        "background: rgba(46, 30, 22, 125);"
        "border: 1px solid rgba(255, 226, 168, 95);"
        "border-radius: 14px;"
        "font-size: 20px;"
        "font-weight: 800;"
        "padding: 6px 18px;");

    m_bossPortrait = new PortraitWidget(GameText::Battle::bossName(), panel);
    m_enemyField = createEnemyField(panel);
    m_playerPortrait = new PortraitWidget(GameText::Battle::playerName(), panel);

    QWidget *leftInfoPanel = new QWidget(panel);
    leftInfoPanel->setMinimumWidth(118);
    leftInfoPanel->setStyleSheet(
        "background: rgba(26, 21, 18, 92);"
        "border: 2px solid rgba(255, 226, 168, 95);"
        "border-radius: 18px;");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftInfoPanel);
    leftLayout->setContentsMargins(10, 12, 10, 12);
    leftLayout->setSpacing(9);

    QLabel *leftTitleLabel = new QLabel(GameText::Battle::relicTitle(), leftInfoPanel);
    leftTitleLabel->setAlignment(Qt::AlignCenter);
    leftTitleLabel->setStyleSheet("color: rgba(247, 234, 208, 165); font-size: 13px; font-weight: 800;");
    leftLayout->addWidget(leftTitleLabel);

    for (int i = 0; i < 5; ++i) {
        QFrame *slot = new QFrame(leftInfoPanel);
        slot->setFixedSize(54, 54);
        slot->setStyleSheet(
            "background: rgba(245, 218, 164, 45);"
            "border: 2px solid rgba(255, 226, 168, 120);"
            "border-radius: 12px;");
        leftLayout->addWidget(slot, 0, Qt::AlignCenter);
    }
    leftLayout->addStretch();

    QWidget *rightControlPanel = new QWidget(panel);
    rightControlPanel->setMinimumWidth(118);
    rightControlPanel->setStyleSheet(
        "background: rgba(26, 21, 18, 92);"
        "border: 2px solid rgba(255, 226, 168, 95);"
        "border-radius: 18px;");
    QVBoxLayout *rightLayout = new QVBoxLayout(rightControlPanel);
    rightLayout->setContentsMargins(8, 12, 8, 12);
    rightLayout->setSpacing(10);
    rightLayout->addStretch();

    m_confirmCardButton = new QPushButton(GameText::Battle::confirmCardButton(), rightControlPanel);
    m_confirmCardButton->setObjectName("ConfirmCardButton");
    m_confirmCardButton->setFixedSize(112, 50);
    rightLayout->addWidget(m_confirmCardButton, 0, Qt::AlignCenter);

    m_endTurnButton = new QPushButton(GameText::Battle::endTurnButton(), rightControlPanel);
    m_endTurnButton->setObjectName("EndTurnButton");
    m_endTurnButton->setFixedSize(112, 50);
    rightLayout->addWidget(m_endTurnButton, 0, Qt::AlignCenter);

    m_energyWidget = new EnergyWidget(rightControlPanel);
    rightLayout->addStretch();
    QLabel *energyLabel = new QLabel(GameText::Battle::energyLabel(), rightControlPanel);
    energyLabel->setAlignment(Qt::AlignCenter);
    energyLabel->setStyleSheet("color: rgba(247, 234, 208, 175); font-size: 13px; font-weight: 800;");
    rightLayout->addWidget(energyLabel);
    rightLayout->addWidget(m_energyWidget, 0, Qt::AlignCenter | Qt::AlignBottom);

    layout->addWidget(m_arenaTitleLabel, 0, 0, 1, 3);
    layout->addWidget(m_bossPortrait, 1, 1, Qt::AlignCenter);
    layout->addWidget(leftInfoPanel, 1, 0, 3, 1);
    layout->addWidget(m_enemyField, 2, 1);
    layout->addWidget(m_playerPortrait, 3, 1, Qt::AlignHCenter | Qt::AlignBottom);
    layout->addWidget(rightControlPanel, 1, 2, 3, 1);

    return panel;
}

QWidget *BattleWidget::createEnemyField(QWidget *parent)
{
    QFrame *field = new QFrame(parent);
    field->setMinimumHeight(330);
    field->setStyleSheet(
        "QFrame {"
        "  background: rgba(72, 40, 30, 78);"
        "  border: 2px solid rgba(255, 226, 168, 95);"
        "  border-radius: 22px;"
        "}");

    QVBoxLayout *layout = new QVBoxLayout(field);
    layout->setContentsMargins(20, 10, 20, 18);
    layout->setSpacing(8);

    QLabel *titleLabel = new QLabel(GameText::Battle::enemyAreaTitle(), field);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: rgba(247, 234, 208, 155); font-size: 15px; font-weight: 800;");

    QWidget *cardRow = new QWidget(field);
    QHBoxLayout *cardLayout = new QHBoxLayout(cardRow);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(22);
    cardLayout->addStretch();

    m_enemyCard = new EnemyCardWidget(GameText::Battle::defaultEnemyName(), 40, cardRow);
    m_enemyCard->setIntent(GameText::Battle::defaultEnemyIntent());

    cardLayout->addWidget(m_enemyCard);
    cardLayout->addStretch();

    layout->addWidget(titleLabel);
    layout->addWidget(cardRow, 1);

    return field;
}

void BattleWidget::setBossBattle(bool isBossBattle)
{
    m_isBossBattle = isBossBattle;

    if (!m_bossPortrait || !m_enemyField) {
        return;
    }

    m_bossPortrait->setVisible(m_isBossBattle);
    m_enemyField->setMinimumHeight(m_isBossBattle ? 260 : 330);
    m_enemyField->setMaximumHeight(420);
}

void BattleWidget::setupInitialDemoText()
{
    m_arenaTitleLabel->setText(GameText::App::title());
    startRun();
}

void BattleWidget::startRun()
{
    m_cardLibrary = createDefaultCards();
    m_playerMaxHp = 70;
    m_playerHp = m_playerMaxHp;
    m_maxEnergy = 3;
    m_battleNumber = 1;
    m_enemiesDefeated = 0;
    m_runFinished = false;
    startBattle();
}

void BattleWidget::startBattle()
{
    m_hand.setDeck(m_cardLibrary);
    m_enemy = createEnemyForBattle();
    m_playerBlock = 0;
    m_energy = m_maxEnergy;
    m_turnNumber = 0;
    m_selectedCardIndex = -1;
    m_battleEnded = false;
    m_playerTurn = true;

    setBossBattle(m_battleNumber >= 4);
    if (m_enemyCard) {
        m_enemyCard->setEnemy(m_enemy.name(), m_enemy.maxHp());
    }
    if (m_arenaTitleLabel) {
        m_arenaTitleLabel->setText(GameText::Battle::battleTitleFormat()
                                      .arg(m_battleNumber)
                                      .arg(m_enemy.name()));
    }

    beginPlayerTurn();
}

void BattleWidget::beginPlayerTurn()
{
    ++m_turnNumber;
    m_playerBlock = 0;
    m_energy = m_maxEnergy;
    m_playerTurn = true;
    m_selectedCardIndex = -1;
    drawCards(5);
    refreshUi();
}

void BattleWidget::drawCards(int count)
{
    m_hand.drawCards(count);
    updateHandView();
}

void BattleWidget::updateHandView()
{
    m_handWidget->setCards(m_hand.buttonTexts());
    m_selectedCardIndex = -1;
    refreshHandButtons();
    refreshActionButtons();
}

void BattleWidget::refreshHandButtons()
{
    if (!m_handWidget) {
        return;
    }

    for (int i = 0; i < m_hand.size(); ++i) {
        const Card card = m_hand.cardAt(i);
        m_handWidget->setCardEnabled(i, m_playerTurn && !m_battleEnded && m_energy >= card.cost());
    }
    m_handWidget->setSelectedCard(m_selectedCardIndex);
}

void BattleWidget::refreshActionButtons()
{
    const bool canAct = m_playerTurn && !m_battleEnded;

    if (m_endTurnButton) {
        if (m_battleEnded) {
            m_endTurnButton->setEnabled(true);
            if (m_playerHp <= 0 || m_runFinished) {
                m_endTurnButton->setText(GameText::Battle::restartButton());
            } else {
                m_endTurnButton->setText(GameText::Battle::nextBattleButton());
            }
        } else {
            m_endTurnButton->setEnabled(canAct);
            m_endTurnButton->setText(GameText::Battle::endTurnButton());
        }
        const bool endTurnEnabled = m_endTurnButton->isEnabled();
        m_endTurnButton->setStyleSheet(endTurnEnabled
            ? "background: #d9a84c; border: 2px solid #ffe0a1; border-radius: 18px; color: white; font-size: 16px; font-weight: 800; padding: 10px 16px;"
            : "background: #3b201c; border: 2px solid #7f5f47; border-radius: 18px; color: #7d6655; font-size: 16px; font-weight: 800; padding: 10px 16px;");
    }

    bool canConfirm = false;
    if (canAct && m_selectedCardIndex >= 0 && m_selectedCardIndex < m_hand.size()) {
        canConfirm = m_energy >= m_hand.cardAt(m_selectedCardIndex).cost();
    }

    if (m_confirmCardButton) {
        m_confirmCardButton->setEnabled(canConfirm);
        m_confirmCardButton->setStyleSheet(canConfirm
            ? "background: #e6b755; border: 2px solid #fff0b7; border-radius: 18px; color: white; font-size: 16px; font-weight: 800; padding: 10px 16px;"
            : "background: #3b201c; border: 2px solid #7f5f47; border-radius: 18px; color: #7d6655; font-size: 16px; font-weight: 800; padding: 10px 16px;");
    }
}

void BattleWidget::refreshUi()
{
    if (m_playerPortrait) {
        m_playerPortrait->setStats(m_playerHp, m_playerBlock);
    }

    if (m_bossPortrait) {
        m_bossPortrait->setStats(m_isBossBattle ? m_enemy.hp() : 100, 0);
    }

    if (m_enemyCard) {
        m_enemyCard->setHp(m_enemy.hp());
        m_enemyCard->setIntent(m_enemy.intentText());
        if (!m_enemy.imagePath().isEmpty()) {
            m_enemyCard->setCardImage(m_enemy.imagePath());
        }
    }

    if (m_energyWidget) {
        m_energyWidget->setEnergy(m_energy, m_maxEnergy);
    }

    refreshHandButtons();
    refreshActionButtons();
}

void BattleWidget::playHitEffect(bool fromPlayer, const QObject *receiver, const char *slot)
{
    if (!m_playerPortrait || !m_enemyCard) {
        if (receiver && slot) {
            QMetaObject::invokeMethod(const_cast<QObject *>(receiver), slot, Qt::QueuedConnection);
        }
        return;
    }

    QLabel *orb = new QLabel(this);
    orb->setFixedSize(22, 22);
    orb->setStyleSheet(
        "background: #ffe48a;"
        "border: 2px solid #fff7c7;"
        "border-radius: 11px;");

    const QPoint playerPoint = m_playerPortrait->mapTo(this, m_playerPortrait->rect().center());
    const QPoint enemyPoint = m_enemyCard->mapTo(this, m_enemyCard->rect().center());
    const QPoint start = fromPlayer ? playerPoint : enemyPoint;
    const QPoint end = fromPlayer ? enemyPoint : playerPoint;

    orb->move(start - QPoint(11, 11));
    orb->show();
    orb->raise();

    QPropertyAnimation *animation = new QPropertyAnimation(orb, "pos", orb);
    animation->setDuration(260);
    animation->setStartValue(start - QPoint(11, 11));
    animation->setEndValue(end - QPoint(11, 11));
    connect(animation, &QPropertyAnimation::finished, orb, &QLabel::deleteLater);
    if (receiver && slot) {
        connect(animation, SIGNAL(finished()), receiver, slot);
    }
    animation->start(QAbstractAnimation::DeleteWhenStopped);
}

bool BattleWidget::checkBattleEnd()
{
    if (m_enemy.isDead()) {
        m_battleEnded = true;
        m_playerTurn = false;
        ++m_enemiesDefeated;
        m_hand.discardHand();
        updateHandView();
        if (m_battleNumber >= 4) {
            m_runFinished = true;
            if (m_arenaTitleLabel) {
                m_arenaTitleLabel->setText(GameText::Battle::runClearTitle());
            }
        } else {
            ++m_battleNumber;
            if (m_arenaTitleLabel) {
                m_arenaTitleLabel->setText(GameText::Battle::battleWinFormat().arg(m_enemiesDefeated));
            }
        }
        refreshUi();
        return true;
    }

    if (m_playerHp <= 0) {
        m_playerHp = 0;
        m_battleEnded = true;
        m_playerTurn = false;
        m_runFinished = true;
        m_hand.discardHand();
        updateHandView();
        if (m_arenaTitleLabel) {
            m_arenaTitleLabel->setText(GameText::Battle::battleFailTitle());
        }
        refreshUi();
        return true;
    }

    return false;
}

QList<Card> BattleWidget::createDefaultCards() const
{
    const Card attack(GameText::CardText::attackName(), 1, 6, 0, 0, 0, GameText::CardText::attackDescription());
    const Card defend(GameText::CardText::defendName(), 1, 0, 5, 0, 0, GameText::CardText::defendDescription());
    const Card night(GameText::CardText::nightName(), 1, 10, 0, 0, 2, GameText::CardText::nightDescription());

    QList<Card> deck;
    deck << attack << attack << attack << attack
         << defend << defend << defend << defend
         << night << night
         << Card(GameText::CardText::healName(), 1, 0, 0, 5, 0, GameText::CardText::healDescription())
         << Card(GameText::CardText::studyName(), 2, 15, 0, 0, 0, GameText::CardText::studyDescription())
         << Card(GameText::CardText::assistantName(), 1, 3, 0, 0, 0, GameText::CardText::assistantDescription(), 2)
         << Card(GameText::CardText::markName(), 1, 0, 4, 0, 0, GameText::CardText::markDescription(), 0, 2)
         << Card(GameText::CardText::inspirationName(), 0, 0, 0, 0, 0, GameText::CardText::inspirationDescription(), 0, 0, 2);
    return deck;
}

Enemy BattleWidget::createEnemyForBattle() const
{
    if (m_battleNumber >= 4) {
        return Enemy::createFinalExam();
    }

    const int roll = QRandomGenerator::global()->bounded(3);
    if (roll == 0) {
        return Enemy::createGaoshuHomework();
    }
    if (roll == 1) {
        return Enemy::createProgramProject();
    }
    return Enemy::createMidterm();
}

void BattleWidget::selectCard(int cardIndex)
{
    if (m_battleEnded || !m_playerTurn || cardIndex < 0 || cardIndex >= m_hand.size()) {
        return;
    }

    const Card card = m_hand.cardAt(cardIndex);
    if (m_energy < card.cost()) {
        return;
    }

    m_selectedCardIndex = cardIndex;
    refreshUi();
    refreshActionButtons();
}

void BattleWidget::confirmCard()
{
    if (m_battleEnded || !m_playerTurn || m_selectedCardIndex < 0 || m_selectedCardIndex >= m_hand.size()) {
        return;
    }

    const Card card = m_hand.cardAt(m_selectedCardIndex);
    if (m_energy < card.cost()) {
        m_selectedCardIndex = -1;
        refreshUi();
        return;
    }

    m_energy -= card.cost();

    if (card.damage() > 0) {
        m_enemy.takeDamage(card.damage());
        playHitEffect(true);
    }

    if (card.block() > 0) {
        m_playerBlock += card.block();
    }

    if (card.heal() > 0) {
        m_playerHp = qMin(m_playerMaxHp, m_playerHp + card.heal());
    }

    if (card.selfDamage() > 0) {
        m_playerHp -= card.selfDamage();
    }

    if (card.weak() > 0) {
        m_enemy.applyWeak(card.weak());
    }

    if (card.vulnerable() > 0) {
        m_enemy.applyVulnerable(card.vulnerable());
    }

    m_hand.takeCard(m_selectedCardIndex);
    m_selectedCardIndex = -1;
    if (card.draw() > 0) {
        m_hand.drawCards(card.draw());
    }
    updateHandView();

    if (!checkBattleEnd()) {
        refreshUi();
    }
}

void BattleWidget::endTurn()
{
    if (m_battleEnded) {
        if (m_playerHp <= 0 || m_runFinished) {
            startRun();
        } else {
            startBattle();
        }
        return;
    }

    if (!m_playerTurn) {
        return;
    }

    m_playerTurn = false;
    m_selectedCardIndex = -1;
    m_hand.discardHand();
    updateHandView();
    refreshUi();
    finishEnemyTurn();
}

void BattleWidget::finishEnemyTurn()
{
    if (m_battleEnded) {
        return;
    }

    const int enemyDamage = m_enemy.attackPlayer();
    const int blockedDamage = qMin(m_playerBlock, enemyDamage);
    const int realDamage = enemyDamage - blockedDamage;
    m_playerBlock -= blockedDamage;
    m_playerHp -= realDamage;

    if (checkBattleEnd()) {
        return;
    }

    beginPlayerTurn();
}
