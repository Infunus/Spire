#ifndef RUNDATA_H
#define RUNDATA_H

#include <QString>
#include <QStringList>

// RunData stores mutable state for the current run.
// Example: deckIds starts with six "strike" and six "defend"; BattleView draws from that list.
// Example: potionIds has at most three entries; StatusBarWidget shows and selects them by index.
// Static definitions for cards, enemies, events, relics, potions, and characters live in src/data.
struct RunData {
    QString characterName = QString::fromUtf8(u8"铁甲战士");
    int currentFloor = 0;
    int currentHp = 80;
    int maxHp = 80;
    int gold = 99;
    QStringList relicIds;
    QStringList deckIds;
    QStringList potionIds;
};

#endif // RUNDATA_H
