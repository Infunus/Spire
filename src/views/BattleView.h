#ifndef BATTLEVIEW_H
#define BATTLEVIEW_H

#include <QWidget>
#include <vector>

#include "data/CardData.h"
#include "data/EnemyData.h"
#include "data/PotionData.h"

class QLabel;
class QPushButton;
class QHBoxLayout;
class QVBoxLayout;
class QWidget;

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
    void refreshPileButtons();
    void showPileDialog(PileKind pileKind);
    QString cardLine(const CardData &card) const;
    void playCard(int handIndex);
    void endTurn();
    void finishVictory();
    void hidePotionConfirm();
    void useSelectedPotionOnEnemy();
    void useSelectedPotionOnPlayer();
    int playerAttackDamage(int baseDamage) const;
    int enemyAttackDamage() const;

    QLabel *titleLabel;
    QLabel *enemyLabel;
    QLabel *playerLabel;
    QLabel *logLabel;
    QWidget *potionConfirmPanel;
    QLabel *potionConfirmLabel;
    QHBoxLayout *pileButtonLayout;
    QHBoxLayout *handLayout;
    QPushButton *endTurnButton;
    QPushButton *drawPileButton;
    QPushButton *discardPileButton;
    QPushButton *exhaustPileButton;
    EnemyData enemy;
    std::vector<CardData> drawPile;
    std::vector<CardData> discardPile;
    std::vector<CardData> exhaustPile;
    std::vector<CardData> hand;
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
    int enemyBlock = 0;
    int enemyPoison = 0;
    int turnNumber = 0;
    int selectedPotionIndex = -1;
    PotionData selectedPotion;
};

#endif // BATTLEVIEW_H
