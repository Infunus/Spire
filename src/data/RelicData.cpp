#include "RelicData.h"

namespace {
RelicData fallbackRelic()
{
    return {
        QStringLiteral("unknown"),
        QString::fromUtf8(u8"未知遗物"),
        QString::fromUtf8(u8"这个遗物还没有定义效果。"),
        QString()
    };
}
}

RelicData RelicCatalog::burningBlood()
{
    return {
        QStringLiteral("burning_blood"),
        QString::fromUtf8(u8"燃烧之血"),
        QString::fromUtf8(u8"每场战斗胜利后回复 6 点生命。"),
        QStringLiteral(":/pictures/resources/pictures/flame.png")
    };
}

RelicData RelicCatalog::crackedCore()
{
    return {
        QStringLiteral("cracked_core"),
        QString::fromUtf8(u8"破碎核心"),
        QString::fromUtf8(u8"测试遗物：每场战斗开始时获得 1 点能量。"),
        QStringLiteral(":/pictures/resources/pictures/Circle.png")
    };
}

RelicData RelicCatalog::tinyChest()
{
    return {
        QStringLiteral("tiny_chest"),
        QString::fromUtf8(u8"小宝箱"),
        QString::fromUtf8(u8"测试遗物：宝箱房出现概率略微提高。"),
        QStringLiteral(":/pictures/resources/pictures/chest.png")
    };
}

RelicData RelicCatalog::luckyCharm()
{
    return {
        QStringLiteral("lucky_charm"),
        QString::fromUtf8(u8"幸运护符"),
        QString::fromUtf8(u8"测试遗物：商店遗物价格降低 10%。"),
        QString()
    };
}

RelicData RelicCatalog::byId(const QString &id)
{
    for (const RelicData &relic : testRelics()) {
        if (relic.id == id) {
            return relic;
        }
    }
    return fallbackRelic();
}

std::vector<RelicData> RelicCatalog::starterRelics()
{
    return {burningBlood()};
}

std::vector<RelicData> RelicCatalog::testRelics()
{
    return {burningBlood(), crackedCore(), tinyChest(), luckyCharm()};
}
