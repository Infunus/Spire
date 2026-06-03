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
inline QString defaultEnemyName() { return QStringLiteral("高数作业"); }
inline QString defaultEnemyIntent() { return QStringLiteral("下回合攻击 6"); }
inline QString battleTitleFormat() { return QStringLiteral("第 %1 战 · %2"); }
inline QString restartButton() { return QStringLiteral("重新开始"); }
inline QString nextBattleButton() { return QStringLiteral("下一场"); }
inline QString runClearTitle() { return QStringLiteral("通关 · 总评 A"); }
inline QString battleWinFormat() { return QStringLiteral("胜利 · 击败 %1 个敌人"); }
inline QString battleFailTitle() { return QStringLiteral("修读失败 · 需要重开"); }
inline QString hpFormat() { return QStringLiteral("HP %1/%2"); }
}

namespace EnemyText
{
inline QString attackIntentFormat() { return QStringLiteral("攻击 %1"); }
inline QString healIntentFormat() { return QStringLiteral("回复 %1 HP"); }
inline QString buffIntentFormat() { return QStringLiteral("卷起来，攻击 +%1"); }
inline QString attackAndBuffIntentFormat() { return QStringLiteral("攻击 %1，攻击 +%2"); }
inline QString weakStatusFormat() { return QStringLiteral("虚弱%1"); }
inline QString vulnerableStatusFormat() { return QStringLiteral("易伤%1"); }
inline QString strengthStatusFormat() { return QStringLiteral("强度+%1"); }
inline QString statusSeparator() { return QStringLiteral(" | "); }
inline QString statusJoiner() { return QStringLiteral(" "); }

inline QString gaoshuHomeworkName() { return QStringLiteral("高数作业"); }
inline QString programProjectName() { return QStringLiteral("程设大作业"); }
inline QString midtermName() { return QStringLiteral("期中考试"); }
inline QString finalExamName() { return QStringLiteral("期末考试"); }
}

namespace CardText
{
inline QString buttonTextFormat() { return QStringLiteral("%1\n费用 %2\n%3"); }

inline QString attackName() { return QStringLiteral("卷"); }
inline QString attackDescription() { return QStringLiteral("造成 6 点伤害"); }
inline QString defendName() { return QStringLiteral("防破防"); }
inline QString defendDescription() { return QStringLiteral("获得 5 点格挡"); }
inline QString nightName() { return QStringLiteral("熬夜"); }
inline QString nightDescription() { return QStringLiteral("造成 10 点伤害\n失去 2 HP"); }
inline QString healName() { return QStringLiteral("摸鱼回血"); }
inline QString healDescription() { return QStringLiteral("回复 5 HP"); }
inline QString studyName() { return QStringLiteral("通宵复习"); }
inline QString studyDescription() { return QStringLiteral("造成 15 点伤害"); }
inline QString assistantName() { return QStringLiteral("请教助教"); }
inline QString assistantDescription() { return QStringLiteral("造成 3 点伤害\n给予 2 层虚弱"); }
inline QString markName() { return QStringLiteral("划重点"); }
inline QString markDescription() { return QStringLiteral("获得 4 格挡\n给予 2 层易伤"); }
inline QString inspirationName() { return QStringLiteral("灵感闪现"); }
inline QString inspirationDescription() { return QStringLiteral("抽 2 张牌"); }
}
}

#endif // GAMETEXT_H
