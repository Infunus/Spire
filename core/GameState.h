#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "Card.h"

#include <QList>
#include <QString>
#include <QtGlobal>

enum class MapNodeType
{
    None,
    Battle,
    Event,
    Shop,
    Rest,
    Reward,
    Boss
};

struct RelicData
{
    QString id;
    QString name;
    QString description;
};

class GameState
{
public:
    static GameState &instance()
    {
        static GameState state;
        return state;
    }

    void resetForNewRun()
    {
        m_maxHp = 70;
        m_hp = m_maxHp;
        m_gradeScore = 0;
        m_credits = 0;
        m_coins = 0;
        m_currentFloor = 0;
        m_currentNodeType = MapNodeType::None;
        m_defeatedEnemies = 0;
        m_eventsFinished = 0;
        m_bossDefeated = false;
        m_ownedCards.clear();
        m_relics.clear();
    }

    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    int gradeScore() const { return m_gradeScore; }
    int credits() const { return m_credits; }
    int coins() const { return m_coins; }
    int currentFloor() const { return m_currentFloor; }
    MapNodeType currentNodeType() const { return m_currentNodeType; }
    int defeatedEnemies() const { return m_defeatedEnemies; }
    int eventsFinished() const { return m_eventsFinished; }
    bool bossDefeated() const { return m_bossDefeated; }
    bool isDead() const { return m_hp <= 0; }

    QList<Card> ownedCards() const { return m_ownedCards; }
    QList<RelicData> relics() const { return m_relics; }

    void setMaxHp(int maxHp, bool healToFull = false)
    {
        m_maxHp = qMax(1, maxHp);
        if (healToFull) {
            m_hp = m_maxHp;
        } else {
            m_hp = qBound(0, m_hp, m_maxHp);
        }
    }

    void setHp(int hp)
    {
        m_hp = qBound(0, hp, m_maxHp);
    }

    void heal(int amount)
    {
        if (amount > 0) {
            setHp(m_hp + amount);
        }
    }

    void loseHp(int amount)
    {
        if (amount > 0) {
            setHp(m_hp - amount);
        }
    }

    void addGradeScore(int amount)
    {
        m_gradeScore = qMax(0, m_gradeScore + amount);
    }

    void addCredits(int amount)
    {
        m_credits = qMax(0, m_credits + amount);
    }

    void addCoins(int amount)
    {
        m_coins = qMax(0, m_coins + amount);
    }

    bool spendCoins(int amount)
    {
        if (amount <= 0) {
            return true;
        }
        if (m_coins < amount) {
            return false;
        }
        m_coins -= amount;
        return true;
    }

    void setCurrentNode(MapNodeType nodeType)
    {
        m_currentNodeType = nodeType;
    }

    void advanceFloor(MapNodeType nodeType)
    {
        ++m_currentFloor;
        m_currentNodeType = nodeType;
    }

    void setOwnedCards(const QList<Card> &cards)
    {
        m_ownedCards = cards;
    }

    void addCard(const Card &card)
    {
        m_ownedCards.append(card);
    }

    bool removeCardAt(int index)
    {
        if (index < 0 || index >= m_ownedCards.size()) {
            return false;
        }
        m_ownedCards.removeAt(index);
        return true;
    }

    bool removeFirstCardByName(const QString &cardName)
    {
        for (int i = 0; i < m_ownedCards.size(); ++i) {
            if (m_ownedCards.at(i).name() == cardName) {
                m_ownedCards.removeAt(i);
                return true;
            }
        }
        return false;
    }

    void addRelic(const RelicData &relic)
    {
        if (relic.id.isEmpty() || hasRelic(relic.id)) {
            return;
        }
        m_relics.append(relic);
    }

    bool hasRelic(const QString &relicId) const
    {
        for (const RelicData &relic : m_relics) {
            if (relic.id == relicId) {
                return true;
            }
        }
        return false;
    }

    void recordEnemyDefeated(int gradeScoreReward = 5, int creditReward = 1)
    {
        ++m_defeatedEnemies;
        addGradeScore(gradeScoreReward);
        addCredits(creditReward);
    }

    void recordEventFinished(int gradeScoreReward = 2)
    {
        ++m_eventsFinished;
        addGradeScore(gradeScoreReward);
    }

    void recordBossDefeated(int gradeScoreReward = 20, int creditReward = 3)
    {
        m_bossDefeated = true;
        addGradeScore(gradeScoreReward);
        addCredits(creditReward);
    }

    double estimatedGpa() const
    {
        if (m_gradeScore >= 90) {
            return 4.0;
        }
        if (m_gradeScore >= 85) {
            return 3.7;
        }
        if (m_gradeScore >= 80) {
            return 3.3;
        }
        if (m_gradeScore >= 75) {
            return 3.0;
        }
        if (m_gradeScore >= 70) {
            return 2.7;
        }
        if (m_gradeScore >= 60) {
            return 2.0;
        }
        return 0.0;
    }

private:
    GameState()
        : m_hp(70),
          m_maxHp(70),
          m_gradeScore(0),
          m_credits(0),
          m_coins(0),
          m_currentFloor(0),
          m_currentNodeType(MapNodeType::None),
          m_defeatedEnemies(0),
          m_eventsFinished(0),
          m_bossDefeated(false)
    {
    }

    GameState(const GameState &) = delete;
    GameState &operator=(const GameState &) = delete;

    int m_hp;
    int m_maxHp;
    int m_gradeScore;
    int m_credits;
    int m_coins;
    int m_currentFloor;
    MapNodeType m_currentNodeType;
    int m_defeatedEnemies;
    int m_eventsFinished;
    bool m_bossDefeated;
    QList<Card> m_ownedCards;
    QList<RelicData> m_relics;
};

#endif // GAMESTATE_H
