#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include <QWidget>
#include <QPoint>
#include <QList>
#include <QPair>
#include <QRect>
#include <vector>

#include "data/CardData.h"
#include "data/EnemyData.h"
#include "data/PotionData.h"

class QLabel;
class QProgressBar;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QWidget;
class QFrame;
class QDragEnterEvent;
class QDropEvent;
class QResizeEvent;

// BattleView is a basic combat test scene.
// Example: startBattle(EnemyCatalog::brute()) starts a fight; battleFinished() emits after victory.
// Relic resolution that affects run data should be handled by GameManager, not directly in UI code.
class BattleView : public QWidget
{
    Q_OBJECT

public:
    explicit BattleView(QWidget *parent = nullptr);
    void startBattle(const EnemyData &enemyData);
    bool selectPotion(int potionIndex);

signals:
    void battleFinished();

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;
    void dragEnterEvent(QDragEnterEvent *event) override;
    void dropEvent(QDropEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    enum class PileKind {
        Draw,
        Discard,
        Exhaust
    };

    void setupBattlePiles();
    void triggerBattleStartRelics();
    void startPlayerTurn(bool firstTurn);
    void discardHandAtEndTurn();
    void triggerPlayerEndTurnEffects();
    void triggerStartTurnEffects();
    void runEnemyTurn();
    void reduceEndOfRoundStatus();
    void drawCards(int count);
    void shuffleDiscardIntoDrawPile();
    void shufflePile(std::vector<CardData> &pile);
    void refreshLabels();
    void rebuildHandButtons();
    void layoutHandButtons(bool animate);
    void refreshPileButtons();
    void showPileDialog(PileKind pileKind);
    QString cardLine(const CardData &card) const;
    void rebuildStatusIcons(QHBoxLayout *layout, const QList<QPair<QString, int>> &statuses, bool playerSide);
    void showStatusDialog(const QString &name, const QString &description);
    void showDamagePopup(QWidget *anchor, int damage);
    void playCard(int handIndex);
    void playDraggedCardIfValid(int handIndex);
    void animateCardToPile(int handIndex, PileKind pileKind);
    void endTurn();
    void finishVictory();
    void hidePotionConfirm();
    void useSelectedPotionOnEnemy();
    void useSelectedPotionOnPlayer();
    int playerAttackDamage(int baseDamage) const;
    int enemyAttackDamage(int baseDamage = -1) const;

    QLabel *titleLabel;
    QLabel *playerPortraitLabel;
    QLabel *enemyPortraitLabel;
    QLabel *enemyLabel;
    QLabel *enemyPreviewDamageLabel;
    QLabel *playerLabel;
    QLabel *logLabel;
    QProgressBar *playerHealthBar;
    QProgressBar *enemyHealthBar;
    QLabel *playerBlockBadge;
    QLabel *enemyBlockBadge;
    QWidget *playerStatusPanel;
    QWidget *enemyStatusPanel;
    QHBoxLayout *playerStatusLayout;
    QHBoxLayout *enemyStatusLayout;
    QWidget *potionConfirmPanel;
    QLabel *potionConfirmLabel;
    QFrame *arenaPanel;
    QWidget *handPanel;
    QHBoxLayout *pileButtonLayout;
    QPushButton *endTurnButton;
    QPushButton *drawPileButton;
    QPushButton *discardPileButton;
    QPushButton *exhaustPileButton;
    EnemyData enemy;
    std::vector<CardData> drawPile;
    std::vector<CardData> discardPile;
    std::vector<CardData> exhaustPile;
    std::vector<CardData> hand;
    std::vector<QPushButton*> handButtons;
    int enemyHp = 0;
    int block = 0;
    int maxEnergy = 3;
    int energy = 3;
    int baseDrawCount = 5;
    int firstTurnDrawBonus = 0;
    int firstTurnEnergyBonus = 0;
    int playerStrength = 0;
    int playerDexterity = 0;
    int playerWeak = 0;
    int playerPoison = 0;
    int playerRegen = 0;
    int enemyStrength = 0;
    int enemyWeak = 0;
    int enemyVulnerable = 0;
    int enemyBlock = 0;
    int enemyPoison = 0;
    int enemyIntentIndex = 0;
    int turnNumber = 0;
    int pendingInsertedCards = 0;
    int hoveredCardIndex = -1;
    int dragCardIndex = -1;
    QPoint dragStartPos;
    QRect dragReturnGeometry;
    bool draggingCardEntity = false;
    int selectedPotionIndex = -1;
    PotionData selectedPotion;
};

#endif // BATTLEVIEW_H
