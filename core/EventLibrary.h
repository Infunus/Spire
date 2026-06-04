#ifndef EVENTLIBRARY_H
#define EVENTLIBRARY_H

#include "CardLibrary.h"
#include "GameState.h"
#include "GameText.h"
#include "GameRandom.h"
#include "Potion.h"
#include "Relic.h"

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
    HealHp,         // 回复心情，amount 表示回复量。代码名沿用 Hp，显示时统一叫“心情”。
    LoseHp,         // 扣除心情，amount 表示扣除量。代码名沿用 Hp，显示时统一叫“心情”。
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

    // 给设计者看的备注，不直接显示。可以写“扣 8 心情后给一张卡”这种自然语言。
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
inline bool applyChoiceEffect(const RandomEventChoice &choice)
{
    GameState &state = GameState::instance();

    switch (choice.effectType) {
    case EventEffectType::HealHp:
        state.heal(choice.amount);
        return choice.amount > 0;
    case EventEffectType::LoseHp:
        state.loseHp(choice.amount);
        return choice.amount > 0;
    case EventEffectType::GainCoins:
        state.addCoins(choice.amount);
        return choice.amount > 0;
    case EventEffectType::AddCardById: {
        const Card card = CardLibrary::byId(choice.targetId);
        if (card.id().isEmpty()) {
            return false;
        }
        state.addCard(card);
        return true;
    }
    case EventEffectType::GainRelicById:
        if (choice.targetId.isEmpty()) {
            return false;
        }
        state.addRelicById(choice.targetId);
        return true;
    case EventEffectType::GainPotionById:
        if (choice.targetId.isEmpty()) {
            return false;
        }
        return state.addPotionById(choice.targetId);
    case EventEffectType::None:
        return false;
    }

    return false;
}

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
               << QStringLiteral("手机震了一下，宿舍群正在约夜宵。你看了看草稿纸，又看了看已经快要见底的心情。");

    QList<RandomEventChoice> choices;
    choices << makeChoice(GameText::EventText::previewChoiceA(),
                          QStringLiteral("你多撑了一会儿，脑子里多了一点解题手感。"),
                          EventEffectType::AddCardById,
                          0,
                          CardIds::pommel(),
                          QStringLiteral("获得卡牌：我觉得我会了。"))
            << makeChoice(GameText::EventText::previewChoiceB(),
                          QStringLiteral("你及时回宿舍休息，心情稍微恢复了一些。"),
                          EventEffectType::HealHp,
                          8,
                          QString(),
                          QStringLiteral("回复 8 点心情。"))
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

inline RandomEventData createDormPowerOutageEvent()
{
    QStringList storyPages;
    storyPages << QStringLiteral("宿舍楼忽然暗了下来，走廊里响起一阵拉长的叹气声。窗外的路灯还亮着，桌上的草稿纸只剩下一角能看清。")
               << QStringLiteral("群里有人说电工已经在路上，也有人开始讨论要不要趁机溜去夜宵。你摸到台灯，又摸到枕头。")
               << QStringLiteral("这段突如其来的安静，像一道没有标准答案的选择题。");

    QList<RandomEventChoice> choices;
    choices << makeChoice(QStringLiteral("借台灯继续整理思路"),
                          QStringLiteral("你把台灯夹在床栏上，艰难地补完了半页推导。虽然眼睛有点酸，思路却顺了起来。"),
                          EventEffectType::AddCardById,
                          0,
                          CardIds::inflame(),
                          QStringLiteral("获得卡牌：思路打开。"))
            << makeChoice(QStringLiteral("顺势早睡"),
                          QStringLiteral("你把手机扣在桌上，决定让明天的自己接手。醒来时，心情恢复了一截。"),
                          EventEffectType::HealHp,
                          10,
                          QString(),
                          QStringLiteral("回复 10 点心情。"))
            << makeChoice(QStringLiteral("下楼买应急补给"),
                          QStringLiteral("便利店门口排起了小队。你带回一点吃的和一瓶临时补给，宿舍也刚好来电。"),
                          EventEffectType::GainPotionById,
                          0,
                          PotionIds::clinicSyrup(),
                          QStringLiteral("获得药水：校医院糖浆。"));

    return makeEvent(QStringLiteral("dorm_power_outage"),
                     QStringLiteral("宿舍停电"),
                     storyPages,
                     choices);
}

