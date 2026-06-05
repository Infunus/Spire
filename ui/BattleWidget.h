#ifndef BATTLEWIDGET_H
#define BATTLEWIDGET_H

#include "../core/Card.h"
#include "../core/Enemy.h"
#include "../core/Hand.h"

#include <QList>
#include <QPoint>
#include <QPixmap>
#include <QPushButton>
#include <QWidget>

#include <functional>

class QEvent;
class QFrame;
class QHBoxLayout;
class QLabel;
class QPaintEvent;
class QProgressBar;
class QResizeEvent;
class EnemyUnitWidget;
class EnergyCrystalWidget;
class PlayerPortraitWidget;
struct PotionData;

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
    void paintEvent(QPaintEvent *event) override;
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
    void usePotionAt(int potionIndex, int targetEnemyIndex = -1);
    void playCard(int handIndex, int targetEnemyIndex = -1);
    void animateCardToPile(int handIndex, PileKind pileKind);
    void runNextEnemyAction();
    void playEnemyAttackAnimation(int enemyIndex);
    void applyEnemyAction(int enemyIndex);
    void showSlashEffect(QWidget *anchor);
    void showImpactFlash(QWidget *anchor);
    void collectEnemyScoreIfDefeated(int enemyIndex);
    void showPileDialog(PileKind pileKind);
    QString cardListText(const QList<Card> &cards) const;
    QString cardLine(const Card &card) const;
    QString resolveAssetPath(const QString &relativePath) const;
    bool checkBattleEnd();
    void scheduleMapCallback(bool victory);
    QList<Card> createDefaultCards() const;
    QList<Enemy> createEnemiesForBattle() const;
    int playerAttackDamage(const Card &card) const;
    int currentUsualScoreRewardForEnemy(const Enemy &enemy) const;
    int currentBattleUsualScoreReward() const;
    int currentFinalExamScore() const;
    void showDamagePopup(QWidget *anchor, int damage);
    bool handlePotionEvent(QWidget *potionWidget, QEvent *event);
    bool potionTargetsEnemy(const PotionData &potion) const;
    void clearPotionDrag();
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
    QList<bool> m_enemyScoreCollected;
    int m_playerHp;
    int m_playerMaxHp;
    int m_playerBlock;
    int m_playerStrength;
    int m_energy;
    int m_maxEnergy;
    int m_turnNumber;
    int m_battleNumber;
    int m_enemiesDefeated;
    int m_pendingUsualScoreReward;
    int m_hoveredCardIndex;
    int m_dragCardIndex;
    int m_dragPotionIndex;
    int m_enemyTurnIndex;
    QPoint m_dragStartPos;
    QPoint m_potionDragStartPos;
    bool m_draggingCardEntity;
    bool m_draggingPotionEntity;
    QLabel *m_dragPotionGhost;
    bool m_battleEnded;
    bool m_playerTurn;
    bool m_runFinished;
    bool m_mapCallbackScheduled;
    std::function<void(bool)> m_finishCallback;
    QPixmap m_backgroundPixmap;
};

#endif // BATTLEWIDGET_H
