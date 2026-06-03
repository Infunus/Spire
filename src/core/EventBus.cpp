#include "EventBus.h"
#include <QCoreApplication>

EventBus* EventBus::s_instance = nullptr;

EventBus* EventBus::instance()
{
    if (!s_instance) {
        s_instance = new EventBus(qApp);
    }
    return s_instance;
}

EventBus::EventBus(QObject *parent)
    : QObject(parent)
{
}

void EventBus::notifyMapRequested()
{
    emit mapRequested();
}

void EventBus::notifyDeckRequested()
{
    emit deckRequested();
}

void EventBus::notifySettingsRequested()
{
    emit settingsRequested();
}
