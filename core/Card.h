#ifndef CARD_H
#define CARD_H

#include <QString>

class Card
{
public:
    Card()
        : m_cost(0),
          m_damage(0),
          m_block(0),
          m_heal(0),
          m_selfDamage(0),
          m_weak(0),
          m_vulnerable(0),
          m_draw(0)
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
         int draw = 0)
        : m_name(name),
          m_cost(cost),
          m_damage(damage),
          m_block(block),
          m_heal(heal),
          m_selfDamage(selfDamage),
          m_description(description),
          m_weak(weak),
          m_vulnerable(vulnerable),
          m_draw(draw)
    {
    }

    QString name() const { return m_name; }
    int cost() const { return m_cost; }
    int damage() const { return m_damage; }
    int block() const { return m_block; }
    int heal() const { return m_heal; }
    int selfDamage() const { return m_selfDamage; }
    int weak() const { return m_weak; }
    int vulnerable() const { return m_vulnerable; }
    int draw() const { return m_draw; }
    QString description() const { return m_description; }

    QString buttonText() const
    {
        return QStringLiteral("%1\n费用 %2\n%3")
            .arg(m_name)
            .arg(m_cost)
            .arg(m_description);
    }

private:
    QString m_name;
    int m_cost;
    int m_damage;
    int m_block;
    int m_heal;
    int m_selfDamage;
    QString m_description;
    int m_weak;
    int m_vulnerable;
    int m_draw;
};

#endif // CARD_H
