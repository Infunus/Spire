#ifndef CARDDATA_H
#define CARDDATA_H

#include <QString>
#include <QStringList>
#include <vector>

// CardData.h
// Put static card definitions and card query interfaces here.
// Example: CardCatalog::strike() returns a 1-cost attack that deals 6 damage.
// Store id, name, card type, cost, damage, block, tags, and description here.
// Example: "damage = 6" belongs here, but "this copy is in hand" does not.
// Do not put runtime hand/deck/discard piles here; those belong in battle/Card or RunData.

enum class CardType {
    Attack,
    Skill,
    Power,
    Status,
    Curse
};

enum class CardRarity {
    Basic,
    Common,
    Uncommon,
    Rare,
    Special
};

struct CardData {
    QString id;
    QString name;
    CardType type = CardType::Attack;
    CardRarity rarity = CardRarity::Common;
    int cost = 0;
    int damage = 0;
    int block = 0;
    int heal = 0;
    int selfDamage = 0;
    int energyGain = 0;
    int strengthGain = 0;
    int enemyWeak = 0;
    int enemyVulnerable = 0;
    int draw = 0;
    int strengthMultiplier = 1;
    int hitCount = 1;
    bool exhaust = false;
    bool retain = false;
    bool ethereal = false;
    QString description;
    QString imagePath;
    QStringList tags;
};

class CardCatalog
{
public:
    static CardData strike();
    static CardData defend();
    static CardData byId(const QString &id);
    static std::vector<CardData> starterDeck();
    static std::vector<CardData> rewardCards();
    static QString typeName(CardType type);
    static QString rarityName(CardRarity rarity);
};

#endif // CARDDATA_H
