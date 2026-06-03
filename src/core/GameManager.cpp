#include "GameManager.h"
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
    emit runDataChanged();
}
