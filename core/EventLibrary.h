#ifndef EVENTLIBRARY_H
#define EVENTLIBRARY_H

#include "CardLibrary.h"
#include "GameBalance.h"
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
 * 5. imagePath 是左侧事件图接口；不手动填写时会默认使用 resources/events/<事件id>.png。
 * 6. 新建事件时同步生成一张事件图，保存为 resources/events/<事件id>.png。
 * 7. 真正的数值效果暂时通过 effectType/amount/targetId 留接口，后续在 MainWindow 的 setChoiceHandler 里接 GameState。
 * 8. 新事件写完后，在 allEvents() 里加一行 events << createXxxEvent(); 即可进入随机池。
 */

// 随机事件的“填表入口”。
// 后续如果单独开新对话设计事件，优先让对方只改这个文件：
// 1. 复制下面的 createLibraryNightEvent()，改成 createYourEventName()。
// 2. 填 title/body/imagePath/choices。
// 3. 用 image2 生成事件图，保存到 resources/events/your_event_id.png。
// 4. 在 allEvents() 里追加 events << createYourEventName();
// 这样不用碰 EventWidget 的 UI 代码，也不用改 .pro 文件。

enum class EventEffectType
{
    None,                    // 暂无真实效果，只显示选择结果。
    HealHp,                  // 回复心情，amount 表示回复量。代码名沿用 Hp，显示时统一叫“心情”。
    LoseHp,                  // 扣除心情，amount 表示扣除量。代码名沿用 Hp，显示时统一叫“心情”。
    GainCoins,               // 调整 Money，amount 为正数时获得，为负数时扣除。
    GainUsualScore,          // 获得平时分，amount 表示获得量。
    GainNextBattleStrength,  // 下一场战斗开局获得效率值，amount 表示获得量。
    GainNextBattleTurnBlock, // 下一场战斗每回合开始获得抗压值，amount 表示获得量。
    AddCardById,             // 获得卡牌，targetId 填 CardLibrary 里的卡牌 id，amount 可填获得张数。
    GainRelicById,           // 获得圣遗物，targetId 填 RelicLibrary 里的圣遗物 id。
    GainPotionById,          // 获得饮料，targetId 填 PotionLibrary 里的饮料 id。
    GainRandomPotion         // 获得随机饮料，amount 表示获得数量。
};

struct RandomEventEffect
{
    EventEffectType effectType = EventEffectType::None;
    int amount = 0;
    QString targetId;
};

struct RandomEventChoice
{
    QString text;       // 按钮上显示的选项文本。
    QString resultText; // 点击后显示的结果文本；为空时默认显示“已选择：选项名”。

    // 单效果快捷字段。多数事件直接填这三项即可；需要多个效果时用 effects。
    EventEffectType effectType = EventEffectType::None;
    int amount = 0;
    QString targetId;
    QList<RandomEventEffect> effects;

    // 给设计者看的备注，不直接显示。可以写“扣 8 心情后给一张卡”这种自然语言。
    QString effectHint;
    bool showEffectSummary = true;
};

struct RandomEventData
{
    QString id;             // 事件唯一 id，建议英文小写加下划线，例如 library_night。
    QString title;          // 事件标题。
    QString body;           // 旧版单段正文兜底。新事件优先填写 storyPages。
    QStringList storyPages; // 剧情正文分段，EventWidget 会按点击顺序逐段显示。

    // 事件图接口。通过 makeEvent() 创建时，默认是 resources/events/<事件id>.png。
    // EventWidget 会尝试从运行目录、项目目录向上查找该相对路径。
    QString imagePath;

    QList<RandomEventChoice> choices;
};

namespace EventLibrary
{
    inline QString defaultEventImagePath(const QString &id)
    {
        return id.isEmpty() ? QString() : QStringLiteral("resources/events/%1.png").arg(id);
    }

    inline RandomEventEffect makeEffect(EventEffectType effectType = EventEffectType::None,
                                        int amount = 0,
                                        const QString &targetId = QString())
    {
        RandomEventEffect effect;
        effect.effectType = effectType;
        effect.amount = amount;
        effect.targetId = targetId;
        return effect;
    }

    inline QList<RandomEventEffect> choiceEffects(const RandomEventChoice &choice)
    {
        if (!choice.effects.isEmpty())
        {
            return choice.effects;
        }

        QList<RandomEventEffect> effects;
        if (choice.effectType != EventEffectType::None)
        {
            effects << makeEffect(choice.effectType, choice.amount, choice.targetId);
        }
        return effects;
    }

    inline QString cardCountText(int count)
    {
        if (count <= 1)
        {
            return QStringLiteral("一张");
        }
        if (count == 2)
        {
            return QStringLiteral("两张");
        }
        return QStringLiteral("%1 张").arg(count);
    }

