#ifndef ENEMYDATA_H
#define ENEMYDATA_H

#include <QString>

struct EnemyData {
    QString id;
    QString name;
    int health = 0;
    int attack = 0;
    QString description;
};

#endif // ENEMYDATA_H
