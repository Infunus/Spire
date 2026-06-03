#ifndef CARDDATA_H
#define CARDDATA_H

#include <QString>

struct CardData {
    QString id;
    QString name;
    QString type;
    int cost = 0;
    int damage = 0;
    int block = 0;
};

#endif // CARDDATA_H
