#include "EventData.h"

QString EventCatalog::iconPath(EventKind kind)
{
    switch (kind) {
    case EventKind::NormalBattle:
        return QStringLiteral(":/pictures/resources/pictures/barbute.png");
    case EventKind::Treasure:
        return QStringLiteral(":/pictures/resources/pictures/locked-chest.png");
    case EventKind::BossBattle:
        return QStringLiteral(":/pictures/resources/pictures/brute.png");
    case EventKind::Merchant:
        return QStringLiteral(":/pictures/resources/pictures/camping-tent.png");
    case EventKind::RestSite:
        return QStringLiteral(":/pictures/resources/pictures/campfire.png");
    case EventKind::Question:
        return QStringLiteral(":/pictures/resources/pictures/circle-64.png");
    case EventKind::FinalBoss:
        return QStringLiteral(":/pictures/resources/pictures/crowned-skull.png");
    case EventKind::Completed:
        return QStringLiteral(":/pictures/resources/pictures/ink-swirl.png");
    case EventKind::Death:
        return QStringLiteral(":/pictures/resources/pictures/death-zone.png");
    }
    return QString();
}

QString EventCatalog::displayName(EventKind kind)
{
    switch (kind) {
    case EventKind::NormalBattle:
        return QString::fromUtf8(u8"基础关");
    case EventKind::Treasure:
        return QString::fromUtf8(u8"宝箱");
    case EventKind::BossBattle:
        return QString::fromUtf8(u8"Boss 关");
    case EventKind::Merchant:
        return QString::fromUtf8(u8"商人");
    case EventKind::RestSite:
        return QString::fromUtf8(u8"休息点");
    case EventKind::Question:
        return QString::fromUtf8(u8"？");
    case EventKind::FinalBoss:
        return QString::fromUtf8(u8"最终 Boss");
    case EventKind::Completed:
        return QString::fromUtf8(u8"已通关");
    case EventKind::Death:
        return QString::fromUtf8(u8"死亡");
    }
    return QString();
}

EventData EventCatalog::eventForKind(EventKind kind)
{
    QString description;
    switch (kind) {
    case EventKind::NormalBattle:
        description = QString::fromUtf8(u8"进入一场基础战斗。");
        break;
    case EventKind::Treasure:
        description = QString::fromUtf8(u8"发现一个宝箱。可以获得遗物、金币或药水。");
        break;
    case EventKind::BossBattle:
        description = QString::fromUtf8(u8"进入一场 Boss 战。胜利后会回到地图。");
        break;
    case EventKind::Merchant:
        description = QString::fromUtf8(u8"商人出售遗物、药水，并提供卡牌移除服务。");
        break;
    case EventKind::RestSite:
        description = QString::fromUtf8(u8"在营火旁休息。可以恢复生命，或锻造一张未升级的卡牌。");
        break;
    case EventKind::Question:
        description = QString::fromUtf8(u8"这里将来会出现随机事件。当前版本先作为占位文本，点击退出符号完成房间。");
        break;
    case EventKind::FinalBoss:
        description = QString::fromUtf8(u8"塔尖的最终 Boss。");
        break;
    case EventKind::Completed:
        description = QString::fromUtf8(u8"这个房间已经完成。");
        break;
    case EventKind::Death:
        description = QString::fromUtf8(u8"角色生命值降为 0，本次对局结束。");
        break;
    }

    return {
        QString::number(static_cast<int>(kind)),
        displayName(kind),
        kind,
        iconPath(kind),
        description
    };
}

std::vector<EventData> EventCatalog::mapRoomEvents()
{
    return {
        eventForKind(EventKind::NormalBattle),
        eventForKind(EventKind::Treasure),
        eventForKind(EventKind::BossBattle),
        eventForKind(EventKind::Merchant),
        eventForKind(EventKind::RestSite),
        eventForKind(EventKind::Question),
        eventForKind(EventKind::FinalBoss),
        eventForKind(EventKind::Completed),
        eventForKind(EventKind::Death),
    };
}