inline RandomEventData createScienceBuildingElevatorEvent()
{
    QStringList storyPages;
    storyPages << QStringLiteral("理科楼电梯口挤满了人，数字停在一个很不讲道理的楼层。上课铃还没响，但空气里已经有了迟到的味道。")
               << QStringLiteral("有人冲向楼梯，有人原地等候，还有人掏出咖啡开始和命运谈判。")
               << QStringLiteral("你看了一眼课表：教室在顶层。");

    QList<RandomEventChoice> choices;
    choices << makeChoice(QStringLiteral("爬楼梯冲上去"),
                          QStringLiteral("你一路爬到顶层，推门时刚好赶上点名。心跳很快，但手里的书包变得更可靠了。"),
                          EventEffectType::GainRelicById,
                          0,
                          RelicIds::packedSchoolbag(),
                          QStringLiteral("获得圣遗物：满载书包。"))
            << makeChoice(QStringLiteral("继续等电梯"),
                          QStringLiteral("电梯终于抵达，你被人群平稳地送上楼。时间被消耗掉一点，心情也跟着掉了一些。"),
                          EventEffectType::LoseHp,
                          5,
                          QString(),
                          QStringLiteral("失去 5 点心情。"))
            << makeChoice(QStringLiteral("先买咖啡再说"),
                          QStringLiteral("你拎着咖啡冲进教室，精神像被强行重启。今天至少还能多撑一会儿。"),
                          EventEffectType::GainPotionById,
                          0,
                          PotionIds::coffeeShot(),
                          QStringLiteral("获得药水：咖啡因冲剂。"));

    return makeEvent(QStringLiteral("science_building_elevator"),
                     QStringLiteral("理科楼电梯"),
                     storyPages,
                     choices);
}

inline RandomEventData createWeimingLakeWalkEvent()
{
    QStringList storyPages;
    storyPages << QStringLiteral("未名湖边的风把一天的噪声吹薄了。水面映着博雅塔，像一页还没写完的结课报告。")
               << QStringLiteral("你本来只是想绕一圈散心，却在长椅旁看见一张被夹在书里的讲座票根。")
               << QStringLiteral("湖边很安静，安静到足够让人重新决定今晚该怎么过。");

    QList<RandomEventChoice> choices;
    choices << makeChoice(QStringLiteral("沿湖复盘错题"),
                          QStringLiteral("你边走边把卡住的地方重新讲给自己听。走到湖对岸时，那道题终于不再像一堵墙。"),
                          EventEffectType::AddCardById,
                          0,
                          CardIds::heavyBlade(),
                          QStringLiteral("获得卡牌：这就是心流？"))
            << makeChoice(QStringLiteral("坐下放空一会儿"),
                          QStringLiteral("你没有打开任何软件，只是看着湖面发了一会儿呆。压力慢慢沉下去，呼吸也顺了。"),
                          EventEffectType::HealHp,
                          12,
                          QString(),
                          QStringLiteral("回复 12 点心情。"))
            << makeChoice(QStringLiteral("收好那张票根"),
                          QStringLiteral("票根背面写着百讲折扣信息。也许下次去商店时，它能帮你省下一点资源。"),
                          EventEffectType::GainRelicById,
                          0,
                          RelicIds::luckyCoupon(),
                          QStringLiteral("获得圣遗物：百讲折扣券。"));

    return makeEvent(QStringLiteral("weiming_lake_walk"),
                     QStringLiteral("未名湖夜行"),
                     storyPages,
                     choices);
}

inline QList<RandomEventData> allEvents()
{
    QList<RandomEventData> events;
    events << createLibraryNightEvent()
           << createDormPowerOutageEvent()
           << createScienceBuildingElevatorEvent()
           << createWeimingLakeWalkEvent();

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
