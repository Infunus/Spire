#include "BattleWidget.h"

#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QPixmap>
#include <QResizeEvent>
#include <QVBoxLayout>

ArcHandWidget::ArcHandWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumHeight(208);
    setAttribute(Qt::WA_StyledBackground, true);
}

void ArcHandWidget::setCards(const QStringList &cards)
{
    qDeleteAll(m_cardButtons);
    m_cardButtons.clear();

    for (const QString &cardText : cards) {
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
            "}");
        m_cardButtons.append(button);
    }

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
    const int cardWidth = qBound(118, width() / 7, 158);
    const int cardHeight = qBound(150, height() - 22, 184);
    const int centerX = width() / 2;
    const int baseY = height() - cardHeight - 2;
    const int spacing = qMin(cardWidth - 8, qMax(84, width() / (count + 2)));

    for (int i = 0; i < count; ++i) {
        const double offset = i - (count - 1) / 2.0;
        const int x = centerX + static_cast<int>(offset * spacing) - cardWidth / 2;
        const int arcLift = static_cast<int>(34 - offset * offset * 8);
        const int y = baseY - qMax(0, arcLift);

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
    m_portraitLabel->setText(QStringLiteral("立绘"));
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
      m_imageLabel(new QLabel(this)),
      m_nameLabel(new QLabel(name, this)),
      m_intentLabel(new QLabel(this)),
      m_hpLabel(new QLabel(this))
{
    setAttribute(Qt::WA_StyledBackground, true);
    setFixedSize(142, 178);
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
        "  font-size: 12px;"
        "  font-weight: 700;"
        "  padding: 2px 6px;"
        "}"
        "QLabel#EnemyHpLabel {"
        "  background: #a82f22;"
        "  border: 2px solid #ffd5b8;"
        "  border-radius: 14px;"
        "  color: white;"
        "  font-size: 16px;"
        "  font-weight: 900;"
        "}");

    m_imageLabel->setObjectName("EnemyImageLabel");
    m_imageLabel->setAlignment(Qt::AlignCenter);
    m_imageLabel->setText(QStringLiteral("敌人图"));
    m_imageLabel->setFixedSize(116, 92);

    m_nameLabel->setObjectName("EnemyNameLabel");
    m_nameLabel->setAlignment(Qt::AlignCenter);

    m_intentLabel->setObjectName("EnemyIntentLabel");
    m_intentLabel->setAlignment(Qt::AlignCenter);
    m_intentLabel->setFixedHeight(25);

    m_hpLabel->setObjectName("EnemyHpLabel");
    m_hpLabel->setAlignment(Qt::AlignCenter);
    m_hpLabel->setFixedSize(38, 32);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(10, 10, 10, 8);
    layout->setSpacing(5);
    layout->addWidget(m_imageLabel, 0, Qt::AlignCenter);
    layout->addWidget(m_nameLabel);
    layout->addWidget(m_intentLabel);
    layout->addStretch();

    m_hpLabel->setParent(this);
    m_hpLabel->move(width() - m_hpLabel->width() - 8, height() - m_hpLabel->height() - 8);
    m_hpLabel->raise();
    setHp(hp);
    setIntent(QStringLiteral("意图待定"));
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
    m_hpLabel->setText(QString::number(hp));
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
      m_enemyField(nullptr),
      m_playerField(nullptr),
      m_energyWidget(nullptr),
      m_handWidget(nullptr),
      m_endTurnButton(nullptr)
{
    setObjectName("BattleWidget");
    setStyleSheet(
        "#BattleWidget {"
        "  background: qlineargradient(x1:0, y1:0, x2:1, y2:1,"
        "                              stop:0 #1b2532, stop:0.46 #463a2b, stop:1 #241713);"
        "}"
        "QLabel { color: #f7ead0; }"
        "QPushButton#EndTurnButton {"
        "  background: #d98b2f;"
        "  border: 2px solid #ffd28a;"
        "  border-radius: 18px;"
        "  color: #2a1a0d;"
        "  font-size: 16px;"
        "  font-weight: 700;"
        "  padding: 10px 16px;"
        "}"
        "QPushButton#EndTurnButton:hover { background: #e29a3a; }");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(18, 18, 18, 14);
    rootLayout->setSpacing(12);

    rootLayout->addWidget(createArenaPanel(), 1);

    m_handWidget = new ArcHandWidget(this);
    rootLayout->addWidget(m_handWidget);

    setupInitialDemoText();
}

QWidget *BattleWidget::createArenaPanel()
{
    QFrame *panel = new QFrame(this);
    panel->setStyleSheet(
        "QFrame {"
        "  background: qradialgradient(cx:0.5, cy:0.45, radius:0.72,"
        "                              stop:0 #d9bd80, stop:0.72 #8f6d45, stop:1 #3b2a22);"
        "  border: 3px solid rgba(255, 226, 168, 150);"
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
    layout->setRowStretch(2, 3);
    layout->setRowStretch(3, 1);
    layout->setRowStretch(4, 0);

    m_arenaTitleLabel = new QLabel(panel);
    m_arenaTitleLabel->setAlignment(Qt::AlignCenter);
    m_arenaTitleLabel->setStyleSheet(
        "background: rgba(46, 30, 22, 125);"
        "border: 1px solid rgba(255, 226, 168, 95);"
        "border-radius: 14px;"
        "font-size: 20px;"
        "font-weight: 800;"
        "padding: 6px 18px;");

    m_bossPortrait = new PortraitWidget(QStringLiteral("Boss"), panel);
    m_enemyField = createEnemyField(panel);
    m_playerField = createPlayerField(panel);
    m_playerPortrait = new PortraitWidget(QStringLiteral("玩家"), panel);

    QWidget *leftInfoPanel = new QWidget(panel);
    leftInfoPanel->setMinimumWidth(118);
    leftInfoPanel->setStyleSheet(
        "background: rgba(26, 21, 18, 92);"
        "border: 2px solid rgba(255, 226, 168, 95);"
        "border-radius: 18px;");
    QVBoxLayout *leftLayout = new QVBoxLayout(leftInfoPanel);
    leftLayout->setContentsMargins(10, 12, 10, 12);
    leftLayout->setSpacing(9);

    QLabel *leftTitleLabel = new QLabel(QStringLiteral("燕园加成"), leftInfoPanel);
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

    m_endTurnButton = new QPushButton(QStringLiteral("结束回合"), rightControlPanel);
    m_endTurnButton->setObjectName("EndTurnButton");
    m_endTurnButton->setFixedSize(112, 50);
    rightLayout->addWidget(m_endTurnButton, 0, Qt::AlignCenter);

    m_energyWidget = new EnergyWidget(rightControlPanel);
    rightLayout->addStretch();
    QLabel *energyLabel = new QLabel(QStringLiteral("精力"), rightControlPanel);
    energyLabel->setAlignment(Qt::AlignCenter);
    energyLabel->setStyleSheet("color: rgba(247, 234, 208, 175); font-size: 13px; font-weight: 800;");
    rightLayout->addWidget(energyLabel);
    rightLayout->addWidget(m_energyWidget, 0, Qt::AlignCenter | Qt::AlignBottom);

    layout->addWidget(m_arenaTitleLabel, 0, 0, 1, 3);
    layout->addWidget(m_bossPortrait, 1, 1, Qt::AlignCenter);
    layout->addWidget(leftInfoPanel, 1, 0, 4, 1);
    layout->addWidget(m_enemyField, 2, 1);
    layout->addWidget(m_playerField, 3, 1);
    layout->addWidget(m_playerPortrait, 4, 1, Qt::AlignHCenter | Qt::AlignBottom);
    layout->addWidget(rightControlPanel, 1, 2, 4, 1);

    return panel;
}

QWidget *BattleWidget::createEnemyField(QWidget *parent)
{
    QFrame *field = new QFrame(parent);
    field->setMinimumHeight(218);
    field->setStyleSheet(
        "QFrame {"
        "  background: rgba(72, 40, 30, 78);"
        "  border: 2px solid rgba(255, 226, 168, 95);"
        "  border-radius: 22px;"
        "}");

    QVBoxLayout *layout = new QVBoxLayout(field);
    layout->setContentsMargins(20, 10, 20, 14);
    layout->setSpacing(8);

    QLabel *titleLabel = new QLabel(QStringLiteral("敌方单位区"), field);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: rgba(247, 234, 208, 155); font-size: 15px; font-weight: 800;");

    QWidget *cardRow = new QWidget(field);
    QHBoxLayout *cardLayout = new QHBoxLayout(cardRow);
    cardLayout->setContentsMargins(0, 0, 0, 0);
    cardLayout->setSpacing(22);
    cardLayout->addStretch();

    EnemyCardWidget *paperEnemy = new EnemyCardWidget(QStringLiteral("小论文"), 24, cardRow);
    paperEnemy->setIntent(QStringLiteral("下回合 4"));
    EnemyCardWidget *exerciseEnemy = new EnemyCardWidget(QStringLiteral("习题集"), 18, cardRow);
    exerciseEnemy->setIntent(QStringLiteral("堆叠压力"));
    EnemyCardWidget *ddlEnemy = new EnemyCardWidget(QStringLiteral("DDL"), 30, cardRow);
    ddlEnemy->setIntent(QStringLiteral("下回合 8"));

    cardLayout->addWidget(paperEnemy);
    cardLayout->addWidget(exerciseEnemy);
    cardLayout->addWidget(ddlEnemy);
    cardLayout->addStretch();

    layout->addWidget(titleLabel);
    layout->addWidget(cardRow, 1);

    return field;
}

QWidget *BattleWidget::createPlayerField(QWidget *parent)
{
    QFrame *field = new QFrame(parent);
    field->setMinimumHeight(76);
    field->setMaximumHeight(94);
    field->setStyleSheet(
        "QFrame {"
        "  background: rgba(28, 76, 67, 70);"
        "  border: 2px solid rgba(185, 215, 150, 95);"
        "  border-radius: 18px;"
        "}");

    QVBoxLayout *layout = new QVBoxLayout(field);
    layout->setContentsMargins(18, 8, 18, 8);
    layout->setSpacing(3);

    QLabel *titleLabel = new QLabel(QStringLiteral("我方状态区"), field);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("color: rgba(247, 234, 208, 160); font-size: 15px; font-weight: 800;");

    QLabel *hintLabel = new QLabel(QStringLiteral("少量状态 / 临时效果 / 遗物触发提示"), field);
    hintLabel->setAlignment(Qt::AlignCenter);
    hintLabel->setStyleSheet("color: rgba(247, 234, 208, 135); font-size: 13px;");

    layout->addWidget(titleLabel);
    layout->addWidget(hintLabel);

    return field;
}

void BattleWidget::setBossBattle(bool isBossBattle)
{
    m_isBossBattle = isBossBattle;

    if (!m_bossPortrait || !m_enemyField) {
        return;
    }

    m_bossPortrait->setVisible(m_isBossBattle);
    m_enemyField->setMinimumHeight(m_isBossBattle ? 142 : 218);
    m_enemyField->setMaximumHeight(m_isBossBattle ? 166 : 280);
}

void BattleWidget::setupInitialDemoText()
{
    m_arenaTitleLabel->setText(QStringLiteral("卷王博雅塔 · 战斗沙盘"));
    m_bossPortrait->setStats(100, 0);
    m_playerPortrait->setStats(70, 3);
    m_energyWidget->setEnergy(3, 3);
    setBossBattle(false);

    m_handWidget->setCards(QStringList()
                           << QStringLiteral("卷\n费用 1\n造成伤害")
                           << QStringLiteral("防破防\n费用 1\n获得格挡")
                           << QStringLiteral("熬夜\n费用 1\n高风险输出")
                           << QStringLiteral("摸鱼回血\n费用 1\n回复 HP")
                           << QStringLiteral("通宵复习\n费用 2\n强力攻击"));
}
