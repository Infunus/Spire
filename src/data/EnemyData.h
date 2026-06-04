#ifndef ENEMYDATA_H
#define ENEMYDATA_H

#include <QString>
#include <vector>

// EnemyData.h
// Put static enemy definitions and enemy query interfaces here.
// Example: EnemyCatalog::brute() defines its hp range, base attack, icon, and intent.
// Store id, name, hp range, base attack, icon, intent, and description here.
// Example: "baseAttack = 9" belongs here, but "currentHp = 12" belongs in BattleView/Enemy.
// Do not put per-battle mutable enemy hp/turn state here; that belongs in battle/Enemy.

enum class EnemyIntent {
    Attack,
    Defend,
    Buff,
    Debuff,
    Heal,
    AttackAndBuff,
    AttackAndBlock,
    Unknown
};

struct EnemyActionData {
    EnemyIntent intent = EnemyIntent::Attack;
    int amount = 0;
    int extra = 0;
};

struct EnemyData {
    QString id;
    QString name;
    int minHp = 0;
    int maxHp = 0;
    int baseAttack = 0;
    EnemyIntent defaultIntent = EnemyIntent::Unknown;
    QString description;
    QString imagePath;
    std::vector<EnemyActionData> actions;
};

class EnemyCatalog
{
public:
    static EnemyData brute();
    static EnemyData bullyMinion();
    static EnemyData hydraBoss();
    static std::vector<EnemyData> actOneEnemies();
    static QString intentName(EnemyIntent intent);
};

#endif // ENEMYDATA_H
