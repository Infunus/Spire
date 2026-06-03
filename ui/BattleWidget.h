#ifndef BATTLEWIDGET_H
#define BATTLEWIDGET_H

#include "../core/Card.h"
#include "../core/Enemy.h"
#include "../core/Hand.h"

#include <QList>
#include <QPushButton>
#include <QStringList>
#include <QWidget>

class QLabel;
class QHBoxLayout;
class QPoint;
class QProgressBar;
class QResizeEvent;

class ArcHandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArcHandWidget(QWidget *parent = nullptr);
    void setCards(const QStringList &cards);
    void setCardEnabled(int cardIndex, bool enabled);
    void setSelectedCard(int cardIndex);

signals:
    void cardClicked(int cardIndex);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateCardLayout();

    QList<QPushButton *> m_cardButtons;
    int m_selectedCardIndex;
};

class PortraitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PortraitWidget(const QString &name, QWidget *parent = nullptr);
    void setPortraitImage(const QString &imagePath);
    void setStats(int hp, int armor);

private:
    QLabel *m_portraitLabel;
    QLabel *m_nameLabel;
    QLabel *m_hpLabel;
    QLabel *m_armorLabel;
};

class EnemyCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EnemyCardWidget(const QString &name, int hp, QWidget *parent = nullptr);
    void setEnemy(const QString &name, int maxHp);
    void setCardImage(const QString &imagePath);
    void setHp(int hp);
    void setIntent(const QString &intent);

private:
    int m_maxHp;
    QLabel *m_imageLabel;
    QLabel *m_nameLabel;
    QLabel *m_intentLabel;
    QProgressBar *m_hpBar;
};

class EnergyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EnergyWidget(QWidget *parent = nullptr);
    void setEnergy(int current, int maximum);

private:
    QHBoxLayout *m_layout;
    QList<QLabel *> m_energyPips;
};

class BattleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BattleWidget(QWidget *parent = nullptr);
    void setBossBattle(bool isBossBattle);
    void startDebugBattle(bool bossBattle);

private slots:
    void selectCard(int cardIndex);
    void confirmCard();
    void endTurn();
    void finishEnemyTurn();

private:
    QWidget *createArenaPanel();
    QWidget *createEnemyField(QWidget *parent);
    void setupInitialDemoText();
    void startRun();
    void startBattle();
    void beginPlayerTurn();
    void drawCards(int count);
    void updateHandView();
    void refreshHandButtons();
    void refreshActionButtons();
    void refreshUi();
    bool checkBattleEnd();
    QList<Card> createDefaultCards() const;
    Enemy createEnemyForBattle() const;
    void playHitEffect(bool fromPlayer, const QObject *receiver = nullptr, const char *slot = nullptr);

    bool m_isBossBattle;
    QLabel *m_arenaTitleLabel;
    PortraitWidget *m_bossPortrait;
    PortraitWidget *m_playerPortrait;
    EnemyCardWidget *m_enemyCard;
    QWidget *m_enemyField;
    EnergyWidget *m_energyWidget;
    ArcHandWidget *m_handWidget;
    QPushButton *m_confirmCardButton;
    QPushButton *m_endTurnButton;

    QList<Card> m_cardLibrary;
    Hand m_hand;
    Enemy m_enemy;
    int m_playerHp;
    int m_playerMaxHp;
    int m_playerBlock;
    int m_energy;
    int m_maxEnergy;
    int m_turnNumber;
    int m_battleNumber;
    int m_enemiesDefeated;
    int m_selectedCardIndex;
    bool m_battleEnded;
    bool m_playerTurn;
    bool m_runFinished;
};

#endif // BATTLEWIDGET_H
