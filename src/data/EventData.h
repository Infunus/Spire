#ifndef EVENTDATA_H
#define EVENTDATA_H

#include <QString>
#include <vector>

// EventData.h
// Put map/event-room definitions and their icon mappings here.
// Example: EventKind::Merchant maps to camping-tent.png and opens ShopView.
// EventKind is the stable gameplay category; EventCatalog maps each kind to one icon.
// Example: EventKind::Completed maps to ink-swirl.png for the completed-room overlay.
// Do not put current map progress here; that belongs in RunData/MapView.
// Example: "floor 3 completed" is RunData, not EventData.

enum class EventKind {
    NormalBattle,
    Treasure,
    BossBattle,
    Merchant,
    RestSite,
    FinalBoss,
    Completed,
    Death
};

struct EventData {
    QString id;
    QString name;
    EventKind kind = EventKind::NormalBattle;
    QString iconPath;
    QString description;
};

class EventCatalog
{
public:
    static QString iconPath(EventKind kind);
    static QString displayName(EventKind kind);
    static EventData eventForKind(EventKind kind);
    static std::vector<EventData> mapRoomEvents();
};

#endif // EVENTDATA_H