    inline QString effectSummaryLine(const RandomEventEffect &effect)
    {
        switch (effect.effectType)
        {
        case EventEffectType::HealHp:
            return effect.amount > 0
                       ? QStringLiteral("心情 +%1。").arg(effect.amount)
                       : QString();
        case EventEffectType::LoseHp:
            return effect.amount > 0
                       ? QStringLiteral("心情 -%1。").arg(effect.amount)
                       : QString();
        case EventEffectType::GainCoins:
            if (effect.amount > 0)
            {
                return QStringLiteral("Money +%1。").arg(effect.amount);
            }
            if (effect.amount < 0)
            {
                return QStringLiteral("Money -%1。").arg(-effect.amount);
            }
            return QString();
        case EventEffectType::GainUsualScore:
            return effect.amount > 0
                       ? QStringLiteral("平时分 +%1。").arg(effect.amount)
                       : QString();
        case EventEffectType::GainNextBattleStrength:
            return effect.amount > 0
                       ? QStringLiteral("下回合开始时，效率值 +%1。").arg(effect.amount)
                       : QString();
        case EventEffectType::GainNextBattleTurnBlock:
            return effect.amount > 0
                       ? QStringLiteral("下场战斗每回合开始时，抗压值 +%1。").arg(effect.amount)
                       : QString();
        case EventEffectType::AddCardById:
        {
            const int count = effect.amount > 0 ? effect.amount : 1;
            return QStringLiteral("获得%1卡牌。").arg(cardCountText(count));
        }
        case EventEffectType::GainRelicById:
        {
            const RelicData relic = RelicLibrary::findById(effect.targetId);
            if (relic.id.isEmpty())
            {
                return QString();
            }
            if (GameState::instance().hasRelic(relic.id))
            {
                return QStringLiteral("已拥有圣遗物 %1，不会重复获得。").arg(relic.name);
            }
            if (relic.description.isEmpty())
            {
                return QStringLiteral("获得圣遗物 %1。").arg(relic.name);
            }
            return QStringLiteral("获得圣遗物 %1：%2。").arg(relic.name, relic.description);
        }
        case EventEffectType::GainPotionById:
        {
            const PotionData potion = PotionLibrary::findById(effect.targetId);
            if (potion.id.isEmpty())
            {
                return QString();
            }
            if (GameState::instance().potions().size() >= GameState::instance().maxPotions())
            {
                return QStringLiteral("饮料栏已满，无法获得饮料 %1。").arg(potion.name);
            }
            return QStringLiteral("获得饮料 %1。").arg(potion.name);
        }
        case EventEffectType::GainRandomPotion:
        {
            const QList<PotionData> potions = PotionLibrary::allPotions();
            if (potions.isEmpty())
            {
                return QString();
            }

            const int count = effect.amount > 0 ? effect.amount : 1;
            if (GameState::instance().potions().size() >= GameState::instance().maxPotions())
            {
                return QStringLiteral("饮料栏已满，无法获得随机饮料 %1 瓶。").arg(count);
            }
            return QStringLiteral("获得随机饮料 %1 瓶。").arg(count);
        }
        case EventEffectType::None:
            return QString();
        }

        return QString();
    }

    inline QString effectSummaryLine(const RandomEventChoice &choice)
    {
        const QList<RandomEventEffect> effects = choiceEffects(choice);
        if (effects.isEmpty())
        {
            return QString();
        }

        QStringList lines;
        for (const RandomEventEffect &effect : effects)
        {
            const QString line = effectSummaryLine(effect);
            if (!line.isEmpty())
            {
                lines << line;
            }
        }
        return lines.join(QStringLiteral("\n"));
    }

    inline QString choiceEffectSummary(const RandomEventChoice &choice, bool)
    {
        if (!choice.showEffectSummary)
        {
            return QString();
        }

        QStringList lines;
        const QString choiceEffect = effectSummaryLine(choice);
        if (!choiceEffect.isEmpty())
        {
            lines << choiceEffect;
        }

        return lines.join(QStringLiteral("\n"));
    }

