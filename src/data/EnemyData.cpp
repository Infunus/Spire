#include "EnemyData.h"

#include "GameText.h"

EnemyData EnemyCatalog::brute()
{
    return {
        QStringLiteral("project_nob"),
        GameText::EnemyText::projectNobName(),
        70,
        74,
        10,
        EnemyIntent::Attack,
        GameText::EnemyText::projectNobDescription(),
        QStringLiteral(":/pictures/resources/pictures/brute.png"),
        {
            {EnemyIntent::Attack, 10, 0},
            {EnemyIntent::AttackAndBuff, 8, 2},
            {EnemyIntent::Attack, 14, 0},
        }
    };
}

EnemyData EnemyCatalog::bullyMinion()
{
    return {
        QStringLiteral("ddl_slime"),
        GameText::EnemyText::ddlSlimeName(),
        32,
        36,
        8,
        EnemyIntent::Attack,
        GameText::EnemyText::ddlSlimeDescription(),
        QStringLiteral(":/pictures/resources/pictures/bully-minion.png"),
        {
            {EnemyIntent::Attack, 8, 0},
            {EnemyIntent::Defend, 5, 0},
            {EnemyIntent::Attack, 10, 0},
        }
    };
}

EnemyData EnemyCatalog::hydraBoss()
{
    return {
        QStringLiteral("final_exam"),
        GameText::EnemyText::finalExamName(),
        105,
        110,
        14,
        EnemyIntent::Attack,
        GameText::EnemyText::finalExamDescription(),
        QStringLiteral(":/pictures/resources/pictures/hydra.png"),
        {
            {EnemyIntent::Attack, 14, 0},
            {EnemyIntent::AttackAndBlock, 10, 8},
            {EnemyIntent::Buff, 4, 0},
            {EnemyIntent::Attack, 20, 0},
        }
    };
}

std::vector<EnemyData> EnemyCatalog::actOneEnemies()
{
    return {
        {
            QStringLiteral("campus_cultist"),
            GameText::EnemyText::campusCultistName(),
            38,
            42,
            6,
            EnemyIntent::Buff,
            GameText::EnemyText::campusCultistDescription(),
            QStringLiteral(":/pictures/resources/pictures/horned-helm.png"),
            {
                {EnemyIntent::Buff, 3, 0},
                {EnemyIntent::Attack, 6, 0},
                {EnemyIntent::Attack, 6, 0},
            }
        },
        {
            QStringLiteral("homework_worm"),
            GameText::EnemyText::homeworkWormName(),
            44,
            48,
            7,
            EnemyIntent::AttackAndBlock,
            GameText::EnemyText::homeworkWormDescription(),
            QStringLiteral(":/pictures/resources/pictures/daemon-skull.png"),
            {
                {EnemyIntent::AttackAndBlock, 7, 5},
                {EnemyIntent::Buff, 3, 0},
                {EnemyIntent::Attack, 11, 0},
            }
        },
        bullyMinion(),
        brute(),
    };
}

QString EnemyCatalog::intentName(EnemyIntent intent)
{
    switch (intent) {
    case EnemyIntent::Attack:
        return QString::fromUtf8(u8"攻击");
    case EnemyIntent::Defend:
        return QString::fromUtf8(u8"格挡");
    case EnemyIntent::Buff:
        return QString::fromUtf8(u8"强化");
    case EnemyIntent::Debuff:
        return QString::fromUtf8(u8"削弱");
    case EnemyIntent::Heal:
        return QString::fromUtf8(u8"治疗");
    case EnemyIntent::AttackAndBuff:
        return QString::fromUtf8(u8"攻击并强化");
    case EnemyIntent::AttackAndBlock:
        return QString::fromUtf8(u8"攻击并格挡");
    case EnemyIntent::Unknown:
        return QString::fromUtf8(u8"未知");
    }
    return QString();
}
