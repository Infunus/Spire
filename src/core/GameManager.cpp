#include "GameManager.h"
#include "data/CardData.h"
#include "data/PotionData.h"
#include "data/RelicData.h"

#include <QApplication>

GameManager* GameManager::s_instance = nullptr;

GameManager* GameManager::instance()
{
    if (!s_instance) {
        s_instance = new GameManager(qApp);
    }
    return s_instance;
}

GameManager::GameManager(QObject *parent)
    : QObject(parent)
{
}

const RunData& GameManager::runData() const
{
    return currentRun;
}

void GameManager::startNewRun()
{
    currentRun = RunData();
    currentRun.relicIds = {
        QStringLiteral("burning_blood"),
    };
    currentRun.deckIds = {
        QStringLiteral("strike"), QStringLiteral("strike"), QStringLiteral("strike"),
        QStringLiteral("strike"), QStringLiteral("strike"), QStringLiteral("strike"),
        QStringLiteral("defend"), QStringLiteral("defend"), QStringLiteral("defend"),
        QStringLiteral("defend"), QStringLiteral("defend"), QStringLiteral("defend"),
    };
    emit runDataChanged();
}

void GameManager::setCompletedFloor(int floor)
{
    const int normalizedFloor = qMax(0, floor);
    if (currentRun.currentFloor == normalizedFloor) {
        return;
    }

    currentRun.currentFloor = normalizedFloor;
    emit runDataChanged();
}

void GameManager::addRelic(const QString &relicId)
{
    if (currentRun.relicIds.contains(relicId)) {
        return;
    }

    currentRun.relicIds.append(relicId);
    emit runDataChanged();
    emit itemAcquired(QString::fromUtf8(u8"你获得了遗物：%1").arg(RelicCatalog::byId(relicId).name));
}

void GameManager::addCard(const QString &cardId)
{
    currentRun.deckIds.append(cardId);
    emit runDataChanged();
    emit itemAcquired(QString::fromUtf8(u8"你获得了卡牌：%1").arg(CardCatalog::byId(cardId).name));
}

void GameManager::addGold(int amount)
{
    if (amount <= 0) {
        return;
    }

    currentRun.gold += amount;
    emit runDataChanged();
    emit itemAcquired(QString::fromUtf8(u8"你获得了 %1 金币").arg(amount));
}

bool GameManager::addPotion(const QString &potionId)
{
    if (currentRun.potionIds.size() >= 3) {
        return false;
    }

    currentRun.potionIds.append(potionId);
    emit runDataChanged();
    emit itemAcquired(QString::fromUtf8(u8"你获得了药水：%1").arg(PotionCatalog::byId(potionId).name));
    return true;
}

bool GameManager::removePotionAt(int index)
{
    if (index < 0 || index >= currentRun.potionIds.size()) {
        return false;
    }

    currentRun.potionIds.removeAt(index);
    emit runDataChanged();
    return true;
}

bool GameManager::spendGold(int amount)
{
    if (amount < 0 || currentRun.gold < amount) {
        return false;
    }

    currentRun.gold -= amount;
    emit runDataChanged();
    return true;
}

void GameManager::heal(int amount)
{
    if (amount <= 0) {
        return;
    }

    currentRun.currentHp = qMin(currentRun.maxHp, currentRun.currentHp + amount);
    emit runDataChanged();
}

void GameManager::takeDamage(int amount)
{
    if (amount <= 0) {
        return;
    }

    currentRun.currentHp = qMax(0, currentRun.currentHp - amount);
    emit runDataChanged();
    if (currentRun.currentHp == 0) {
        emit playerDied();
    }
}

void GameManager::finishBattleVictory()
{
    if (currentRun.relicIds.contains(QStringLiteral("burning_blood"))) {
        currentRun.currentHp = qMin(currentRun.maxHp, currentRun.currentHp + 6);
    }

    emit runDataChanged();
}
