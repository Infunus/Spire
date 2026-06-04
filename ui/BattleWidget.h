#ifndef BATTLEWIDGET_H
#define BATTLEWIDGET_H

#include "../core/Card.h"
#include "../core/Enemy.h"
#include "../core/Hand.h"

#include <QList>
#include <QPoint>
#include <QPushButton>
#include <QWidget>

#include <functional>

class QEvent;
class QFrame;
class QHBoxLayout;
class QLabel;
class QProgressBar;
class QResizeEvent;
class EnemyUnitWidget;
class EnergyCrystalWidget;
class PlayerPortraitWidget;

class BattleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BattleWidget(QWidget *parent = nullptr);
    void setBossBattle(bool isBossBattle);
    void startDebugBattle(bool bossBattle);
    void startMapBattle(bool bossBattle, const std::function<void(bool)> &finishCallback);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void endTurn();
    void finishEnemyTurn();

private:
    enum class PileKind {
        Draw,
        Discard,
        Exhaust
    };

    QWidget *createArenaPanel();
    QWidget *createControlStrip();
    void setupInitialDemoText();
    void startRun();
    void startBattle();
    void beginPlayerTurn();
    void drawCards(int count);
    void rebuildHandButtons();
    void layoutHandButtons(bool animate);
    void refreshUi();
    void rebuildEnemyWidgets();
    void layoutEnemyWidgets();
    void refreshActionButtons();
    void refreshRelicSlots();
    void refreshPotionSlots();
    void refreshPileButtons();
    void rebuildStatusIcons(QHBoxLayout *layout, const QList<QPair<QString, int>> &statuses);
    void applyRelicsAtBattleStart();
    void applyRelicsAfterBattleWin();
    void usePotionAt(int potionIndex);
    void playCard(int handIndex, int targetEnemyIndex = -1);
    void animateCardToPile(int handIndex, PileKind pileKind);
    void runNextEnemyAction();
    void playEnemyAttackAnimation(int enemyIndex);
    void applyEnemyAction(int enemyIndex);
    void showSlashEffect(QWidget *anchor);
    void showImpactFlash(QWidget *anchor);
    void showPileDialog(PileKind pileKind);
    QString cardListText(const QList<Card> &cards) const;
    QString cardLine(const Card &card) const;
    bool checkBattleEnd();
    void scheduleMapCallback(bool victory);
    QList<Card> createDefaultCards() const;
    QList<Enemy> createEnemiesForBattle() const;
    int playerAttackDamage(const Card &card) const;
    void showDamagePopup(QWidget *anchor, int damage);
    int firstAliveEnemyIndex() const;
    int enemyIndexAtGlobalPoint(const QPoint &globalPoint) const;
    bool allEnemiesDead() const;
    void clearStatusLayout(QHBoxLayout *layout);

    bool m_isBossBattle;
    QLabel *m_titleLabel;
    PlayerPortraitWidget *m_playerPortraitWidget;
    QWidget *m_enemyBoard;
    QLabel *m_playerInfoLabel;
    QLabel *m_tipLabel;
    QLabel *m_playerBlockBadge;
    QProgressBar *m_playerHealthBar;
    QWidget *m_playerStatusPanel;
    QHBoxLayout *m_playerStatusLayout;
    EnergyCrystalWidget *m_energyCrystalWidget;
    QWidget *m_handPanel;
    QPushButton *m_endTurnButton;
    QPushButton *m_drawPileButton;
    QPushButton *m_discardPileButton;
    QPushButton *m_exhaustPileButton;
    QList<QLabel *> m_relicLabels;
    QList<QPushButton *> m_potionButtons;
    QList<QPushButton *> m_handButtons;
    QList<EnemyUnitWidget *> m_enemyWidgets;

    QList<Card> m_cardLibrary;
    QList<Card> m_exhaustPile;
    Hand m_hand;
    QList<Enemy> m_enemies;
    int m_playerHp;
    int m_playerMaxHp;
    int m_playerBlock;
    int m_playerStrength;
    int m_energy;
    int m_maxEnergy;
    int m_turnNumber;
    int m_battleNumber;
    int m_enemiesDefeated;
    int m_hoveredCardIndex;
    int m_dragCardIndex;
    int m_enemyTurnIndex;
    QPoint m_dragStartPos;
    bool m_draggingCardEntity;
    bool m_battleEnded;
    bool m_playerTurn;
    bool m_runFinished;
    bool m_mapCallbackScheduled;
    std::function<void(bool)> m_finishCallback;
};

#endif // BATTLEWIDGET_H
