#ifndef GAMETEXT_H
#define GAMETEXT_H

#include <QString>

// 所有玩家能看到的文案尽量集中在这里，方便课程展示前快速改名改描述。
namespace GameText
{
namespace Assets
{
inline QString mainMenuBackground() { return QStringLiteral("resources/main_menu_bg.png"); }
inline QString eventBackground() { return QStringLiteral("resources/event_bg.png"); }
}

namespace App
{
inline QString title() { return QStringLiteral("卷王博雅塔"); }
}

namespace Menu
{
inline QString schoolLabel() { return QStringLiteral("北京大学校园卡牌肉鸽"); }
inline QString subtitle() { return QStringLiteral("从高数作业一路打到期末考试"); }
inline QString startButton() { return QStringLiteral("开始修读"); }
inline QString eventPreviewButton() { return QStringLiteral("事件预览"); }
inline QString quitButton() { return QStringLiteral("退出游戏"); }
inline QString backToMenuButton() { return QStringLiteral("主界面"); }

inline QString debugTitle() { return QStringLiteral("开发调试入口"); }
inline QString debugBattleButton() { return QStringLiteral("普通战斗"); }
inline QString debugBossButton() { return QStringLiteral("Boss 战"); }
inline QString debugEventButton() { return QStringLiteral("随机事件"); }
inline QString debugMapButton() { return QStringLiteral("地图节点"); }
inline QString debugRewardButton() { return QStringLiteral("卡牌奖励"); }
inline QString debugShopButton() { return QStringLiteral("商店"); }
inline QString debugRestButton() { return QStringLiteral("休息点"); }
}

namespace EventText
{
inline QString pageTitle() { return QStringLiteral("随机事件"); }
inline QString eventImagePlaceholder() { return QStringLiteral("事件图"); }
inline QString defaultTitle() { return QStringLiteral("未命名事件"); }
inline QString defaultBody() { return QStringLiteral("在这里填写事件正文。可以写成北大校园生活里的一个选择场景。"); }
inline QString choicePrefixFormat() { return QStringLiteral("%1. %2"); }
inline QString resultFormat() { return QStringLiteral("已选择：%1"); }

inline QString previewTitle() { return QStringLiteral("图书馆深夜"); }
inline QString previewBody()
{
    return QStringLiteral("这里是随机事件正文占位。后续可以改成图书馆、理教、宿舍、食堂、未名湖等主题事件。");
}
inline QString previewChoiceA() { return QStringLiteral("继续自习，获得一张学习牌"); }
inline QString previewChoiceB() { return QStringLiteral("回宿舍休息，回复少量 HP"); }
inline QString previewChoiceC() { return QStringLiteral("去便利店补给，获得临时资源"); }
}

namespace Battle
{
inline QString bossName() { return QStringLiteral("Boss"); }
inline QString playerName() { return QStringLiteral("玩家"); }
inline QString relicTitle() { return QStringLiteral("燕园加成"); }
inline QString confirmCardButton() { return QStringLiteral("确认出牌"); }
inline QString endTurnButton() { return QStringLiteral("结束回合"); }
inline QString energyLabel() { return QStringLiteral("精力"); }
inline QString enemyAreaTitle() { return QStringLiteral("敌方单位区"); }
inline QString portraitPlaceholder() { return QStringLiteral("立绘"); }
inline QString enemyImagePlaceholder() { return QStringLiteral("敌人图"); }
inline QString pendingIntent() { return QStringLiteral("意图待定"); }
inline QString defaultEnemyName() { return QStringLiteral("绩点传教同学"); }
inline QString defaultEnemyIntent() { return QStringLiteral("下回合攻击 6"); }
inline QString battleTitleFormat() { return QStringLiteral("第 %1 战 · %2"); }
inline QString restartButton() { return QStringLiteral("重新开始"); }
inline QString nextBattleButton() { return QStringLiteral("下一场"); }
inline QString runClearTitle() { return QStringLiteral("通关 · 总评 A"); }
inline QString battleWinFormat() { return QStringLiteral("胜利 · 击败 %1 个敌人"); }
inline QString battleFailTitle() { return QStringLiteral("修读失败 · 需要重开"); }
inline QString hpFormat() { return QStringLiteral("HP %1/%2"); }
inline QString strengthLabelFormat() { return QStringLiteral("攻击强度 +%1"); }
}

namespace EnemyText
{
inline QString attackIntentFormat() { return QStringLiteral("攻击 %1"); }
inline QString healIntentFormat() { return QStringLiteral("回复 %1 HP"); }
inline QString buffIntentFormat() { return QStringLiteral("卷起来，攻击 +%1"); }
inline QString blockIntentFormat() { return QStringLiteral("获得 %1 格挡"); }
inline QString attackAndBuffIntentFormat() { return QStringLiteral("攻击 %1，攻击 +%2"); }
inline QString attackAndBlockIntentFormat() { return QStringLiteral("攻击 %1，获得 %2 格挡"); }
inline QString weakStatusFormat() { return QStringLiteral("虚弱%1"); }
inline QString vulnerableStatusFormat() { return QStringLiteral("易伤%1"); }
inline QString strengthStatusFormat() { return QStringLiteral("强度+%1"); }
inline QString blockStatusFormat() { return QStringLiteral("格挡%1"); }
inline QString statusSeparator() { return QStringLiteral(" | "); }
inline QString statusJoiner() { return QStringLiteral(" "); }

inline QString campusCultistName() { return QStringLiteral("绩点传教同学"); }
inline QString campusCultistDescription() { return QStringLiteral("先给自己打鸡血，之后连续攻击。"); }
inline QString homeworkWormName() { return QStringLiteral("高数题面虫"); }
inline QString homeworkWormDescription() { return QStringLiteral("会一边进攻一边堆格挡，适合作为普通敌人的压力测试。"); }
inline QString ddlSlimeName() { return QStringLiteral("DDL 史莱姆"); }
inline QString ddlSlimeDescription() { return QStringLiteral("低血量普通敌人，会在攻击间隙获得格挡。"); }
inline QString projectNobName() { return QStringLiteral("程设大作业精英"); }
inline QString projectNobDescription() { return QStringLiteral("精英敌人，攻击会越拖越痛。"); }
inline QString finalExamName() { return QStringLiteral("期末考试"); }
inline QString finalExamDescription() { return QStringLiteral("最终 Boss，攻击、格挡和强化都会出现。"); }

// 图片路径接口：以后把资源放进 resources/ 后，在这里返回路径即可，例如 QStringLiteral("resources/cards/strike.png")。
inline QString campusCultistImage() { return QString(); }
inline QString homeworkWormImage() { return QString(); }
inline QString ddlSlimeImage() { return QString(); }
inline QString projectNobImage() { return QString(); }
inline QString finalExamImage() { return QString(); }
}

namespace CardText
{
inline QString buttonTextFormat() { return QStringLiteral("%1\n费用 %2\n%3"); }

inline QString strikeName() { return QStringLiteral("刷题"); }
inline QString strikeDescription() { return QStringLiteral("造成 6 点伤害"); }
inline QString defendName() { return QStringLiteral("防破防"); }
inline QString defendDescription() { return QStringLiteral("获得 5 点格挡"); }
inline QString bashName() { return QStringLiteral("当堂点名"); }
inline QString bashDescription() { return QStringLiteral("造成 8 点伤害\n给予 2 层易伤"); }
inline QString inflameName() { return QStringLiteral("鸡血动员"); }
inline QString inflameDescription() { return QStringLiteral("本场战斗攻击强度 +2"); }
inline QString heavyBladeName() { return QStringLiteral("大题重击"); }
inline QString heavyBladeDescription() { return QStringLiteral("造成 14 点伤害\n攻击强度加成按 3 倍计算"); }
inline QString pommelName() { return QStringLiteral("边写边想"); }
inline QString pommelDescription() { return QStringLiteral("造成 9 点伤害\n抽 1 张牌"); }
inline QString shrugName() { return QStringLiteral("缓口气"); }
inline QString shrugDescription() { return QStringLiteral("获得 8 点格挡\n抽 1 张牌"); }
inline QString angerName() { return QStringLiteral("灵感速写"); }
inline QString angerDescription() { return QStringLiteral("造成 4 点伤害\n费用为 0"); }
inline QString flexName() { return QStringLiteral("临时鸡血"); }
inline QString flexDescription() { return QStringLiteral("本场战斗攻击强度 +3\n抽 1 张牌"); }
inline QString cleaveName() { return QStringLiteral("知识点横扫"); }
inline QString cleaveDescription() { return QStringLiteral("造成 8 点伤害"); }

// 卡牌图片路径接口：名称和描述在上面改，图片在这里改。
inline QString strikeImage() { return QString(); }
inline QString defendImage() { return QString(); }
inline QString bashImage() { return QString(); }
inline QString inflameImage() { return QString(); }
inline QString heavyBladeImage() { return QString(); }
inline QString pommelImage() { return QString(); }
inline QString shrugImage() { return QString(); }
inline QString angerImage() { return QString(); }
inline QString flexImage() { return QString(); }
inline QString cleaveImage() { return QString(); }
}

namespace DebugText
{
inline QString mapTitle() { return QStringLiteral("地图节点"); }
inline QString mapBody() { return QStringLiteral("地图 UI 尚未实现。这里先作为开发占位页，后续接入固定节点路线：战斗、事件、商店、休息、Boss。"); }
inline QString rewardTitle() { return QStringLiteral("卡牌奖励"); }
inline QString rewardBody() { return QStringLiteral("奖励 UI 尚未实现。后续这里展示三选一卡牌，并把选择结果加入牌组。"); }
inline QString shopTitle() { return QStringLiteral("商店"); }
inline QString shopBody() { return QStringLiteral("商店 UI 尚未实现。后续这里展示可购买卡牌、遗物和删牌入口。"); }
inline QString restTitle() { return QStringLiteral("休息点"); }
inline QString restBody() { return QStringLiteral("休息 UI 尚未实现。后续这里提供回血、升级卡牌等选项。"); }
}
}

#endif // GAMETEXT_H
