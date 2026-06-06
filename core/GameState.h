#ifndef GAMESTATE_H
#define GAMESTATE_H

#include "CardLibrary.h"
#include "GameBalance.h"
#include "GameRandom.h"
#include "Potion.h"
#include "Relic.h"

#include <QJsonArray>
#include <QJsonObject>
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
        resetForNewRunWithSeed(GameRandom::instance().createRandomSeed());
    }

    void resetForNewRunWithSeed(quint32 seed)
    {
        m_maxHp = GameBalance::Player::startMaxHp();
        m_hp = m_maxHp;
        m_runSeed = seed == 0 ? 1 : seed;
        m_hasRunSeed = true;
        GameRandom::instance().setSeed(m_runSeed);
        m_gradeScore = 0;
        m_finalExamScore = 0;
        m_nextBattleStartStrength = 0;
        m_nextBattleTurnBlock = 0;
        m_credits = 0;
        m_coins = GameBalance::Player::startCoins();
        m_currentFloor = 0;
        m_currentNodeType = MapNodeType::None;
        m_defeatedEnemies = 0;
        m_eventsFinished = 0;
        m_bossDefeated = false;
        m_ownedCards = CardLibrary::starterDeck();
        m_relics.clear();
        m_potions.clear();
        addRelic(RelicLibrary::starterRelic());
        addPotion(PotionLibrary::createCoffeeShot());
        addPotion(PotionLibrary::createAntiBreakSpray());
    }

    int hp() const { return m_hp; }
    int maxHp() const { return m_maxHp; }
    int gradeScore() const { return m_gradeScore; }
    int usualScore() const { return m_gradeScore; }
    int finalExamScore() const { return m_finalExamScore; }
    int nextBattleStartStrength() const { return m_nextBattleStartStrength; }
    int nextBattleTurnBlock() const { return m_nextBattleTurnBlock; }
    int totalScore() const
    {
        return qBound(0,
                      (m_gradeScore + m_finalExamScore) / 2,
                      GameBalance::CourseGrade::finalTotalScoreMax());
    }
    int credits() const { return m_credits; }
    int coins() const { return m_coins; }
    quint32 runSeed() const { return m_runSeed; }
    bool hasRunSeed() const { return m_hasRunSeed; }
    int currentFloor() const { return m_currentFloor; }
    MapNodeType currentNodeType() const { return m_currentNodeType; }
    int defeatedEnemies() const { return m_defeatedEnemies; }
    int eventsFinished() const { return m_eventsFinished; }
    bool bossDefeated() const { return m_bossDefeated; }
    bool isDead() const { return m_hp <= 0; }

    QList<Card> ownedCards() const { return m_ownedCards; }
    QList<RelicData> relics() const { return m_relics; }
    QList<PotionData> potions() const { return m_potions; }
    int maxPotions() const { return GameBalance::Player::maxPotions(); }

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
        m_gradeScore = qBound(0,
                              m_gradeScore + amount,
                              GameBalance::CourseGrade::usualScoreMax());
    }

    void addUsualScore(int amount)
    {
        addGradeScore(amount);
    }

    void setFinalExamScore(int score)
    {
        m_finalExamScore = qBound(0,
                                  score,
                                  GameBalance::CourseGrade::finalExamScoreMax());
    }

    void addNextBattleStartStrength(int amount)
    {
        if (amount > 0) {
            m_nextBattleStartStrength += amount;
        }
    }

    int consumeNextBattleStartStrength()
    {
        const int amount = m_nextBattleStartStrength;
        m_nextBattleStartStrength = 0;
        return amount;
    }

    void addNextBattleTurnBlock(int amount)
    {
        if (amount > 0) {
            m_nextBattleTurnBlock += amount;
        }
    }

    int consumeNextBattleTurnBlock()
    {
        const int amount = m_nextBattleTurnBlock;
        m_nextBattleTurnBlock = 0;
        return amount;
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

    bool canUpgradeCardAt(int index) const
    {
        if (index < 0 || index >= m_ownedCards.size()) {
            return false;
        }
        return CardLibrary::canUpgrade(m_ownedCards.at(index));
    }

    bool upgradeCardAt(int index)
    {
        if (!canUpgradeCardAt(index)) {
            return false;
        }
        m_ownedCards[index] = CardLibrary::upgradedCard(m_ownedCards.at(index));
        return true;
    }

    bool upgradeFirstCardByName(const QString &cardName)
    {
        for (int i = 0; i < m_ownedCards.size(); ++i) {
            if (m_ownedCards.at(i).name() == cardName || m_ownedCards.at(i).displayName() == cardName) {
                return upgradeCardAt(i);
            }
        }
        return false;
    }

    bool upgradeFirstCardById(const QString &cardId)
    {
        for (int i = 0; i < m_ownedCards.size(); ++i) {
            if (m_ownedCards.at(i).id() == cardId) {
                return upgradeCardAt(i);
            }
        }
        return false;
    }

    bool removeFirstCardByName(const QString &cardName)
    {
        for (int i = 0; i < m_ownedCards.size(); ++i) {
            if (m_ownedCards.at(i).name() == cardName || m_ownedCards.at(i).displayName() == cardName) {
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

    void addRelicById(const QString &relicId)
    {
        addRelic(RelicLibrary::findById(relicId));
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

    bool addPotion(const PotionData &potion)
    {
        if (potion.id.isEmpty() || m_potions.size() >= maxPotions()) {
            return false;
        }
        m_potions.append(potion);
        return true;
    }

    bool addPotionById(const QString &potionId)
    {
        return addPotion(PotionLibrary::findById(potionId));
    }

    bool removePotionAt(int index)
    {
        if (index < 0 || index >= m_potions.size()) {
            return false;
        }
        m_potions.removeAt(index);
        return true;
    }

    void recordEnemyDefeated(int usualScoreReward = GameBalance::Rewards::battleGradeScore(),
                             int creditReward = GameBalance::Rewards::battleCredits(),
                             int defeatedEnemyCount = 1)
    {
        m_defeatedEnemies += qMax(1, defeatedEnemyCount);
        addUsualScore(usualScoreReward);
        addCredits(creditReward);
        addCoins(GameBalance::Rewards::battleCoins());
    }

    void recordEventFinished()
    {
        ++m_eventsFinished;
    }

    void recordBossDefeated(int finalExamScore = GameBalance::CourseGrade::finalExamStartScore(),
                            int creditReward = GameBalance::Rewards::bossCredits())
    {
        m_bossDefeated = true;
        setFinalExamScore(finalExamScore);
        addCredits(creditReward);
        addCoins(GameBalance::Rewards::bossCoins());
    }

    double estimatedGpa() const
    {
        if (m_gradeScore >= GameBalance::Gpa::gradeAThreshold()) {
            return GameBalance::Gpa::gradeAGpa();
        }
        if (m_gradeScore >= GameBalance::Gpa::gradeAMinusThreshold()) {
            return GameBalance::Gpa::gradeAMinusGpa();
        }
        if (m_gradeScore >= GameBalance::Gpa::gradeBPlusThreshold()) {
            return GameBalance::Gpa::gradeBPlusGpa();
        }
        if (m_gradeScore >= GameBalance::Gpa::gradeBThreshold()) {
            return GameBalance::Gpa::gradeBGpa();
        }
        if (m_gradeScore >= GameBalance::Gpa::gradeBMinusThreshold()) {
            return GameBalance::Gpa::gradeBMinusGpa();
        }
        if (m_gradeScore >= GameBalance::Gpa::passThreshold()) {
            return GameBalance::Gpa::passGpa();
        }
        return GameBalance::Gpa::failGpa();
    }

    QJsonObject toJson() const
    {
        QJsonArray cardArray;
        for (const Card &card : m_ownedCards) {
            if (card.id().isEmpty()) {
                continue;
            }
            QJsonObject cardObject;
            cardObject[QStringLiteral("id")] = card.id();
            cardObject[QStringLiteral("upgraded")] = card.upgraded();
            cardArray.append(cardObject);
        }

        QJsonArray relicArray;
        for (const RelicData &relic : m_relics) {
            if (!relic.id.isEmpty()) {
                relicArray.append(relic.id);
            }
        }

        QJsonArray potionArray;
        for (const PotionData &potion : m_potions) {
            if (!potion.id.isEmpty()) {
                potionArray.append(potion.id);
            }
        }

        QJsonObject json;
        json[QStringLiteral("maxHp")] = m_maxHp;
        json[QStringLiteral("hp")] = m_hp;
        json[QStringLiteral("runSeed")] = QString::number(m_runSeed);
        json[QStringLiteral("gradeScore")] = m_gradeScore;
        json[QStringLiteral("finalExamScore")] = m_finalExamScore;
        json[QStringLiteral("nextBattleStartStrength")] = m_nextBattleStartStrength;
        json[QStringLiteral("nextBattleTurnBlock")] = m_nextBattleTurnBlock;
        json[QStringLiteral("credits")] = m_credits;
        json[QStringLiteral("coins")] = m_coins;
        json[QStringLiteral("currentFloor")] = m_currentFloor;
        json[QStringLiteral("currentNodeType")] = static_cast<int>(m_currentNodeType);
        json[QStringLiteral("defeatedEnemies")] = m_defeatedEnemies;
        json[QStringLiteral("eventsFinished")] = m_eventsFinished;
        json[QStringLiteral("bossDefeated")] = m_bossDefeated;
        json[QStringLiteral("ownedCards")] = cardArray;
        json[QStringLiteral("relics")] = relicArray;
        json[QStringLiteral("potions")] = potionArray;
        return json;
    }

    bool loadFromJson(const QJsonObject &json)
    {
        if (json.isEmpty()) {
            return false;
        }

        const int loadedMaxHp = json.value(QStringLiteral("maxHp"))
                                    .toInt(GameBalance::Player::startMaxHp());
        if (loadedMaxHp <= 0) {
            return false;
        }

        m_maxHp = qMax(1, loadedMaxHp);
        m_hp = qBound(0, json.value(QStringLiteral("hp")).toInt(m_maxHp), m_maxHp);

        const QJsonValue seedValue = json.value(QStringLiteral("runSeed"));
        if (seedValue.isUndefined() || seedValue.isNull()) {
            return false;
        }

        bool seedOk = false;
        quint32 loadedSeed = seedValue.toString().toUInt(&seedOk);
        if (!seedOk) {
            const int seedNumber = seedValue.toInt(0);
            loadedSeed = seedNumber <= 0 ? 1u : static_cast<quint32>(seedNumber);
        }
        if (loadedSeed == 0) {
            return false;
        }
        m_runSeed = loadedSeed;
        m_hasRunSeed = true;
        GameRandom::instance().setSeed(m_runSeed);

        m_gradeScore = qBound(0,
                              json.value(QStringLiteral("gradeScore")).toInt(0),
                              GameBalance::CourseGrade::usualScoreMax());
        m_finalExamScore = qBound(0,
                                  json.value(QStringLiteral("finalExamScore")).toInt(0),
                                  GameBalance::CourseGrade::finalExamScoreMax());
        m_nextBattleStartStrength = qMax(0, json.value(QStringLiteral("nextBattleStartStrength")).toInt(0));
        m_nextBattleTurnBlock = qMax(0, json.value(QStringLiteral("nextBattleTurnBlock")).toInt(0));
        m_credits = qMax(0, json.value(QStringLiteral("credits")).toInt(0));
        m_coins = qMax(0, json.value(QStringLiteral("coins")).toInt(GameBalance::Player::startCoins()));
        m_currentFloor = qMax(0, json.value(QStringLiteral("currentFloor")).toInt(0));
        const int nodeType = qBound(0,
                                    json.value(QStringLiteral("currentNodeType")).toInt(0),
                                    static_cast<int>(MapNodeType::Boss));
        m_currentNodeType = static_cast<MapNodeType>(nodeType);
        m_defeatedEnemies = qMax(0, json.value(QStringLiteral("defeatedEnemies")).toInt(0));
        m_eventsFinished = qMax(0, json.value(QStringLiteral("eventsFinished")).toInt(0));
        m_bossDefeated = json.value(QStringLiteral("bossDefeated")).toBool(false);

        QList<Card> loadedCards;
        const QJsonArray cardArray = json.value(QStringLiteral("ownedCards")).toArray();
        for (const QJsonValue &value : cardArray) {
            const QJsonObject cardObject = value.toObject();
            const Card card = CardLibrary::byId(cardObject.value(QStringLiteral("id")).toString(),
                                                cardObject.value(QStringLiteral("upgraded")).toBool(false));
            if (!card.id().isEmpty()) {
                loadedCards.append(card);
            }
        }
        m_ownedCards = loadedCards.isEmpty() ? CardLibrary::starterDeck() : loadedCards;

        m_relics.clear();
        const QJsonArray relicArray = json.value(QStringLiteral("relics")).toArray();
        for (const QJsonValue &value : relicArray) {
            addRelicById(value.toString());
        }
        if (m_relics.isEmpty()) {
            addRelic(RelicLibrary::starterRelic());
        }

        m_potions.clear();
        const QJsonArray potionArray = json.value(QStringLiteral("potions")).toArray();
        for (const QJsonValue &value : potionArray) {
            addPotionById(value.toString());
        }

        return true;
    }

private:
    GameState()
        : m_hp(GameBalance::Player::startMaxHp()),
          m_maxHp(GameBalance::Player::startMaxHp()),
          m_gradeScore(0),
          m_finalExamScore(0),
          m_nextBattleStartStrength(0),
          m_nextBattleTurnBlock(0),
          m_credits(0),
          m_coins(GameBalance::Player::startCoins()),
          m_runSeed(0),
          m_hasRunSeed(false),
          m_currentFloor(0),
          m_currentNodeType(MapNodeType::None),
          m_defeatedEnemies(0),
          m_eventsFinished(0),
          m_bossDefeated(false)
    {
        m_ownedCards = CardLibrary::starterDeck();
        addRelic(RelicLibrary::starterRelic());
        addPotion(PotionLibrary::createCoffeeShot());
        addPotion(PotionLibrary::createAntiBreakSpray());
    }

    GameState(const GameState &) = delete;
    GameState &operator=(const GameState &) = delete;

    int m_hp;
    int m_maxHp;
    int m_gradeScore;
    int m_finalExamScore;
    int m_nextBattleStartStrength;
    int m_nextBattleTurnBlock;
    int m_credits;
    int m_coins;
    quint32 m_runSeed;
    bool m_hasRunSeed;
    int m_currentFloor;
    MapNodeType m_currentNodeType;
    int m_defeatedEnemies;
    int m_eventsFinished;
    bool m_bossDefeated;
    QList<Card> m_ownedCards;
    QList<RelicData> m_relics;
    QList<PotionData> m_potions;
};

#endif // GAMESTATE_H
