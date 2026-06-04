#ifndef RELICDATA_H
#define RELICDATA_H

#include <QString>
#include <vector>

// RelicData.h
// Put static relic definitions here: id, name, effect text, icon path.
// Example: "burning_blood" heals 6 HP in GameManager::finishBattleVictory().
// Runtime ownership is stored in RunData::relicIds.
// Example: after buying tiny_chest, append "tiny_chest" to RunData::relicIds.

struct RelicData {
    QString id;
    QString name;
    QString effect;
    QString iconPath;
};

class RelicCatalog
{
public:
    static RelicData burningBlood();
    static RelicData crackedCore();
    static RelicData tinyChest();
    static RelicData luckyCharm();
    static RelicData byId(const QString &id);
    static std::vector<RelicData> starterRelics();
    static std::vector<RelicData> testRelics();
};

#endif // RELICDATA_H
