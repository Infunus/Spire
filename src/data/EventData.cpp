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
        return QString::fromUtf8("基础关");
    case EventKind::Treasure:
        return QString::fromUtf8("宝箱");
    case EventKind::BossBattle:
        return QString::fromUtf8("Boss 关");
    case EventKind::Merchant:
        return QString::fromUtf8("商人");
    case EventKind::RestSite:
        return QString::fromUtf8("休息点");
    case EventKind::FinalBoss:
        return QString::fromUtf8("最终 Boss");
    case EventKind::Completed:
        return QString::fromUtf8("已通关");
    case EventKind::Death:
        return QString::fromUtf8("死亡");
    }
    return QString();
}

EventData EventCatalog::eventForKind(EventKind kind)
{
    QString description;
    switch (kind) {
    case EventKind::NormalBattle:
        description = QString::fromUtf8("进入一场基础战斗。测试中点击攻击按钮击败敌人。");
        break;
    case EventKind::Treasure:
        description = QString::fromUtf8("发现一个宝箱。测试中需要点击退出事件，房间才会完成。");
        break;
    case EventKind::BossBattle:
        description = QString::fromUtf8("进入一场 Boss 战。胜利后会自动返回地图。");
        break;
    case EventKind::Merchant:
        description = QString::fromUtf8("商人出售遗物。购买后金币减少，状态栏遗物会更新。");
        break;
    case EventKind::RestSite:
        description = QString::fromUtf8("在营火旁休息。测试中点击退出事件后完成房间。");
        break;
    case EventKind::FinalBoss:
        description = QString::fromUtf8("塔尖的最终 Boss。胜利后地图节点完成。");
        break;
    case EventKind::Completed:
        description = QString::fromUtf8("这个房间已经完成。");
        break;
    case EventKind::Death:
        description = QString::fromUtf8("角色生命值降为 0，本次测试对局结束。");
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
        eventForKind(EventKind::FinalBoss),
        eventForKind(EventKind::Completed),
        eventForKind(EventKind::Death),
    };
}
