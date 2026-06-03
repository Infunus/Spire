#ifndef EVENTBUS_H
#define EVENTBUS_H

#include <QObject>

class EventBus : public QObject
{
    Q_OBJECT

public:
    static EventBus* instance();
    void notifyMapRequested();
    void notifyDeckRequested();
    void notifySettingsRequested();

signals:
    void mapRequested();
    void deckRequested();
    void settingsRequested();

private:
    explicit EventBus(QObject *parent = nullptr);
    static EventBus *s_instance;
};

#endif // EVENTBUS_H