    inline bool applyEffect(const RandomEventEffect &effect, bool applyCardEffects = true)
    {
        GameState &state = GameState::instance();

        switch (effect.effectType)
        {
        case EventEffectType::HealHp:
            state.heal(effect.amount);
            return effect.amount > 0;
        case EventEffectType::LoseHp:
            state.loseHp(effect.amount);
            return effect.amount > 0;
        case EventEffectType::GainCoins:
            state.addCoins(effect.amount);
            return effect.amount != 0;
        case EventEffectType::GainUsualScore:
            state.addUsualScore(effect.amount);
            return effect.amount > 0;
        case EventEffectType::GainNextBattleStrength:
            state.addNextBattleStartStrength(effect.amount);
            return effect.amount > 0;
        case EventEffectType::GainNextBattleTurnBlock:
            state.addNextBattleTurnBlock(effect.amount);
            return effect.amount > 0;
        case EventEffectType::AddCardById:
        {
            if (!applyCardEffects || effect.targetId.isEmpty())
            {
                return false;
            }
            const Card card = CardLibrary::byId(effect.targetId);
            if (card.id().isEmpty())
            {
                return false;
            }
            const int count = effect.amount > 0 ? effect.amount : 1;
            for (int i = 0; i < count; ++i)
            {
                state.addCard(card);
            }
            return true;
        }
        case EventEffectType::GainRelicById:
        {
            const RelicData relic = RelicLibrary::findById(effect.targetId);
            if (relic.id.isEmpty() || state.hasRelic(relic.id))
            {
                return false;
            }
            state.addRelic(relic);
            return true;
        }
        case EventEffectType::GainPotionById:
            if (effect.targetId.isEmpty())
            {
                return false;
            }
            return state.addPotionById(effect.targetId);
        case EventEffectType::GainRandomPotion:
        {
            const QList<PotionData> potions = PotionLibrary::allPotions();
            if (potions.isEmpty())
            {
                return false;
            }

            bool applied = false;
            const int count = effect.amount > 0 ? effect.amount : 1;
            for (int i = 0; i < count && state.potions().size() < state.maxPotions(); ++i)
            {
                const PotionData potion = potions.at(GameRandom::instance().bounded(potions.size()));
                applied = state.addPotion(potion) || applied;
            }
            return applied;
        }
        case EventEffectType::None:
            return false;
        }

        return false;
    }

    inline bool applyChoiceEffect(const RandomEventChoice &choice, bool applyCardEffects = true)
    {
        bool applied = false;
        const QList<RandomEventEffect> effects = choiceEffects(choice);
        for (const RandomEventEffect &effect : effects)
        {
            applied = applyEffect(effect, applyCardEffects) || applied;
        }
        return applied;
    }

    inline bool choiceHasCardReward(const RandomEventChoice &choice)
    {
        const QList<RandomEventEffect> effects = choiceEffects(choice);
        for (const RandomEventEffect &effect : effects)
        {
            if (effect.effectType == EventEffectType::AddCardById)
            {
                return true;
            }
        }
        return false;
    }

    inline QString cardRewardTargetId(const RandomEventChoice &choice)
    {
        const QList<RandomEventEffect> effects = choiceEffects(choice);
        for (const RandomEventEffect &effect : effects)
        {
            if (effect.effectType == EventEffectType::AddCardById)
            {
                return effect.targetId;
            }
        }
        return QString();
    }

    inline int cardRewardCount(const RandomEventChoice &choice)
    {
        int count = 0;
        const QList<RandomEventEffect> effects = choiceEffects(choice);
        for (const RandomEventEffect &effect : effects)
        {
            if (effect.effectType == EventEffectType::AddCardById)
            {
                count += effect.amount > 0 ? effect.amount : 1;
            }
        }
        return count;
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
        if (effectType != EventEffectType::None)
        {
            choice.effects << makeEffect(effectType, amount, targetId);
        }
        choice.effectHint = effectHint;
        return choice;
    }

    inline RandomEventChoice makeChoiceWithEffects(const QString &text,
                                                   const QString &resultText,
                                                   const QList<RandomEventEffect> &effects,
                                                   const QString &effectHint = QString())
    {
        RandomEventChoice choice;
        choice.text = text;
        choice.resultText = resultText;
        choice.effects = effects;
        choice.effectHint = effectHint;

        for (const RandomEventEffect &effect : effects)
        {
            if (effect.effectType != EventEffectType::None)
            {
                choice.effectType = effect.effectType;
                choice.amount = effect.amount;
                choice.targetId = effect.targetId;
                break;
            }
        }

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
        event.imagePath = imagePath.isEmpty() ? defaultEventImagePath(id) : imagePath;
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
        if (!body.isEmpty())
        {
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
                              GameBalance::Events::libraryNightHeal(),
                              QString(),
                              QStringLiteral("回复心情。"))
                << makeChoice(GameText::EventText::previewChoiceC(),
                              QStringLiteral("你买到了夜宵和补给，余额增加。"),
                              EventEffectType::GainCoins,
                              GameBalance::Events::libraryNightCoins(),
                              QString(),
                              QStringLiteral("获得 Money。"));

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
                              GameBalance::Events::dormPowerOutageHeal(),
                              QString(),
                              QStringLiteral("回复心情。"))
                << makeChoice(QStringLiteral("下楼买应急补给"),
                              QStringLiteral("便利店门口排起了小队。你带回一点吃的和一瓶临时补给，宿舍也刚好来电。"),
                              EventEffectType::GainPotionById,
                              0,
                              PotionIds::clinicSyrup(),
                              QStringLiteral("获得饮料 可口可乐。"));

