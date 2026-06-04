#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "RunData.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    static GameManager* instance();
    const RunData& runData() const;
    void startNewRun();
    void setCompletedFloor(int floor);
    void addRelic(const QString &relicId);
    void addCard(const QString &cardId);
    bool upgradeCardAt(int index);
    bool removeCardAt(int index);
    void addGold(int amount);
    bool addPotion(const QString &potionId);
    bool removePotionAt(int index);
    bool spendGold(int amount);
    void heal(int amount);
    void takeDamage(int amount);
    void finishBattleVictory();

signals:
    void runDataChanged();
    void playerDied();
    void itemAcquired(const QString &text);

private:
    explicit GameManager(QObject *parent = nullptr);
    RunData currentRun;
    static GameManager *s_instance;
};

#endif // GAMEMANAGER_H
