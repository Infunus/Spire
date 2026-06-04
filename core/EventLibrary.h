#ifndef EVENTLIBRARY_H
#define EVENTLIBRARY_H

#include "GameText.h"
#include "GameRandom.h"

#include <QList>
#include <QString>
#include <QStringList>

/*
 * 随机事件的填写说明，给后续新对话/新同学看的：
 *
 * 1. 一个事件只需要在这个文件里新增一个 createXxxEvent() 函数，不需要改 EventWidget，也不需要改 .pro。
 * 2. title 是事件标题；storyPages 是正文分段，EventWidget 会随着点击一段一段显示。
 * 3. choices 会在 storyPages 全部显示后出现。每个 choice 的 resultText 是选择后的后续描写。
 * 4. 玩家选完后不会立刻离开，而是先显示 resultText；再点“结束”按钮就直接回到地图/调试入口。
 * 5. imagePath 是左侧事件图接口，可以填 resources/events/xxx.png；为空时显示占位图框。
 * 6. 真正的数值效果暂时通过 effectType/amount/targetId 留接口，后续在 MainWindow 的 setChoiceHandler 里接 GameState。
 * 7. 新事件写完后，在 allEvents() 里加一行 events << createXxxEvent(); 即可进入随机池。
 */

// 随机事件的“填表入口”。
// 后续如果单独开新对话设计事件，优先让对方只改这个文件：
// 1. 复制下面的 createLibraryNightEvent()，改成 createYourEventName()。
// 2. 填 title/body/imagePath/choices。
// 3. 在 allEvents() 里追加 events << createYourEventName();
// 这样不用碰 EventWidget 的 UI 代码，也不用改 .pro 文件。

enum class EventEffectType
{
    None,           // 暂无真实效果，只显示选择结果。
    HealHp,         // 回复精力/HP，amount 表示回复量。
    LoseHp,         // 扣除精力/HP，amount 表示扣除量。
    GainCoins,      // 获得资源点，amount 表示获得量。
    AddCardById,    // 获得卡牌，targetId 填 CardLibrary 里的卡牌 id。
    GainRelicById,  // 获得圣遗物，targetId 填 RelicLibrary 里的圣遗物 id。
    GainPotionById  // 获得药水，targetId 填 PotionLibrary 里的药水 id。
};

struct RandomEventChoice
{
    QString text;        // 按钮上显示的选项文本。
    QString resultText;  // 点击后显示的结果文本；为空时默认显示“已选择：选项名”。

    // 下面这几个字段先作为接口预留。当前 MainWindow 暂时只完成“事件节点已完成”的结算，
    // 后续要接真实效果时，可以在 MainWindow 的 setChoiceHandler 里按 effectType 分支处理。
    EventEffectType effectType = EventEffectType::None;
    int amount = 0;
    QString targetId;

    // 给设计者看的备注，不直接显示。可以写“扣 8 HP 后给一张卡”这种自然语言。
    QString effectHint;
};

struct RandomEventData
{
    QString id;         // 事件唯一 id，建议英文小写加下划线，例如 library_night。
    QString title;      // 事件标题。
    QString body;       // 旧版单段正文兜底。新事件优先填写 storyPages。
    QStringList storyPages; // 剧情正文分段，EventWidget 会按点击顺序逐段显示。

    // 事件图接口。可以为空；也可以填 resources/events/xxx.png。
    // EventWidget 会尝试从运行目录、项目目录向上查找该相对路径。
    QString imagePath;

    QList<RandomEventChoice> choices;
};

namespace EventLibrary
{
inline RandomEventChoice makeChoice(const QString &text,
                                    const QString &resultText = QString(),
                                    EventEffectType effectType = EventEffectType::None,
                                    int amount = 0,
                                    const QString &targetId = QString(),
                                    const QString &effectHint = QString())
{
    RandomEventChoice choice;
    choice.text = text;
    choice.resultText = resultText;
    choice.effectType = effectType;
    choice.amount = amount;
    choice.targetId = targetId;
    choice.effectHint = effectHint;
    return choice;
}

inline RandomEventData makeEvent(const QString &id,
                                 const QString &title,
                                 const QStringList &storyPages,
                                 const QList<RandomEventChoice> &choices,
                                 const QString &imagePath = QString())
{
    RandomEventData event;
    event.id = id;
    event.title = title;
    event.storyPages = storyPages;
    event.body = storyPages.join(QStringLiteral("\n\n"));
    event.imagePath = imagePath;
    event.choices = choices;
    return event;
}

inline RandomEventData makeEvent(const QString &id,
                                 const QString &title,
                                 const QString &body,
                                 const QList<RandomEventChoice> &choices,
                                 const QString &imagePath = QString())
{
    QStringList pages;
    if (!body.isEmpty()) {
        pages << body;
    }
    return makeEvent(id, title, pages, choices, imagePath);
}

// 示例事件：保留给后续设计者参考格式。
// 真正扩展时可以复制这个函数，然后在 allEvents() 里追加。
inline RandomEventData createLibraryNightEvent()
{
    QStringList storyPages;
    storyPages << QStringLiteral("闭馆前的图书馆只剩下很浅的灯光。你在书架之间绕了半圈，发现一张写满公式的草稿纸夹在借阅机旁。")
               << QStringLiteral("草稿纸背面用铅笔写着一行字：如果今晚把这道题想明白，明天的高数作业也许就不会那么可怕。")
               << QStringLiteral("手机震了一下，宿舍群正在约夜宵。你看了看草稿纸，又看了看已经快要见底的精力。");

    QList<RandomEventChoice> choices;
    choices << makeChoice(GameText::EventText::previewChoiceA(),
                          QStringLiteral("你多撑了一会儿，脑子里多了一点解题手感。"),
                          EventEffectType::AddCardById,
                          0,
                          QString(),
                          QStringLiteral("后续可改成奖励一张学习主题卡牌。"))
            << makeChoice(GameText::EventText::previewChoiceB(),
                          QStringLiteral("你及时回宿舍休息，精神稍微恢复了一些。"),
                          EventEffectType::HealHp,
                          8,
                          QString(),
                          QStringLiteral("后续可接入回复 8 点 HP。"))
            << makeChoice(GameText::EventText::previewChoiceC(),
                          QStringLiteral("你买到了夜宵和补给，资源点增加。"),
                          EventEffectType::GainCoins,
                          12,
                          QString(),
                          QStringLiteral("后续可接入获得 12 资源点。"));

    return makeEvent(QStringLiteral("library_night"),
                     GameText::EventText::previewTitle(),
                     storyPages,
                     choices);
}

inline QList<RandomEventData> allEvents()
{
    QList<RandomEventData> events;
    events << createLibraryNightEvent();

    // 新事件统一加在这里，例如：
    // events << createDormPowerOutageEvent();
    // events << createScienceBuildingElevatorEvent();
    // events << createWeimingLakeWalkEvent();

    return events;
}

inline RandomEventData randomEvent()
{
    const QList<RandomEventData> events = allEvents();
    if (events.isEmpty()) {
        return RandomEventData();
    }
    return events.at(GameRandom::instance().bounded(events.size()));
}
}

#endif // EVENTLIBRARY_H