        return makeEvent(QStringLiteral("dorm_power_outage"),
                         QStringLiteral("宿舍停电"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createScienceBuildingElevatorEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("理教电梯口挤满了人，数字停在一个很不讲道理的楼层。上课铃还没响，但空气里已经有了迟到的味道。")
                   << QStringLiteral("有人冲向楼梯，有人原地等候，还有人掏出咖啡开始和命运谈判。")
                   << QStringLiteral("你看了一眼课表：教室在顶层。");

        QList<RandomEventChoice> choices;
        choices << makeChoice(QStringLiteral("爬楼梯冲上去"),
                              QStringLiteral("你一路往上冲，路上还看到一个袋子，大概是同样心急如焚的人留下的吧"),
                              EventEffectType::GainRelicById,
                              0,
                              RelicIds::packedSchoolbag(),
                              QStringLiteral("获得圣遗物：红色的帆布袋。"))
                << makeChoice(QStringLiteral("继续等电梯"),
                              QStringLiteral("电梯终于抵达，你被人群平稳地送上楼。时间被消耗掉一点，心情也跟着掉了一些。"),
                              EventEffectType::LoseHp,
                              GameBalance::Events::elevatorWaitLoseHp(),
                              QString(),
                              QStringLiteral("失去心情。"))
                << makeChoice(QStringLiteral("先买咖啡再说"),
                              QStringLiteral("你拎着咖啡冲进教室，精神像被强行重启。今天至少还能多撑一会儿。"),
                              EventEffectType::GainPotionById,
                              0,
                              PotionIds::coffeeShot(),
                              QStringLiteral("获得饮料 抹茶拿铁。"));

        return makeEvent(QStringLiteral("science_building_elevator"),
                         QStringLiteral("理教电梯口"),
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
                              GameBalance::Events::weimingLakeRestHeal(),
                              QString(),
                              QStringLiteral("回复心情。"))
                << makeChoice(QStringLiteral("收好那张票根"),
                              QStringLiteral("票根背面写着物美会员信息。也许下次去超市时，它能帮你省下一点 Money。"),
                              EventEffectType::GainRelicById,
                              0,
                              RelicIds::luckyCoupon(),
                              QStringLiteral("获得圣遗物：物美会员卡。"));

        return makeEvent(QStringLiteral("weiming_lake_walk"),
                         QStringLiteral("未名湖夜行"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createStudentAffairsNewTaskEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("#1229139\n学工群里跳出一条新消息。部长发来一个新锅，后面跟着一句熟悉的“大家看看谁方便接一下”。")
                   << QStringLiteral("你正坐在桌边吃小番茄。屏幕亮起的那一刻，群聊安静得像被按下了暂停键，手里的小番茄也忽然显得危险起来。")
                   << QStringLiteral("你咽下嘴里的小番茄，盯着输入框，开始判断这口锅有没有自然蒸发的可能。");

        QList<RandomEventEffect> volunteerEffects;
        volunteerEffects << makeEffect(EventEffectType::LoseHp,
                                       GameBalance::Events::studentAffairsVolunteerLoseHp())
                         << makeEffect(EventEffectType::AddCardById);

        QList<RandomEventChoice> choices;
        choices << makeChoice(QStringLiteral("不回复，看看群里会不会有人领锅"),
                              QStringLiteral("你继续吃小番茄，打算先观察一下局势。结果一颗小番茄的汁水溅到了手机屏幕上。\n\n你手忙脚乱地擦干手机，才发现擦拭过程中误触了输入框，往群里连发了五十多条“突突突”。群里陷入了可怕的沉默。最终，你不得不把这个锅领了下来。"),
                              EventEffectType::LoseHp,
                              GameBalance::Events::studentAffairsWaitLoseHp(),
                              QString(),
                              QStringLiteral("失去心情。"))
                << makeChoice(QStringLiteral("不回复，干其他事"),
                              QStringLiteral("你熟练地无视了部长，把手机翻面，去干其他事了。\n\n等你再打开群聊时，这口锅已经被后来的勇者领走。无事发生。"))
                << makeChoiceWithEffects(QStringLiteral("主动领锅"),
                                         QStringLiteral("你叹了口气，在群里回了一句“我来吧”。事情确实有点累，但推进过程中你摸清了一套新的工作流程，也算把经验装进了牌组。"),
                                         volunteerEffects,
                                         QStringLiteral("失去心情，并获得一张卡牌。"));

        return makeEvent(QStringLiteral("student_affairs_new_task"),
                         QStringLiteral("小番茄突突突"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createOpenDayEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("又到了一年一度的开放日。校门口挂起横幅，路边站满指引牌，校园忽然变成了一张会走路的宣传册。")
                   << QStringLiteral("主路上到处都是游客、相机和被反复问路的同学。你看着窗外越来越热闹的人群，意识到今天想安静度过并不容易。")
                   << QStringLiteral("为了度过这艰难的一天，你决定：");

        QList<RandomEventEffect> goOutEffects;
        goOutEffects << makeEffect(EventEffectType::HealHp,
                                   GameBalance::Events::openDayGoOutHeal())
                     << makeEffect(EventEffectType::GainRandomPotion,
                                   GameBalance::Events::openDayRandomPotionCount());

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("和朋友出去玩"),
                                         QStringLiteral("你和朋友们迅速达成一致，果断逃离校园。\n\n一行人在外面吃饭、闲逛、聊天，把开放日过成了难得的休息日。回来的路上，你还顺手带回了一瓶随机饮料。"),
                                         goOutEffects,
                                         QStringLiteral("回复心情，并获得随机饮料。"))
                << makeChoice(QStringLiteral("开放日无法阻止我去图书馆自习！"),
                              QStringLiteral("你抱着书钻进图书馆，试图用自律对抗开放日。可游客不断从身边经过，有人围观，有人拍照，你根本没办法集中注意力。\n\n更杀人诛心的是，朋友给你发来一张社交平台截图：有人发了图书馆照片，而你正好被拍了下来。"),
                              EventEffectType::LoseHp,
                              GameBalance::Events::openDayStudyLoseHp(),
                              QString(),
                              QStringLiteral("失去心情。"))
                << makeChoice(QStringLiteral("你说得对，但我是志愿者"),
                              QStringLiteral("你在指定位置执行工作，努力维持开放日应有的秩序。这时，一个家长带着孩子走到你身边。\n\n她们在你身边的树下停住。然后，她带着孩子开始上厕所……\n\n你无力地看着这一切，最后发了树洞吐槽。虽然你很无助，但你给大家带来了乐子。"),
                              EventEffectType::LoseHp,
                              GameBalance::Events::openDayVolunteerLoseHp(),
                              QString(),
                              QStringLiteral("失去心情。"));

        return makeEvent(QStringLiteral("open_day"),
                         QStringLiteral("开放日"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createIdeologyPreEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("思政课群里忽然弹出一条通知：下周要做 Pre。")
                   << QStringLiteral("你盯着屏幕看了几秒，长叹一声。PPT、分工、排练、上台，几个词轮流从脑子里冒出来。")
                   << QStringLiteral("这项任务总得有人开始。你开始思考该怎么办。");

        QList<RandomEventEffect> seriousEffects;
        seriousEffects << makeEffect(EventEffectType::LoseHp,
                                     GameBalance::Events::ideologyPreSeriousLoseHp())
                       << makeEffect(EventEffectType::GainUsualScore,
                                     GameBalance::Events::ideologyPreSeriousUsualScore());

        QList<RandomEventEffect> friendsEffects;
        friendsEffects << makeEffect(EventEffectType::LoseHp,
                                     GameBalance::Events::ideologyPreFriendsLoseHp())
                       << makeEffect(EventEffectType::GainUsualScore,
                                     GameBalance::Events::ideologyPreFriendsUsualScore());

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("立即开始找人，认认真真完成这项任务"),
                                         QStringLiteral("你打开聊天列表，开始一个个拉人、确认分工、整理资料。\n\n过程确实有点累，但你们最后交出了一份相当完整的展示。你的付出获得了回报。"),
                                         seriousEffects,
                                         QStringLiteral("失去心情，并获得平时分。"))
                << makeChoice(QStringLiteral("还是开摆吧……"),
                              QStringLiteral("你决定先把这件事从脑子里挪开，等局势自己发展。\n\n最后你跟着别人的队伍，勉强混完了 Pre。虽然没有出大问题，但心情还是被这段悬而未决的等待磨掉了一点。"),
                              EventEffectType::LoseHp,
                              GameBalance::Events::ideologyPreCoastLoseHp(),
                              QString(),
                              QStringLiteral("失去心情。"))
                << makeChoiceWithEffects(QStringLiteral("不好意思，我有不少朋友"),
                                         QStringLiteral("你熟练地在几个群里发出邀请，很快凑齐了一支还算靠谱的小队。\n\n朋友们一起分工、互相兜底，共同完成了这次任务。虽然还是耗了一点精力，但事情推进得顺利多了。"),
                                         friendsEffects,
                                         QStringLiteral("失去心情，并获得平时分。"));

        return makeEvent(QStringLiteral("ideology_pre"),
                         QStringLiteral("思政课 Pre"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createGaokaoBlessingVideoEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("临近高考，高中老师忽然联系到你，希望你拍一段祝福视频。")
                   << QStringLiteral("消息里写着“给学弟学妹们加加油就好”，语气很温和，却把你一下子拽回了高三。")
                   << QStringLiteral("你看了看自己的期末安排，又看了看老师发来的拍摄要求，开始思考该怎么回复。");

        QList<RandomEventEffect> shootEffects;
        shootEffects << makeEffect(EventEffectType::LoseHp,
                                   GameBalance::Events::gaokaoBlessingShootLoseHp())
                     << makeEffect(EventEffectType::AddCardById);

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("去拍"),
                                         QStringLiteral("你来到湖边，对着镜头为学弟学妹们录制了祝福视频。\n\n说着说着，你不由得回想起过去。那些曾经觉得过不去的日子，如今已经变成了可以讲给别人听的经验。一时间思绪万千，你有了新的感悟。"),
                                         shootEffects,
                                         QStringLiteral("失去心情，并获得一张卡牌。"))
                << makeChoice(QStringLiteral("婉拒"),
                              QStringLiteral("他们要高考，我也要期末了啊！你看着还没动工的程设大作业，意识到自己实在分身乏术。\n\n你婉拒了老师的请求，屏蔽掉这次干扰。重新坐回电脑前时，你感觉自己变得更加专注了。"),
                              EventEffectType::GainNextBattleStrength,
                              GameBalance::Events::gaokaoBlessingFocusStrength(),
                              QString(),
                              QStringLiteral("下回合开始时获得效率值。"))
                << makeChoice(QStringLiteral("装病"),
                              QStringLiteral("你假装自己生病了，不得不拒绝拍摄。\n\n本来一切看上去都很顺利，直到同学朋友圈里的一张照片出卖了你。老师没有多说什么，但你已经开始心虚了。"),
                              EventEffectType::LoseHp,
                              GameBalance::Events::gaokaoBlessingFakeSickLoseHp(),
                              QString(),
                              QStringLiteral("失去心情。"));

        return makeEvent(QStringLiteral("gaokao_blessing_video"),
                         QStringLiteral("高考祝福视频"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createClubFairEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("又到了百团大战的时候。")
                   << QStringLiteral("社团招新摊位一字排开。你本来只想路过，结果已经有人把报名表递到了你手里。")
                   << QStringLiteral("（以下仅列出部分我们了解的社团）");

        QList<RandomEventEffect> yuanhuoEffects;
        yuanhuoEffects << makeEffect(EventEffectType::HealHp,
                                     GameBalance::Events::clubFairYuanhuoHeal())
                       << makeEffect(EventEffectType::AddCardById,
                                     GameBalance::Events::clubFairYuanhuoCardCount());

        QList<RandomEventEffect> railwayEffects;
        railwayEffects << makeEffect(EventEffectType::HealHp,
                                     GameBalance::Events::clubFairRailwayHeal())
                       << makeEffect(EventEffectType::AddCardById,
                                     GameBalance::Events::clubFairRailwayCardCount());

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("加入元火"),
                                         QStringLiteral("在元火，你和同好们愉快交流，收获颇丰。"),
                                         yuanhuoEffects,
                                         QStringLiteral("回复心情，并获得两张卡牌。"))
                << makeChoiceWithEffects(QStringLiteral("加入铁协"),
                                         QStringLiteral("作为光荣的铁协成员，你在耳濡目染下，完整地背下了北京地铁图！\n\n从此以后，你就是朋友们的人形导航。"),
                                         railwayEffects,
                                         QStringLiteral("回复心情，并获得一张卡牌。"))
                << makeChoice(QStringLiteral("加入青天"),
                              QStringLiteral("你接过社团资料，认真听完了学长学姐的介绍。\n\n他们送给你一本青天观测指南。从此以后，每场战斗开始时，你都会更快进入状态。"),
                              EventEffectType::GainRelicById,
                              0,
                              RelicIds::qingtianObservationGuide(),
                              QStringLiteral("获得圣遗物 青天观测指南。"))
                << makeChoice(QStringLiteral("“我再想想”"),
                              QStringLiteral("你没有加入任何社团，但获得了宝贵的自由时间。"),
                              EventEffectType::GainNextBattleTurnBlock,
                              GameBalance::Events::clubFairFreeTimeTurnBlock(),
                              QString(),
                              QStringLiteral("下场战斗每回合开始时获得抗压值。"));

        return makeEvent(QStringLiteral("club_fair"),
                         QStringLiteral("百团大战"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createWusiNightRunEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("今天的五四夜奔似乎比较特殊。")
                   << QStringLiteral("群里有人说，今年不只是长跑，好像还会有特殊奖品。消息越传越热闹，你开始思考要不要去凑这个热闹。");

        QList<RandomEventEffect> runEffects;
        runEffects << makeEffect(EventEffectType::HealHp,
                                 GameBalance::Events::wusiNightRunHeal())
                   << makeEffect(EventEffectType::GainRelicById,
                                 0,
                                 RelicIds::pkuWatermelon());

        QList<RandomEventEffect> xiGaiEffects;
        xiGaiEffects << makeEffect(EventEffectType::LoseHp,
                                   GameBalance::Events::wusiNightRunXiGaiLoseHp())
                     << makeEffect(EventEffectType::GainUsualScore,
                                   GameBalance::Events::wusiNightRunXiGaiUsualScore());

        RandomEventChoice skipChoice = makeChoice(QStringLiteral("不去"),
                                                  QStringLiteral("这种跑步活动还是不去凑热闹了吧……"));
        skipChoice.showEffectSummary = false;

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("去参加"),
                                         QStringLiteral("你参加了五四长跑。大屏播放的动漫让你仿佛回到了童年。\n\n跑完之后，你领到了西瓜。冰凉的甜味在喉咙里散开，味道还不错。"),
                                         runEffects,
                                         QStringLiteral("回复心情，并获得圣遗物 北大的瓜。"))
                << skipChoice
                << makeChoiceWithEffects(QStringLiteral("你怎么知道我在上习概？"),
                                         QStringLiteral("我不是不想去，是去不了。\n\n你坐在习概课堂里，听着外面的消息一条条刷过，只能默默把手机扣下，继续记笔记。"),
                                         xiGaiEffects,
                                         QStringLiteral("失去心情，并获得平时分。"));

        return makeEvent(QStringLiteral("wusi_night_run"),
                         QStringLiteral("五四夜奔"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createWeishenPlainLunchEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("午饭时间，你刷到一段关于“韦神”的旧采访。镜头里没有豪华装备，只有矿泉水、馒头和一种近乎离谱的专注。")
                   << QStringLiteral("评论区里有人感叹天赋，有人转发锦鲤，也有人认真讨论：如果把外卖钱省下来买习题集，会不会变强。")
                   << QStringLiteral("你看着自己桌上的半份作业和还没点开的外卖软件，忽然觉得午饭也变成了一道选择题。");

        QList<RandomEventEffect> plainLunchEffects;
        plainLunchEffects << makeEffect(EventEffectType::LoseHp,
                                        GameBalance::Events::weishenPlainLunchLoseHp())
                          << makeEffect(EventEffectType::GainNextBattleStrength,
                                        GameBalance::Events::weishenPlainLunchStrength());

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("试试朴素午餐法"),
                                         QStringLiteral("你买了矿泉水和馒头，坐回桌前硬啃题目。味道很素，过程也很硬，但注意力居然真的被压缩到了一条窄窄的直线上。\n\n尽管你还是会想着其他美食。"),
                                         plainLunchEffects,
                                         QStringLiteral("失去心情，并在下一场战斗开局获得效率值。"))
                << makeChoice(QStringLiteral("先写完作业再说"),
                              QStringLiteral("你没有模仿午餐，而是模仿那种“先把手上的题做完”的气势。\n\n半小时后，你在草稿纸角落写下一个终于通顺的解法，忽然有了“我觉得我会了”的错觉。"),
                              EventEffectType::AddCardById,
                              0,
                              CardIds::pommel(),
                              QStringLiteral("获得卡牌：我觉得我会了。"))
                << makeChoice(QStringLiteral("尊重故事，然后去吃饭"),
                              QStringLiteral("你向这份朴素的专注表达敬意，然后诚实地走向食堂。\n\n吃饱之后，你发现自己又能重新面对作业了。"),
                              EventEffectType::HealHp,
                              GameBalance::Events::weishenPlainLunchHeal(),
                              QString(),
                              QStringLiteral("回复心情。"));

        return makeEvent(QStringLiteral("weishen_plain_lunch"),
                         QStringLiteral("馒头、矿泉水与数学"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createSecurityBoothNightClassEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("晚归时，楼下的楼长室还亮着。你本来只想低头刷脸，却看见玻璃窗后摊着一本被翻旧的书。")
                   << QStringLiteral("有人说，北大的工作人员里曾经走出过许多继续读书的人。这个故事在夜色里显得很安静，却比大多数鸡汤都更扎实。")
                   << QStringLiteral("你看着镜头里的自己，突然不太想把今晚随便糊弄过去。");

        QList<RandomEventEffect> talkEffects;
        talkEffects << makeEffect(EventEffectType::HealHp,
                                  GameBalance::Events::securityBoothTalkHeal())
                    << makeEffect(EventEffectType::GainUsualScore,
                                  GameBalance::Events::securityBoothTalkUsualScore());

        QList<RandomEventEffect> studyEffects;
        studyEffects << makeEffect(EventEffectType::LoseHp,
                                   GameBalance::Events::securityBoothStudyLoseHp())
                     << makeEffect(EventEffectType::GainUsualScore,
                                   GameBalance::Events::securityBoothStudyUsualScore());

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("停下聊几句"),
                                         QStringLiteral("你问起那本书，对方笑着说只是每天多看几页。没有豪言壮语，也没有背景音乐，只有一盏灯和一点点坚持。\n\n你带着这份轻微的震动离开，心情和分数都被往上托了一点。"),
                                         talkEffects,
                                         QStringLiteral("回复心情，并获得平时分。"))
                << makeChoiceWithEffects(QStringLiteral("回宿舍继续学习"),
                                         QStringLiteral("受到鼓舞之后，你回去继续努力学习。\n\n你感到自己颇有收获。"),
                                         studyEffects,
                                         QStringLiteral("失去心情，并获得平时分。"))
                << makeChoice(QStringLiteral("今晚还是先休息吧……"),
                              QStringLiteral("你没有立刻开始学习，但把这个故事收进了心里。\n\n至少今晚先好好休息一下吧。"),
                              EventEffectType::HealHp,
                              GameBalance::Events::securityBoothRestHeal(),
                              QString(),
                              QStringLiteral("回复心情。"));

        return makeEvent(QStringLiteral("security_booth_night_class"),
                         QStringLiteral("夜课"),
                         storyPages,
                         choices);
    }

    inline RandomEventData createBaijiangLastTicketEvent()
    {
        QStringList storyPages;
        storyPages << QStringLiteral("百讲今晚有一场演出。已开票，票务系统就像被全校同时点开一样，开始缓慢地转圈。")
                   << QStringLiteral("看着那个一直在转的圈，心里升起一种熟悉的竞技感。期末、作业和文化生活在同一个浏览器窗口里狭路相逢。")
                   << QStringLiteral("距离开票还有三分钟，你的鼠标已经悬在刷新按钮上。");

        QList<RandomEventEffect> ticketEffects;
        ticketEffects << makeEffect(EventEffectType::LoseHp,
                                    GameBalance::Events::baijiangTicketLoseHp())
                      << makeEffect(EventEffectType::GainCoins,
                                    -GameBalance::Events::baijiangTicketCost());

        QList<RandomEventEffect> treeholeEffects;
        treeholeEffects << makeEffect(EventEffectType::HealHp,
                                      GameBalance::Events::baijiangTreeholeHeal())
                        << makeEffect(EventEffectType::GainCoins,
                                      -GameBalance::Events::baijiangTreeholeCost());

        QList<RandomEventChoice> choices;
        choices << makeChoiceWithEffects(QStringLiteral("准点蹲票"),
                                         QStringLiteral("你在整点按下刷新。页面卡住、恢复、又卡住，最后居然真的跳出了一张可购买的票。\n\n抢票过程耗掉了一些心情，票钱也真实地从余额里消失了，但你至少拿到了座位。"),
                                         ticketEffects,
                                         QStringLiteral("失去心情，并失去 Money。"))
                << makeChoiceWithEffects(QStringLiteral("上树洞蹲守"),
                                         QStringLiteral("你没抢到票，只好上树洞等别人出票。\n\n虽然略贵，但至少最后你成功买到了票。"),
                                         treeholeEffects,
                                         QStringLiteral("回复心情，并失去余额。"))
                << makeChoice(QStringLiteral("把夜晚还给作业"),
                              QStringLiteral("你关闭票务页面，把这份竞争欲转移到作业上。\n\n没有掌声，没有舞台，但进度条往前走了一截。下一场战斗开始时，你会更快进入状态。"),
                              EventEffectType::GainNextBattleStrength,
                              GameBalance::Events::baijiangFocusStrength(),
                              QString(),
                              QStringLiteral("下回合开始时获得效率值。"));

        return makeEvent(QStringLiteral("baijiang_last_ticket"),
                         QStringLiteral("百讲最后一张票"),
                         storyPages,
                         choices);
    }

    inline QList<RandomEventData> allEvents()
    {
        QList<RandomEventData> events;
        events << createLibraryNightEvent()
               << createDormPowerOutageEvent()
               << createScienceBuildingElevatorEvent()
               << createWeimingLakeWalkEvent()
               << createStudentAffairsNewTaskEvent()
               << createOpenDayEvent()
               << createIdeologyPreEvent()
               << createGaokaoBlessingVideoEvent()
               << createClubFairEvent()
               << createWusiNightRunEvent()
               << createWeishenPlainLunchEvent()
               << createSecurityBoothNightClassEvent()
               << createBaijiangLastTicketEvent();

        // 新事件统一加在这里，例如：
        // events << createDormPowerOutageEvent();
        // events << createScienceBuildingElevatorEvent();
        // events << createWeimingLakeWalkEvent();

        return events;
    }

    inline RandomEventData randomEvent()
    {
        const QList<RandomEventData> events = allEvents();
        if (events.isEmpty())
        {
            return RandomEventData();
        }
        return events.at(GameRandom::instance().bounded(events.size()));
    }
}

#endif // EVENTLIBRARY_H
