#ifndef ENEMY_H
#define ENEMY_H

#include "GameText.h"

#include <QList>
#include <QString>
#include <QStringList>
#include <QtGlobal>

enum EnemyActionType
{
    EnemyActionAttack,
    EnemyActionHeal,
    EnemyActionBuff,
    EnemyActionAttackAndBuff
};

struct EnemyAction
{
    EnemyAction()
        : type(EnemyActionAttack),
          amount(0),
          extra(0)
    {
    }

    EnemyAction(EnemyActionType actionType, int actionAmount, int actionExtra = 0)
        : type(actionType),
          amount(actionAmount),
          extra(actionExtra)
    {
    }

    EnemyActionType type;
    int amount;
    int extra;
};

class Enemy
{
public:
    Enemy()
        : m_hp(0),
          m_maxHp(0),
          m_strength(0),
          m_intentIndex(0),
          m_weakStacks(0),
          m_vulnerableStacks(0)
    {
    }

    Enemy(const QString &name,
          int maxHp,
          const QString &imagePath,
          const QList<EnemyAction> &actions)
        : m_name(name),
          m_hp(maxHp),
          m_maxHp(maxHp),
          m_imagePath(imagePath),
          m_strength(0),
          m_actions(actions),
          m_intentIndex(0),
          m_weakStacks(0),
          m_vulnerableStacks(0)
    {
    }

    QString name() const { return m_name; }
    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    QString imagePath() const { return m_imagePath; }
    int intentIndex() const { return m_intentIndex; }
    bool isDead() const { return m_hp <= 0; }
    int weakStacks() const { return m_weakStacks; }
    int vulnerableStacks() const { return m_vulnerableStacks; }
    int strength() const { return m_strength; }

    QString intentText() const
    {
        const EnemyAction action = currentAction();
        QString text;
        if (action.type == EnemyActionAttack) {
            text = GameText::EnemyText::attackIntentFormat().arg(effectiveAttackDamage(action.amount));
        } else if (action.type == EnemyActionHeal) {
            text = GameText::EnemyText::healIntentFormat().arg(action.amount);
        } else if (action.type == EnemyActionBuff) {
            text = GameText::EnemyText::buffIntentFormat().arg(action.amount);
        } else {
            text = GameText::EnemyText::attackAndBuffIntentFormat()
                       .arg(effectiveAttackDamage(action.amount))
                       .arg(action.extra);
        }

        QStringList states;
        if (m_weakStacks > 0) {
            states << GameText::EnemyText::weakStatusFormat().arg(m_weakStacks);
        }
        if (m_vulnerableStacks > 0) {
            states << GameText::EnemyText::vulnerableStatusFormat().arg(m_vulnerableStacks);
        }
        if (m_strength > 0) {
            states << GameText::EnemyText::strengthStatusFormat().arg(m_strength);
        }
        if (!states.isEmpty()) {
            text += GameText::EnemyText::statusSeparator()
                    + states.join(GameText::EnemyText::statusJoiner());
        }
        return text;
    }

    int attackPlayer()
    {
        const EnemyAction action = currentAction();
        int damage = 0;

        if (action.type == EnemyActionAttack) {
            damage = effectiveAttackDamage(action.amount);
        } else if (action.type == EnemyActionHeal) {
            heal(action.amount);
        } else if (action.type == EnemyActionBuff) {
            m_strength += action.amount;
        } else if (action.type == EnemyActionAttackAndBuff) {
            damage = effectiveAttackDamage(action.amount);
            m_strength += action.extra;
        }

        tickStatus();
        advanceIntent();
        return damage;
    }

    void takeDamage(int damage)
    {
        if (damage <= 0) {
            return;
        }

        int realDamage = damage;
        if (m_vulnerableStacks > 0) {
            realDamage = damage + damage / 2;
        }

        m_hp -= realDamage;
        if (m_hp < 0) {
            m_hp = 0;
        }
    }

    void heal(int amount)
    {
        if (amount <= 0 || isDead()) {
            return;
        }

        m_hp += amount;
        if (m_hp > m_maxHp) {
            m_hp = m_maxHp;
        }
    }

    void applyWeak(int stacks)
    {
        if (stacks > 0) {
            m_weakStacks += stacks;
        }
    }

    void applyVulnerable(int stacks)
    {
        if (stacks > 0) {
            m_vulnerableStacks += stacks;
        }
    }

    static Enemy createGaoshuHomework()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttack, 6)
                << EnemyAction(EnemyActionHeal, 4)
                << EnemyAction(EnemyActionAttack, 8);
        return Enemy(GameText::EnemyText::gaoshuHomeworkName(), 42, QString(), actions);
    }

    static Enemy createProgramProject()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionBuff, 2)
                << EnemyAction(EnemyActionAttack, 8)
                << EnemyAction(EnemyActionAttackAndBuff, 10, 1);
        return Enemy(GameText::EnemyText::programProjectName(), 56, QString(), actions);
    }

    static Enemy createMidterm()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttack, 10)
                << EnemyAction(EnemyActionAttack, 6)
                << EnemyAction(EnemyActionHeal, 5);
        return Enemy(GameText::EnemyText::midtermName(), 64, QString(), actions);
    }

    static Enemy createFinalExam()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttack, 12)
                << EnemyAction(EnemyActionBuff, 3)
                << EnemyAction(EnemyActionAttack, 18)
                << EnemyAction(EnemyActionHeal, 8);
        return Enemy(GameText::EnemyText::finalExamName(), 96, QString(), actions);
    }

private:
    EnemyAction currentAction() const
    {
        if (m_actions.isEmpty()) {
            return EnemyAction(EnemyActionAttack, 6);
        }
        return m_actions.at(m_intentIndex % m_actions.size());
    }

    int effectiveAttackDamage(int baseDamage) const
    {
        int damage = baseDamage + m_strength;
        if (m_weakStacks > 0) {
            damage = damage * 3 / 4;
        }
        return qMax(0, damage);
    }

    void tickStatus()
    {
        if (m_weakStacks > 0) {
            --m_weakStacks;
        }
        if (m_vulnerableStacks > 0) {
            --m_vulnerableStacks;
        }
    }

    void advanceIntent()
    {
        if (!m_actions.isEmpty()) {
            m_intentIndex = (m_intentIndex + 1) % m_actions.size();
        }
    }

    QString m_name;
    int m_hp;
    int m_maxHp;
    QString m_imagePath;
    int m_strength;
    QList<EnemyAction> m_actions;
    int m_intentIndex;
    int m_weakStacks;
    int m_vulnerableStacks;
};

#endif // ENEMY_H
