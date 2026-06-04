#ifndef GAMETEXT_H
#define GAMETEXT_H

#include <QString>

// GameText.h
// Put player-facing names, descriptions, and short UI labels here.
// Example: change CardText::strikeName() to rename the starter attack everywhere CardCatalog uses it.
// Keep numeric balance in CardData/EnemyData unless the number is part of visible text.
namespace GameText
{
namespace Assets
{
inline QString mainMenuBackground() { return QStringLiteral(":/pictures/resources/pictures/main_menu_bg.png"); }
inline QString eventBackground() { return QStringLiteral(":/pictures/resources/pictures/event_bg.png"); }
// Character/enemy portrait insertion point.
// Example: put resources/pictures/player.png into resource.qrc, then change this to
// QStringLiteral(":/pictures/resources/pictures/player.png").
inline QString playerBattlePortrait() { return QStringLiteral(":/pictures/resources/pictures/backpack.png"); }
inline QString enemyFallbackPortrait() { return QStringLiteral(":/pictures/resources/pictures/brute.png"); }
}

namespace App
{
inline QString title() { return QString::fromUtf8(u8"卷王博雅塔"); }
inline QString subtitle() { return QString::fromUtf8(u8"从高数作业一路打到期末考试"); }
inline QString schoolLabel() { return QString::fromUtf8(u8"北京大学校园卡牌肉鸽"); }
}

namespace Menu
{
inline QString startButton() { return QString::fromUtf8(u8"开始修读"); }
inline QString quitButton() { return QString::fromUtf8(u8"退出游戏"); }
inline QString backToMenuButton() { return QString::fromUtf8(u8"主界面"); }
inline QString debugBattleButton() { return QString::fromUtf8(u8"测试战斗"); }
inline QString debugBossButton() { return QString::fromUtf8(u8"测试 Boss"); }
inline QString debugShopButton() { return QString::fromUtf8(u8"测试商店"); }
inline QString debugEventButton() { return QString::fromUtf8(u8"测试事件"); }
inline QString debugRewardButton() { return QString::fromUtf8(u8"测试奖励"); }
inline QString debugRestButton() { return QString::fromUtf8(u8"测试休息"); }
}

namespace EventText
{
inline QString pageTitle() { return QString::fromUtf8(u8"随机事件"); }
inline QString imagePlaceholder() { return QString::fromUtf8(u8"事件图"); }
inline QString enterFormat() { return QString::fromUtf8(u8"你进入了：%1\n\n%2"); }
inline QString confirmButton() { return QString::fromUtf8(u8"确认"); }
inline QString exitButton() { return QString::fromUtf8(u8"退出事件"); }
}

namespace Battle
{
inline QString endTurnButton() { return QString::fromUtf8(u8"结束回合"); }
inline QString drawPileButton() { return QString::fromUtf8(u8"抽牌堆 %1"); }
inline QString discardPileButton() { return QString::fromUtf8(u8"弃牌堆 %1"); }
inline QString exhaustPileButton() { return QString::fromUtf8(u8"消耗堆 %1"); }
inline QString potionConfirmButton() { return QString::fromUtf8(u8"确认使用"); }
inline QString potionCancelButton() { return QString::fromUtf8(u8"取消"); }
inline QString victoryText() { return QString::fromUtf8(u8"战斗胜利！正在进入奖励界面。"); }
inline QString defeatText() { return QString::fromUtf8(u8"你被击败了。"); }
inline QString emptyPileText() { return QString::fromUtf8(u8"这里暂时没有牌。"); }
}

namespace Reward
{
inline QString title() { return QString::fromUtf8(u8"卡牌奖励"); }
inline QString skipButton() { return QString::fromUtf8(u8"不选择卡牌"); }
inline QString rewardWithPotion() { return QString::fromUtf8(u8"获得 20 金币和一瓶火焰药水。选择一张卡牌加入牌组，或直接跳过。"); }
inline QString rewardPotionFull() { return QString::fromUtf8(u8"获得 20 金币。药水栏已满，无法获得新药水。选择一张卡牌加入牌组，或直接跳过。"); }
}

namespace Shop
{
inline QString title() { return QString::fromUtf8(u8"商店"); }
inline QString leaveButton() { return QString::fromUtf8(u8"离开商店"); }
inline QString emptyText() { return QString::fromUtf8(u8"商店暂时没有新的遗物。"); }
inline QString chooseText() { return QString::fromUtf8(u8"选择一个遗物购买。拥有幸运护符时，价格降低 10%。"); }
inline QString boughtButton() { return QString::fromUtf8(u8"已购买"); }
inline QString buyButtonFormat() { return QString::fromUtf8(u8"购买 %1 金币"); }
inline QString notEnoughGoldFormat() { return QString::fromUtf8(u8"金币不足，买不起 %1。"); }
inline QString boughtFormat() { return QString::fromUtf8(u8"购买了 %1。"); }
}

namespace EnemyText
{
inline QString attackIntentFormat() { return QString::fromUtf8(u8"攻击 %1"); }
inline QString healIntentFormat() { return QString::fromUtf8(u8"回复 %1 HP"); }
inline QString buffIntentFormat() { return QString::fromUtf8(u8"强化，力量 +%1"); }
inline QString blockIntentFormat() { return QString::fromUtf8(u8"获得 %1 格挡"); }
inline QString attackAndBuffIntentFormat() { return QString::fromUtf8(u8"攻击 %1，力量 +%2"); }
inline QString attackAndBlockIntentFormat() { return QString::fromUtf8(u8"攻击 %1，获得 %2 格挡"); }

inline QString campusCultistName() { return QString::fromUtf8(u8"绩点传教同学"); }
inline QString campusCultistDescription() { return QString::fromUtf8(u8"先给自己打鸡血，之后连续攻击。"); }
inline QString homeworkWormName() { return QString::fromUtf8(u8"高数题面虫"); }
inline QString homeworkWormDescription() { return QString::fromUtf8(u8"会一边进攻一边堆格挡，适合作为普通敌人的压力测试。"); }
inline QString ddlSlimeName() { return QString::fromUtf8(u8"DDL 史莱姆"); }
inline QString ddlSlimeDescription() { return QString::fromUtf8(u8"低血量普通敌人，会在攻击间隙获得格挡。"); }
inline QString projectNobName() { return QString::fromUtf8(u8"程设大作业精英"); }
inline QString projectNobDescription() { return QString::fromUtf8(u8"精英敌人，攻击会越拖越痛。"); }
inline QString finalExamName() { return QString::fromUtf8(u8"期末考试"); }
inline QString finalExamDescription() { return QString::fromUtf8(u8"最终 Boss，攻击、格挡和强化都会出现。"); }
}

namespace CardText
{
inline QString buttonTextFormat() { return QString::fromUtf8(u8"%1\n费用 %2\n%3"); }

inline QString strikeName() { return QString::fromUtf8(u8"刷题"); }
inline QString strikeDescription() { return QString::fromUtf8(u8"造成 6 点伤害。"); }
inline QString defendName() { return QString::fromUtf8(u8"防破防"); }
inline QString defendDescription() { return QString::fromUtf8(u8"获得 5 点格挡。"); }
inline QString bashName() { return QString::fromUtf8(u8"当堂点名"); }
inline QString bashDescription() { return QString::fromUtf8(u8"造成 8 点伤害，给予 2 层易伤。"); }
inline QString inflameName() { return QString::fromUtf8(u8"鸡血动员"); }
inline QString inflameDescription() { return QString::fromUtf8(u8"本场战斗力量 +2。消耗。"); }
inline QString heavyBladeName() { return QString::fromUtf8(u8"大题重击"); }
inline QString heavyBladeDescription() { return QString::fromUtf8(u8"造成 14 点伤害，力量加成按 3 倍计算。"); }
inline QString pommelName() { return QString::fromUtf8(u8"边写边想"); }
inline QString pommelDescription() { return QString::fromUtf8(u8"造成 9 点伤害，抽 1 张牌。"); }
inline QString shrugName() { return QString::fromUtf8(u8"缓口气"); }
inline QString shrugDescription() { return QString::fromUtf8(u8"获得 8 点格挡，抽 1 张牌。"); }
inline QString angerName() { return QString::fromUtf8(u8"灵感速写"); }
inline QString angerDescription() { return QString::fromUtf8(u8"费用为 0，造成 4 点伤害。"); }
inline QString flexName() { return QString::fromUtf8(u8"临时鸡血"); }
inline QString flexDescription() { return QString::fromUtf8(u8"本场战斗力量 +3，抽 1 张牌。消耗。"); }
inline QString cleaveName() { return QString::fromUtf8(u8"知识点横扫"); }
inline QString cleaveDescription() { return QString::fromUtf8(u8"造成 8 点伤害。"); }
}
}

#endif // GAMETEXT_H
