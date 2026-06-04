#ifndef CARD_H
#define CARD_H

#include "GameText.h"

#include <QString>

class Card
{
public:
    Card()
        : m_upgraded(false),
          m_cost(0),
          m_damage(0),
          m_block(0),
          m_heal(0),
          m_selfDamage(0),
          m_weak(0),
          m_vulnerable(0),
          m_draw(0),
          m_strengthGain(0),
          m_strengthMultiplier(1),
          m_exhaust(false)
    {
    }

    Card(const QString &name,
         int cost,
         int damage,
         int block,
         int heal,
         int selfDamage,
         const QString &description,
         int weak = 0,
         int vulnerable = 0,
         int draw = 0,
         int strengthGain = 0,
         int strengthMultiplier = 1,
         const QString &imagePath = QString(),
         bool exhaust = false,
         const QString &id = QString(),
         bool upgraded = false)
        : m_id(id),
          m_name(name),
          m_upgraded(upgraded),
          m_cost(cost),
          m_damage(damage),
          m_block(block),
          m_heal(heal),
          m_selfDamage(selfDamage),
          m_description(description),
          m_weak(weak),
          m_vulnerable(vulnerable),
          m_draw(draw),
          m_strengthGain(strengthGain),
          m_strengthMultiplier(strengthMultiplier),
          m_imagePath(imagePath),
          m_exhaust(exhaust)
    {
    }

    QString id() const { return m_id; }
    QString name() const { return m_name; }
    QString displayName() const
    {
        if (m_upgraded) {
            return GameText::CardText::upgradedNameFormat().arg(m_name);
        }
        return m_name;
    }
    bool upgraded() const { return m_upgraded; }
    int cost() const { return m_cost; }
    int damage() const { return m_damage; }
    int block() const { return m_block; }
    int heal() const { return m_heal; }
    int selfDamage() const { return m_selfDamage; }
    int weak() const { return m_weak; }
    int vulnerable() const { return m_vulnerable; }
    int draw() const { return m_draw; }
    int strengthGain() const { return m_strengthGain; }
    int strengthMultiplier() const { return m_strengthMultiplier; }
    QString imagePath() const { return m_imagePath; }
    bool exhaust() const { return m_exhaust; }
    QString description() const { return m_description; }

    QString buttonText() const
    {
        return GameText::CardText::buttonTextFormat()
            .arg(displayName())
            .arg(m_cost)
            .arg(m_description);
    }

private:
    QString m_id;
    QString m_name;
    bool m_upgraded;
    int m_cost;
    int m_damage;
    int m_block;
    int m_heal;
    int m_selfDamage;
    QString m_description;
    int m_weak;
    int m_vulnerable;
    int m_draw;
    int m_strengthGain;
    int m_strengthMultiplier;
    QString m_imagePath;
    bool m_exhaust;
};

#endif // CARD_H
