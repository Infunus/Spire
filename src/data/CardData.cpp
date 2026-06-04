#include "CardData.h"

#include "GameText.h"

#include <QRandomGenerator>

namespace {
CardData makeCard(const QString &id,
                  const QString &name,
                  CardType type,
                  CardRarity rarity,
                  int cost,
                  int damage,
                  int block,
                  int heal,
                  int selfDamage,
                  int energyGain,
                  int strengthGain,
                  int enemyWeak,
                  int enemyVulnerable,
                  int draw,
                  int strengthMultiplier,
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
        rarity,
        cost,
        damage,
        block,
        heal,
        selfDamage,
        energyGain,
        strengthGain,
        enemyWeak,
        enemyVulnerable,
        draw,
        strengthMultiplier,
        hitCount,
        exhaust,
        retain,
        ethereal,
        QString::fromUtf8(u8"消耗 %1 能量。%2").arg(cost).arg(description),
        icon,
        {}
    };
}

QString attackIcon()
{
    return QStringLiteral(":/pictures/resources/pictures/flame.png");
}

QString skillIcon()
{
    return QStringLiteral(":/pictures/resources/pictures/heavy-helm.png");
}

std::vector<CardData> rewardPool()
{
    return {
        makeCard(QStringLiteral("bash"), GameText::CardText::bashName(), CardType::Attack, CardRarity::Common,
                 2, 8, 0, 0, 0, 0, 0, 0, 2, 0, 1, 1, false, false, false,
                 GameText::CardText::bashDescription(), attackIcon()),
        makeCard(QStringLiteral("inflame"), GameText::CardText::inflameName(), CardType::Skill, CardRarity::Uncommon,
                 1, 0, 0, 0, 0, 0, 2, 0, 0, 0, 1, 1, true, false, false,
                 GameText::CardText::inflameDescription(), QStringLiteral(":/pictures/resources/pictures/Circle.png")),
        makeCard(QStringLiteral("heavy_blade"), GameText::CardText::heavyBladeName(), CardType::Attack, CardRarity::Uncommon,
                 2, 14, 0, 0, 0, 0, 0, 0, 0, 0, 3, 1, false, false, false,
                 GameText::CardText::heavyBladeDescription(), QStringLiteral(":/pictures/resources/pictures/brute.png")),
        makeCard(QStringLiteral("pommel"), GameText::CardText::pommelName(), CardType::Attack, CardRarity::Common,
                 1, 9, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, false, false, false,
                 GameText::CardText::pommelDescription(), attackIcon()),
        makeCard(QStringLiteral("shrug"), GameText::CardText::shrugName(), CardType::Skill, CardRarity::Common,
                 1, 0, 8, 0, 0, 0, 0, 0, 0, 1, 1, 1, false, false, false,
                 GameText::CardText::shrugDescription(), skillIcon()),
        makeCard(QStringLiteral("anger"), GameText::CardText::angerName(), CardType::Attack, CardRarity::Common,
                 0, 4, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, false, false, false,
                 GameText::CardText::angerDescription(), attackIcon()),
        makeCard(QStringLiteral("flex"), GameText::CardText::flexName(), CardType::Skill, CardRarity::Common,
                 0, 0, 0, 0, 0, 0, 3, 0, 0, 1, 1, 1, true, false, false,
                 GameText::CardText::flexDescription(), QStringLiteral(":/pictures/resources/pictures/Circle.png")),
        makeCard(QStringLiteral("cleave"), GameText::CardText::cleaveName(), CardType::Attack, CardRarity::Common,
                 1, 8, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, false, false, false,
                 GameText::CardText::cleaveDescription(), attackIcon()),
    };
}
}

CardData CardCatalog::strike()
{
    CardData card = makeCard(QStringLiteral("strike"), GameText::CardText::strikeName(), CardType::Attack, CardRarity::Basic,
                             1, 6, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, false, false, false,
                             GameText::CardText::strikeDescription(), attackIcon());
    card.tags = {QStringLiteral("starter")};
    return card;
}

CardData CardCatalog::defend()
{
    CardData card = makeCard(QStringLiteral("defend"), GameText::CardText::defendName(), CardType::Skill, CardRarity::Basic,
                             1, 0, 5, 0, 0, 0, 0, 0, 0, 0, 1, 1, false, false, false,
                             GameText::CardText::defendDescription(), skillIcon());
    card.tags = {QStringLiteral("starter")};
    return card;
}

CardData CardCatalog::byId(const QString &id)
{
    const bool upgraded = isUpgradedId(id);
    const QString baseId = upgraded ? id.left(id.size() - 1) : id;
    CardData card;
    if (baseId == QStringLiteral("strike")) {
        card = strike();
    } else if (baseId == QStringLiteral("defend")) {
        card = defend();
    } else {
        for (const CardData &candidate : rewardPool()) {
            if (candidate.id == baseId) {
                card = candidate;
                break;
            }
        }
        if (card.id.isEmpty()) {
            card = strike();
        }
    }

    if (!upgraded) {
        return card;
    }

    card.id = upgradedId(card.id);
    card.name += QStringLiteral("+");
    if (card.damage > 0) {
        card.damage += qMax(2, card.damage / 3);
    }
    if (card.block > 0) {
        card.block += qMax(2, card.block / 3);
    }
    if (card.enemyWeak > 0) {
        ++card.enemyWeak;
    }
    if (card.enemyVulnerable > 0) {
        ++card.enemyVulnerable;
    }
    if (card.cost > 0 && card.damage == 0 && card.block == 0) {
        --card.cost;
    }
    card.description = QString::fromUtf8(u8"升级：%1").arg(card.description);
    return card;
}

QString CardCatalog::upgradedId(const QString &id)
{
    return isUpgradedId(id) ? id : id + QStringLiteral("+");
}

bool CardCatalog::isUpgradedId(const QString &id)
{
    return id.endsWith(QLatin1Char('+'));
}

#if 0
CardData CardCatalog::byIdOld(const QString &id)
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
#endif

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
