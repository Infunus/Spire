#ifndef GAMERANDOM_H
#define GAMERANDOM_H

#include <QRandomGenerator>
#include <QtGlobal>

// 全局随机源：游戏里所有随机结果都应该从这里取。
// 只要同一个 seed 下按同样顺序游玩，地图、遇敌、抽牌、事件、奖励都会复现。
class GameRandom
{
public:
    static GameRandom &instance()
    {
        static GameRandom random;
        return random;
    }

    void setSeed(quint32 seed)
    {
        if (seed == 0) {
            seed = 1;
        }
        m_seed = seed;
        m_generator.seed(m_seed);
    }

    quint32 seed() const { return m_seed; }

    int bounded(int upper)
    {
        if (upper <= 0) {
            return 0;
        }
        return int(m_generator.bounded(quint32(upper)));
    }

    quint32 createRandomSeed() const
    {
        quint32 seed = QRandomGenerator::system()->generate();
        return seed == 0 ? 1 : seed;
    }

private:
    GameRandom()
        : m_seed(1),
          m_generator(1)
    {
    }

    GameRandom(const GameRandom &) = delete;
    GameRandom &operator=(const GameRandom &) = delete;

    quint32 m_seed;
    QRandomGenerator m_generator;
};

#endif // GAMERANDOM_H
