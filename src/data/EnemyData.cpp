#include "EnemyData.h"

EnemyData EnemyCatalog::brute()
{
    return {
        QStringLiteral("brute"),
        QString::fromUtf8("暴徒"),
        42,
        48,
        9,
        EnemyIntent::Attack,
        QString::fromUtf8("基础近战敌人，用于测试普通战斗。"),
        QStringLiteral(":/pictures/resources/pictures/brute.png")
    };
}

EnemyData EnemyCatalog::bullyMinion()
{
    return {
        QStringLiteral("bully_minion"),
        QString::fromUtf8("小喽啰"),
        22,
        28,
        6,
        EnemyIntent::Attack,
        QString::fromUtf8("低血量敌人，用于测试多敌人战斗。"),
        QStringLiteral(":/pictures/resources/pictures/bully-minion.png")
    };
}

EnemyData EnemyCatalog::hydraBoss()
{
    return {
        QStringLiteral("hydra_boss"),
        QString::fromUtf8("多头蛇"),
        160,
        180,
        14,
        EnemyIntent::Attack,
        QString::fromUtf8("Boss 占位敌人。"),
        QStringLiteral(":/pictures/resources/pictures/hydra.png")
    };
}

std::vector<EnemyData> EnemyCatalog::actOneEnemies()
{
    return {brute(), bullyMinion(), hydraBoss()};
}

QString EnemyCatalog::intentName(EnemyIntent intent)
{
    switch (intent) {
    case EnemyIntent::Attack:
        return QString::fromUtf8("攻击");
    case EnemyIntent::Defend:
        return QString::fromUtf8("防御");
    case EnemyIntent::Buff:
        return QString::fromUtf8("强化");
    case EnemyIntent::Debuff:
        return QString::fromUtf8("削弱");
    case EnemyIntent::Unknown:
        return QString::fromUtf8("未知");
    }
    return QString();
}
