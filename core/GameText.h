#ifndef GAMETEXT_H
#define GAMETEXT_H

#include "GameBalance.h"

#include <QString>

// 所有玩家能看到的文案尽量集中在这里，方便课程展示前快速改名改描述。
namespace GameText
{
namespace Assets
{
inline QString mainMenuBackground() { return QStringLiteral("resources/main_menu_bg.png"); }
inline QString eventBackground() { return QStringLiteral("resources/event_bg.png"); }
inline QString mapBackground() { return QStringLiteral("resources/map_bg.png"); }
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
inline QString defaultEnemyIntent() { return QStringLiteral("下回合攻击 %1").arg(GameBalance::EnemyStatus::fallbackAttack()); }
inline QString battleTitleFormat() { return QStringLiteral("第 %1 战 · %2"); }
inline QString restartButton() { return QStringLiteral("重新开始"); }
inline QString nextBattleButton() { return QStringLiteral("下一场"); }
inline QString returnToMapButton() { return QStringLiteral("返回地图"); }
inline QString runClearTitle() { return QStringLiteral("通关 · 总评 A"); }
inline QString battleWinFormat() { return QStringLiteral("胜利 · 击败 %1 个敌人"); }
inline QString battleFailTitle() { return QStringLiteral("修读失败 · 需要重开"); }
inline QString hpFormat() { return QStringLiteral("HP %1/%2"); }
inline QString strengthLabelFormat() { return QStringLiteral("攻击强度 +%1"); }
inline QString drawPileButtonFormat() { return QStringLiteral("抽牌堆 %1"); }
inline QString discardPileButtonFormat() { return QStringLiteral("弃牌堆 %1"); }
inline QString exhaustPileButtonFormat() { return QStringLiteral("消耗堆 %1"); }
inline QString emptyPileText() { return QStringLiteral("这里暂时没有牌。"); }
inline QString potionEmptyText() { return QStringLiteral("空"); }
inline QString handTipText() { return QStringLiteral("拖动卡牌到敌人身上出牌。"); }
inline QString selectedCardTipFormat() { return QStringLiteral("已选择：%1"); }
inline QString playerInfoFormat() { return QStringLiteral("回合 %1  |  精力 %2/%3  |  攻击强度 +%4"); }
inline QString potionTitle() { return QStringLiteral("药水"); }
inline QString exhaustTag() { return QStringLiteral("消耗"); }
inline QString victoryTip() { return QStringLiteral("战斗胜利，正在结算奖励。"); }
inline QString failTip() { return QStringLiteral("精力归零，本次修读失败。"); }
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
inline QString strikeDescription(int damage);
inline QString defendDescription(int block);
inline QString bashDescription(int damage, int vulnerable);
inline QString inflameDescription(int strengthGain);
inline QString heavyBladeDescription(int damage, int strengthMultiplier);
inline QString pommelDescription(int damage, int draw);
inline QString shrugDescription(int block, int draw);
inline QString angerDescription(int damage);
inline QString flexDescription(int strengthGain, int draw);
inline QString cleaveDescription(int damage);

inline QString buttonTextFormat() { return QStringLiteral("%1\n费用 %2\n%3"); }
inline QString upgradedNameFormat() { return QStringLiteral("%1+"); }

inline QString strikeName() { return QStringLiteral("刷题"); }
inline QString strikeDescription() { return strikeDescription(GameBalance::Cards::strike().damage); }
inline QString strikeDescription(int damage) { return QStringLiteral("造成 %1 点伤害").arg(damage); }
inline QString defendName() { return QStringLiteral("防破防"); }
inline QString defendDescription() { return defendDescription(GameBalance::Cards::defend().block); }
inline QString defendDescription(int block) { return QStringLiteral("获得 %1 点格挡").arg(block); }
inline QString bashName() { return QStringLiteral("当堂点名"); }
inline QString bashDescription() { return bashDescription(GameBalance::Cards::bash().damage, GameBalance::Cards::bash().vulnerable); }
inline QString bashDescription(int damage, int vulnerable) { return QStringLiteral("造成 %1 点伤害\n给予 %2 层易伤").arg(damage).arg(vulnerable); }
inline QString inflameName() { return QStringLiteral("鸡血动员"); }
inline QString inflameDescription() { return inflameDescription(GameBalance::Cards::inflame().strengthGain); }
inline QString inflameDescription(int strengthGain) { return QStringLiteral("本场战斗攻击强度 +%1").arg(strengthGain); }
inline QString heavyBladeName() { return QStringLiteral("大题重击"); }
inline QString heavyBladeDescription() { return heavyBladeDescription(GameBalance::Cards::heavyBlade().damage, GameBalance::Cards::heavyBlade().strengthMultiplier); }
inline QString heavyBladeDescription(int damage, int strengthMultiplier) { return QStringLiteral("造成 %1 点伤害\n攻击强度加成按 %2 倍计算").arg(damage).arg(strengthMultiplier); }
inline QString pommelName() { return QStringLiteral("边写边想"); }
inline QString pommelDescription() { return pommelDescription(GameBalance::Cards::pommel().damage, GameBalance::Cards::pommel().draw); }
inline QString pommelDescription(int damage, int draw) { return QStringLiteral("造成 %1 点伤害\n抽 %2 张牌").arg(damage).arg(draw); }
inline QString shrugName() { return QStringLiteral("缓口气"); }
inline QString shrugDescription() { return shrugDescription(GameBalance::Cards::shrug().block, GameBalance::Cards::shrug().draw); }
inline QString shrugDescription(int block, int draw) { return QStringLiteral("获得 %1 点格挡\n抽 %2 张牌").arg(block).arg(draw); }
inline QString angerName() { return QStringLiteral("灵感速写"); }
inline QString angerDescription() { return angerDescription(GameBalance::Cards::anger().damage); }
inline QString angerDescription(int damage) { return QStringLiteral("造成 %1 点伤害\n费用为 0").arg(damage); }
inline QString flexName() { return QStringLiteral("临时鸡血"); }
inline QString flexDescription() { return flexDescription(GameBalance::Cards::flex().strengthGain, GameBalance::Cards::flex().draw); }
inline QString flexDescription(int strengthGain, int draw) { return QStringLiteral("本场战斗攻击强度 +%1\n抽 %2 张牌").arg(strengthGain).arg(draw); }
inline QString cleaveName() { return QStringLiteral("知识点横扫"); }
inline QString cleaveDescription() { return cleaveDescription(GameBalance::Cards::cleave().damage); }
inline QString cleaveDescription(int damage) { return QStringLiteral("造成 %1 点伤害").arg(damage); }

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

namespace RelicText
{
inline QString yanyuanMealCardDescription(int heal);
inline QString boyaTowerBadgeDescription(int strength);
inline QString librarySeatDescription(int block);
inline QString packedSchoolbagDescription(int draw);
inline QString luckyCouponDescription(int discountPercent);

inline QString yanyuanMealCardName() { return QStringLiteral("燕园饭卡"); }
inline QString yanyuanMealCardDescription() { return yanyuanMealCardDescription(GameBalance::Relics::yanyuanMealCardHeal()); }
inline QString yanyuanMealCardDescription(int heal) { return QStringLiteral("战斗胜利后回复 %1 点精力。").arg(heal); }
inline QString boyaTowerBadgeName() { return QStringLiteral("博雅塔徽章"); }
inline QString boyaTowerBadgeDescription() { return boyaTowerBadgeDescription(GameBalance::Relics::boyaTowerBadgeStrength()); }
inline QString boyaTowerBadgeDescription(int strength) { return QStringLiteral("每场战斗开始时，攻击强度 +%1。").arg(strength); }
inline QString librarySeatName() { return QStringLiteral("图书馆座位"); }
inline QString librarySeatDescription() { return librarySeatDescription(GameBalance::Relics::librarySeatBlock()); }
inline QString librarySeatDescription(int block) { return QStringLiteral("每场战斗开始时，获得 %1 点格挡。").arg(block); }
inline QString packedSchoolbagName() { return QStringLiteral("满载书包"); }
inline QString packedSchoolbagDescription() { return packedSchoolbagDescription(GameBalance::Relics::packedSchoolbagDraw()); }
inline QString packedSchoolbagDescription(int draw) { return QStringLiteral("每场战斗开始时，额外抽 %1 张牌。").arg(draw); }
inline QString luckyCouponName() { return QStringLiteral("百讲折扣券"); }
inline QString luckyCouponDescription() { return luckyCouponDescription(GameBalance::Relics::luckyCouponDiscount()); }
inline QString luckyCouponDescription(int discountPercent) { return QStringLiteral("商店里所有商品价格降低 %1%。").arg(discountPercent); }

// 圣遗物图片路径接口：以后把图片放进 resources/relics/ 后，在这里返回路径。
inline QString yanyuanMealCardImage() { return QString(); }
inline QString boyaTowerBadgeImage() { return QString(); }
inline QString librarySeatImage() { return QString(); }
inline QString packedSchoolbagImage() { return QString(); }
inline QString luckyCouponImage() { return QString(); }
}

namespace PotionText
{
inline QString coffeeShotDescription(int damage);
inline QString antiBreakSprayDescription(int block);
inline QString clinicSyrupDescription(int heal);

inline QString coffeeShotName() { return QStringLiteral("咖啡因冲剂"); }
inline QString coffeeShotDescription() { return coffeeShotDescription(GameBalance::Potions::coffeeShotDamage()); }
inline QString coffeeShotDescription(int damage) { return QStringLiteral("对敌人造成 %1 点伤害。").arg(damage); }
inline QString antiBreakSprayName() { return QStringLiteral("防破防喷雾"); }
inline QString antiBreakSprayDescription() { return antiBreakSprayDescription(GameBalance::Potions::antiBreakBlock()); }
inline QString antiBreakSprayDescription(int block) { return QStringLiteral("获得 %1 点格挡。").arg(block); }
inline QString clinicSyrupName() { return QStringLiteral("校医院糖浆"); }
inline QString clinicSyrupDescription() { return clinicSyrupDescription(GameBalance::Potions::clinicSyrupHeal()); }
inline QString clinicSyrupDescription(int heal) { return QStringLiteral("回复 %1 点精力。").arg(heal); }

// 药水图片路径接口：以后把图片放进 resources/potions/ 后，在这里返回路径。
inline QString coffeeShotImage() { return QString(); }
inline QString antiBreakSprayImage() { return QString(); }
inline QString clinicSyrupImage() { return QString(); }
}

namespace MapText
{
inline QString pageTitle() { return QStringLiteral("修读路线"); }
inline QString newRunButton() { return QStringLiteral("重新生成路线"); }
inline QString completeNodeButton() { return QStringLiteral("完成节点"); }
inline QString backToMapButton() { return QStringLiteral("返回地图"); }
inline QString battleNodeName() { return QStringLiteral("战斗"); }
inline QString eventNodeName() { return QStringLiteral("事件"); }
inline QString shopNodeName() { return QStringLiteral("商店"); }
inline QString restNodeName() { return QStringLiteral("休息"); }
inline QString rewardNodeName() { return QStringLiteral("奖励"); }
inline QString bossNodeName() { return QStringLiteral("期末"); }
inline QString completedNodeSuffix() { return QStringLiteral("已完成"); }
inline QString lockedTooltip() { return QStringLiteral("先完成上一层节点"); }
inline QString availableTooltip() { return QStringLiteral("点击进入该节点"); }
inline QString selectedTooltip() { return QStringLiteral("当前正在处理该节点"); }
}

namespace ShopText
{
inline QString intro(int discountPercent);

inline QString pageTitle() { return QStringLiteral("校园商店"); }
inline QString leaveButton() { return QStringLiteral("离开商店"); }
inline QString coinsFormat() { return QStringLiteral("资源点：%1"); }
inline QString intro() { return intro(GameBalance::Shop::discountPercent()); }
inline QString intro(int discountPercent) { return QStringLiteral("买一点能撑过下个 ddl 的东西。百讲折扣券会让商店价格降低 %1%。").arg(discountPercent); }
inline QString relicSectionTitle() { return QStringLiteral("圣遗物"); }
inline QString cardSectionTitle() { return QStringLiteral("卡牌"); }
inline QString potionSectionTitle() { return QStringLiteral("药水"); }
inline QString serviceSectionTitle() { return QStringLiteral("服务"); }
inline QString buyButtonFormat() { return QStringLiteral("购买 %1"); }
inline QString boughtButton() { return QStringLiteral("已购买"); }
inline QString noRelicText() { return QStringLiteral("暂时没有新的圣遗物。"); }
inline QString noPotionSlotText() { return QStringLiteral("药水栏已满。"); }
inline QString buySuccessFormat() { return QStringLiteral("购买了：%1"); }
inline QString notEnoughCoinsFormat() { return QStringLiteral("资源点不足，买不起：%1"); }
inline QString shopFinishedTip() { return QStringLiteral("离开后该商店节点完成。"); }
inline QString cardRemovalName() { return QStringLiteral("移除一张卡牌"); }
inline QString cardRemovalDescription() { return QStringLiteral("从当前牌组中永久删除一张卡牌。"); }
inline QString cardRemovalDialogTitle() { return QStringLiteral("选择要移除的卡牌"); }
inline QString cardRemovalSuccess() { return QStringLiteral("卡牌已移除。"); }
inline QString cardRemovalNotEnoughCoins() { return QStringLiteral("资源点不足，无法移除卡牌。"); }
inline QString emptyDeckText() { return QStringLiteral("当前没有可移除的卡牌。"); }
}

namespace RewardText
{
inline QString rewardPotionFull(int coins);

inline QString pageTitle() { return QStringLiteral("战斗奖励"); }
inline QString rewardWithPotion() { return QStringLiteral("获得 %1 资源点，并得到一瓶咖啡因冲剂。选择一张卡加入牌组。").arg(GameBalance::Rewards::rewardScreenCoins()); }
inline QString rewardPotionFull() { return rewardPotionFull(GameBalance::Rewards::rewardScreenCoins()); }
inline QString rewardWithPotion(int coins, const QString &potionName) { return QStringLiteral("获得 %1 资源点，并得到一瓶%2。选择一张卡加入牌组。").arg(coins).arg(potionName); }
inline QString rewardPotionFull(int coins) { return QStringLiteral("获得 %1 资源点。药水栏已满，无法获得新的药水。选择一张卡加入牌组。").arg(coins); }
inline QString skipButton() { return QStringLiteral("跳过奖励"); }
inline QString takeCardButton() { return QStringLiteral("加入牌组"); }
inline QString finishTip() { return QStringLiteral("奖励结算完成。"); }
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
