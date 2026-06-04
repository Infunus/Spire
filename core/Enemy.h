#ifndef ENEMY_H
#define ENEMY_H

#include "GameBalance.h"
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
    EnemyActionBlock,
    EnemyActionAttackAndBuff,
    EnemyActionAttackAndBlock
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
          m_block(0),
          m_strength(0),
          m_intentIndex(0),
          m_weakStacks(0),
          m_vulnerableStacks(0)
    {
    }

    Enemy(const QString &name,
          int maxHp,
          const QString &imagePath,
          const QList<EnemyAction> &actions,
          const QString &description = QString())
        : m_name(name),
          m_hp(maxHp),
          m_maxHp(maxHp),
          m_block(0),
          m_imagePath(imagePath),
          m_description(description),
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
    int block() const { return m_block; }
    QString imagePath() const { return m_imagePath; }
    QString description() const { return m_description; }
    int intentIndex() const { return m_intentIndex; }
    bool isDead() const { return m_hp <= 0; }
    int weakStacks() const { return m_weakStacks; }
    int vulnerableStacks() const { return m_vulnerableStacks; }
    int strength() const { return m_strength; }
    bool currentActionDealsDamage() const
    {
        const EnemyAction action = currentAction();
        return action.type == EnemyActionAttack
               || action.type == EnemyActionAttackAndBuff
               || action.type == EnemyActionAttackAndBlock;
    }

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
        } else if (action.type == EnemyActionBlock) {
            text = GameText::EnemyText::blockIntentFormat().arg(action.amount);
        } else if (action.type == EnemyActionAttackAndBuff) {
            text = GameText::EnemyText::attackAndBuffIntentFormat()
                       .arg(effectiveAttackDamage(action.amount))
                       .arg(action.extra);
        } else {
            text = GameText::EnemyText::attackAndBlockIntentFormat()
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
        if (m_block > 0) {
            states << GameText::EnemyText::blockStatusFormat().arg(m_block);
        }
        if (!states.isEmpty()) {
            text += GameText::EnemyText::statusSeparator()
                    + states.join(GameText::EnemyText::statusJoiner());
        }
        return text;
    }

    QString intentTooltip() const
    {
        const EnemyAction action = currentAction();
        QString rulesText;
        if (action.type == EnemyActionAttack) {
            rulesText = GameText::EnemyText::attackIntentRules(effectiveAttackDamage(action.amount));
        } else if (action.type == EnemyActionHeal) {
            rulesText = GameText::EnemyText::healIntentRules(action.amount);
        } else if (action.type == EnemyActionBuff) {
            rulesText = GameText::EnemyText::buffIntentRules(action.amount);
        } else if (action.type == EnemyActionBlock) {
            rulesText = GameText::EnemyText::blockIntentRules(action.amount);
        } else if (action.type == EnemyActionAttackAndBuff) {
            rulesText = GameText::EnemyText::attackAndBuffIntentRules(effectiveAttackDamage(action.amount),
                                                                      action.extra);
        } else {
            rulesText = GameText::EnemyText::attackAndBlockIntentRules(effectiveAttackDamage(action.amount),
                                                                       action.extra);
        }
        return GameText::Battle::intentTooltip(intentText(), rulesText);
    }

    int attackPlayer()
    {
        const EnemyAction action = currentAction();
        int damage = 0;
        m_block = 0;

        if (action.type == EnemyActionAttack) {
            damage = effectiveAttackDamage(action.amount);
        } else if (action.type == EnemyActionHeal) {
            heal(action.amount);
        } else if (action.type == EnemyActionBuff) {
            m_strength += action.amount;
        } else if (action.type == EnemyActionBlock) {
            m_block += action.amount;
        } else if (action.type == EnemyActionAttackAndBuff) {
            damage = effectiveAttackDamage(action.amount);
            m_strength += action.extra;
        } else if (action.type == EnemyActionAttackAndBlock) {
            damage = effectiveAttackDamage(action.amount);
            m_block += action.extra;
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
            realDamage = damage + damage / GameBalance::EnemyStatus::vulnerableBonusDivisor();
        }

        const int blockedDamage = qMin(m_block, realDamage);
        m_block -= blockedDamage;
        realDamage -= blockedDamage;
        if (realDamage <= 0) {
            return;
        }

        m_hp -= realDamage;
        if (m_hp < 0) {
            m_hp = 0;
        }
    }

    int previewIncomingDamage(int damage) const
    {
        if (damage <= 0) {
            return 0;
        }

        int realDamage = damage;
        if (m_vulnerableStacks > 0) {
            realDamage = damage + damage / GameBalance::EnemyStatus::vulnerableBonusDivisor();
        }

        realDamage -= qMin(m_block, realDamage);
        return qMax(0, realDamage);
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
        return createCampusCultist();
    }

    static Enemy createCampusCultist()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionBuff, GameBalance::Enemies::CampusCultist::firstBuff())
                << EnemyAction(EnemyActionAttack, GameBalance::Enemies::CampusCultist::attackA())
                << EnemyAction(EnemyActionAttack, GameBalance::Enemies::CampusCultist::attackB());
        return Enemy(GameText::EnemyText::campusCultistName(),
                     GameBalance::Enemies::CampusCultist::maxHp(),
                     GameText::EnemyText::campusCultistImage(),
                     actions,
                     GameText::EnemyText::campusCultistDescription());
    }

    static Enemy createProgramProject()
    {
        return createProjectNob();
    }

    static Enemy createProjectNob()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttack, GameBalance::Enemies::ProjectNob::attackA())
                << EnemyAction(EnemyActionAttackAndBuff,
                               GameBalance::Enemies::ProjectNob::attackB(),
                               GameBalance::Enemies::ProjectNob::buff())
                << EnemyAction(EnemyActionAttack, GameBalance::Enemies::ProjectNob::attackC());
        return Enemy(GameText::EnemyText::projectNobName(),
                     GameBalance::Enemies::ProjectNob::maxHp(),
                     GameText::EnemyText::projectNobImage(),
                     actions,
                     GameText::EnemyText::projectNobDescription());
    }

    static Enemy createMidterm()
    {
        return createHomeworkWorm();
    }

    static Enemy createHomeworkWorm()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttackAndBlock,
                               GameBalance::Enemies::HomeworkWorm::attackA(),
                               GameBalance::Enemies::HomeworkWorm::blockA())
                << EnemyAction(EnemyActionBuff, GameBalance::Enemies::HomeworkWorm::buff())
                << EnemyAction(EnemyActionAttack, GameBalance::Enemies::HomeworkWorm::attackB());
        return Enemy(GameText::EnemyText::homeworkWormName(),
                     GameBalance::Enemies::HomeworkWorm::maxHp(),
                     GameText::EnemyText::homeworkWormImage(),
                     actions,
                     GameText::EnemyText::homeworkWormDescription());
    }

    static Enemy createDdlSlime()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttack, GameBalance::Enemies::DdlSlime::attackA())
                << EnemyAction(EnemyActionBlock, GameBalance::Enemies::DdlSlime::block())
                << EnemyAction(EnemyActionAttack, GameBalance::Enemies::DdlSlime::attackB());
        return Enemy(GameText::EnemyText::ddlSlimeName(),
                     GameBalance::Enemies::DdlSlime::maxHp(),
                     GameText::EnemyText::ddlSlimeImage(),
                     actions,
                     GameText::EnemyText::ddlSlimeDescription());
    }

    static Enemy createFinalExam()
    {
        QList<EnemyAction> actions;
        actions << EnemyAction(EnemyActionAttack, GameBalance::Enemies::FinalExam::attackA())
                << EnemyAction(EnemyActionAttackAndBlock,
                               GameBalance::Enemies::FinalExam::attackB(),
                               GameBalance::Enemies::FinalExam::block())
                << EnemyAction(EnemyActionBuff, GameBalance::Enemies::FinalExam::buff())
                << EnemyAction(EnemyActionAttack, GameBalance::Enemies::FinalExam::attackC());
        return Enemy(GameText::EnemyText::finalExamName(),
                     GameBalance::Enemies::FinalExam::maxHp(),
                     GameText::EnemyText::finalExamImage(),
                     actions,
                     GameText::EnemyText::finalExamDescription());
    }

private:
    EnemyAction currentAction() const
    {
        if (m_actions.isEmpty()) {
            return EnemyAction(EnemyActionAttack, GameBalance::EnemyStatus::fallbackAttack());
        }
        return m_actions.at(m_intentIndex % m_actions.size());
    }

    int effectiveAttackDamage(int baseDamage) const
    {
        int damage = baseDamage + m_strength;
        if (m_weakStacks > 0) {
            damage = damage * GameBalance::EnemyStatus::weakDamageNumerator()
                     / GameBalance::EnemyStatus::weakDamageDenominator();
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
    int m_block;
    QString m_imagePath;
    QString m_description;
    int m_strength;
    QList<EnemyAction> m_actions;
    int m_intentIndex;
    int m_weakStacks;
    int m_vulnerableStacks;
};

#endif // ENEMY_H
