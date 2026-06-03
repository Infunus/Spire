#ifndef RUNDATA_H
#define RUNDATA_H

#include <QString>

struct RunData {
    QString characterName = QString::fromUtf8("铁甲战士");
    int currentFloor = 1;
    int currentHp = 80;
    int maxHp = 80;
    int gold = 99;
    int potions = 3;
};

#endif // RUNDATA_H
