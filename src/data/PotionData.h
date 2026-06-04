#ifndef POTIONDATA_H
#define POTIONDATA_H

#include <QString>
#include <vector>

// PotionData.h
// Put static potion definitions here: id, name, effect text, icon path.
// Example: "fire_potion" appears in RunData::potionIds and shows its effect in the status bar tooltip.
// Runtime ownership is stored in RunData::potionIds, with at most three entries.

struct PotionData {
    QString id;
    QString name;
    QString effect;
    QString iconPath;
    bool targetsEnemy = false;
    int damage = 0;
    int block = 0;
    int heal = 0;
};

class PotionCatalog
{
public:
    static PotionData firePotion();
    static PotionData blockPotion();
    static PotionData healingPotion();
    static PotionData byId(const QString &id);
    static std::vector<PotionData> rewardPotions();
};

#endif // POTIONDATA_H
