#include "CardData.h"

#include <QRandomGenerator>

namespace {
CardData makeCard(const QString &id,
                  const QString &name,
                  CardType type,
                  int cost,
                  int damage,
                  int block,
                  int energyGain,
                  int strengthGain,
                  int enemyWeak,
                  int hitCount,
                  bool exhaust,
                  bool retain,
                  bool ethereal,
                  const QString &description,
                  const QString &icon)
{
    return {
        id,
        name,
        type,
        CardRarity::Common,
        cost,
        damage,
        block,
        energyGain,
        strengthGain,
        enemyWeak,
        hitCount,
        exhaust,
        retain,
        ethereal,
        description,
        icon,
        {}
    };
}

std::vector<CardData> rewardPool()
{
    return {
        makeCard(QStringLiteral("slash"), QString::fromUtf8(u8"劈砍"), CardType::Attack, 1, 8, 0, 0, 0, 0, 1, false, false, false,
                 QString::fromUtf8(u8"消耗 1 能量。造成 8 点伤害。"),
                 QStringLiteral(":/pictures/resources/pictures/flame.png")),
        makeCard(QStringLiteral("twin_strike"), QString::fromUtf8(u8"双重打击"), CardType::Attack, 1, 5, 0, 0, 0, 0, 2, false, false, false,
                 QString::fromUtf8(u8"消耗 1 能量。攻击 2 次，每次造成 5 点伤害。"),
                 QStringLiteral(":/pictures/resources/pictures/flame.png")),
        makeCard(QStringLiteral("heavy_hit"), QString::fromUtf8(u8"重击"), CardType::Attack, 2, 14, 0, 0, 0, 1, 1, false, false, false,
                 QString::fromUtf8(u8"消耗 2 能量。造成 14 点伤害，给予 1 层虚弱。"),
                 QStringLiteral(":/pictures/resources/pictures/brute.png")),
        makeCard(QStringLiteral("iron_guard"), QString::fromUtf8(u8"铁壁"), CardType::Skill, 1, 0, 8, 0, 0, 0, 1, false, true, false,
                 QString::fromUtf8(u8"消耗 1 能量。获得 8 点格挡。保留。"),
                 QStringLiteral(":/pictures/resources/pictures/heavy-helm.png")),
        makeCard(QStringLiteral("battle_focus"), QString::fromUtf8(u8"战意"), CardType::Skill, 1, 0, 0, 0, 2, 0, 1, true, false, false,
                 QString::fromUtf8(u8"消耗 1 能量。获得 2 点力量。消耗。"),
                 QStringLiteral(":/pictures/resources/pictures/Circle.png")),
        makeCard(QStringLiteral("quick_step"), QString::fromUtf8(u8"疾步"), CardType::Skill, 0, 0, 4, 1, 0, 0, 1, false, false, false,
                 QString::fromUtf8(u8"消耗 0 能量。获得 4 点格挡，并获得 1 点能量。"),
                 QStringLiteral(":/pictures/resources/pictures/campfire.png")),
        makeCard(QStringLiteral("ghost_armor"), QString::fromUtf8(u8"幽魂护甲"), CardType::Skill, 1, 0, 10, 0, 0, 0, 1, false, true, true,
                 QString::fromUtf8(u8"消耗 1 能量。获得 10 点格挡。保留，虚无。"),
                 QStringLiteral(":/pictures/resources/pictures/heavy-helm.png")),
    };
}
}

CardData CardCatalog::strike()
{
    CardData card = makeCard(QStringLiteral("strike"), QString::fromUtf8(u8"打击"), CardType::Attack, 1, 6, 0, 0, 0, 0, 1, false, false, false,
                             QString::fromUtf8(u8"消耗 1 能量。造成 6 点伤害。"),
                             QStringLiteral(":/pictures/resources/pictures/flame.png"));
    card.rarity = CardRarity::Basic;
    card.tags = {QStringLiteral("starter")};
    return card;
}

CardData CardCatalog::defend()
{
    CardData card = makeCard(QStringLiteral("defend"), QString::fromUtf8(u8"防御"), CardType::Skill, 1, 0, 5, 0, 0, 0, 1, false, false, false,
                             QString::fromUtf8(u8"消耗 1 能量。获得 5 点格挡。"),
                             QStringLiteral(":/pictures/resources/pictures/heavy-helm.png"));
    card.rarity = CardRarity::Basic;
    card.tags = {QStringLiteral("starter")};
    return card;
}

CardData CardCatalog::byId(const QString &id)
{
    if (id == QStringLiteral("strike")) {
        return strike();
    }
    if (id == QStringLiteral("defend")) {
        return defend();
    }

    for (const CardData &card : rewardPool()) {
        if (card.id == id) {
            return card;
        }
    }
    return strike();
}

std::vector<CardData> CardCatalog::starterDeck()
{
    return {
        strike(), strike(), strike(), strike(), strike(), strike(),
        defend(), defend(), defend(), defend(), defend(), defend(),
    };
}

std::vector<CardData> CardCatalog::rewardCards()
{
    std::vector<CardData> pool = rewardPool();
    std::vector<CardData> rewards;
    rewards.reserve(3);

    for (int i = 0; i < 3 && !pool.empty(); ++i) {
        const int index = QRandomGenerator::global()->bounded(static_cast<int>(pool.size()));
        rewards.push_back(pool[index]);
        pool.erase(pool.begin() + index);
    }
    return rewards;
}

QString CardCatalog::typeName(CardType type)
{
    switch (type) {
    case CardType::Attack:
        return QString::fromUtf8(u8"攻击");
    case CardType::Skill:
        return QString::fromUtf8(u8"技能");
    case CardType::Power:
        return QString::fromUtf8(u8"能力");
    case CardType::Status:
        return QString::fromUtf8(u8"状态");
    case CardType::Curse:
        return QString::fromUtf8(u8"诅咒");
    }
    return QString();
}

QString CardCatalog::rarityName(CardRarity rarity)
{
    switch (rarity) {
    case CardRarity::Basic:
        return QString::fromUtf8(u8"基础");
    case CardRarity::Common:
        return QString::fromUtf8(u8"普通");
    case CardRarity::Uncommon:
        return QString::fromUtf8(u8"罕见");
    case CardRarity::Rare:
        return QString::fromUtf8(u8"稀有");
    case CardRarity::Special:
        return QString::fromUtf8(u8"特殊");
    }
    return QString();
}
