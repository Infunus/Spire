#include "PotionData.h"

namespace {
PotionData fallbackPotion()
{
    return {
        QStringLiteral("unknown"),
        QString::fromUtf8(u8"未知药水"),
        QString::fromUtf8(u8"这个药水还没有定义效果。"),
        QString(),
        false,
        0,
        0,
        0
    };
}
}

PotionData PotionCatalog::firePotion()
{
    return {
        QStringLiteral("fire_potion"),
        QString::fromUtf8(u8"火焰药水"),
        QString::fromUtf8(u8"指向怪物使用：造成 20 点伤害。"),
        QStringLiteral(":/pictures/resources/pictures/flame.png"),
        true,
        20,
        0,
        0
    };
}

PotionData PotionCatalog::blockPotion()
{
    return {
        QStringLiteral("block_potion"),
        QString::fromUtf8(u8"格挡药水"),
        QString::fromUtf8(u8"确认后使用：获得 12 点格挡。"),
        QStringLiteral(":/pictures/resources/pictures/heavy-helm.png"),
        false,
        0,
        12,
        0
    };
}

PotionData PotionCatalog::healingPotion()
{
    return {
        QStringLiteral("healing_potion"),
        QString::fromUtf8(u8"治疗药水"),
        QString::fromUtf8(u8"确认后使用：回复 15 点生命。"),
        QStringLiteral(":/pictures/resources/pictures/fireplace.png"),
        false,
        0,
        0,
        15
    };
}

PotionData PotionCatalog::byId(const QString &id)
{
    for (const PotionData &potion : rewardPotions()) {
        if (potion.id == id) {
            return potion;
        }
    }
    return fallbackPotion();
}

std::vector<PotionData> PotionCatalog::rewardPotions()
{
    return {firePotion(), blockPotion(), healingPotion()};
}
