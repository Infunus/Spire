#ifndef GAMETEXT_H
#define GAMETEXT_H

#include "GameBalance.h"

#include <QString>

/*
 * 文案总表
 *
 * 课程展示前如果要改名字、描述、按钮文字、图片路径，优先改这个文件。
 * 数值不要放在这里，统一去 GameBalance.h 调；事件正文优先去 EventLibrary.h 填。
 *
 * 常用修改位置：
 * - 游戏名和主菜单：App / Menu
 * - 战斗界面固定文案：Battle
 * - 卡牌名称、描述、图片：CardText
 * - 敌人名称、描述、图片：EnemyText
 * - 圣遗物、药水、商店、奖励：对应 namespace
 */
namespace GameText
{

    // 图片资源路径。路径从项目根目录写起，文件不存在时 UI 会显示占位或备用背景。
    namespace Assets
    {
        inline QString mainMenuBackground() { return QStringLiteral("resources/main_menu_bg.png"); }
        inline QString eventBackground() { return QStringLiteral("resources/event_bg.png"); }
        inline QString mapBackground() { return QStringLiteral("resources/map_bg.png"); }
        inline QString battleBackground() { return QStringLiteral("resources/battle_bg.png"); }
    }

    namespace App
    {
        inline QString title() { return QStringLiteral("卷王博雅塔"); }
    }

    // 主菜单和开发调试入口。
    namespace Menu
    {
        inline QString schoolLabel() { return QStringLiteral("在卡牌与肉鸽中体验燕园生活"); }
        inline QString subtitle() { return QStringLiteral("保持健康，提高绩点，登临卷王之巅"); }

        inline QString startButton() { return QStringLiteral("开始新学期"); }
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

    // 战斗界面的固定 UI 文案。
    namespace Battle
    {
        inline QString bossName() { return QStringLiteral("Boss"); }
        inline QString playerName() { return QStringLiteral("玩家"); }
        inline QString relicTitle() { return QStringLiteral("燕园加成"); }
        inline QString potionTitle() { return QStringLiteral("药水"); }

        inline QString confirmCardButton() { return QStringLiteral("确认出牌"); }
        inline QString endTurnButton() { return QStringLiteral("结束回合"); }
        inline QString restartButton() { return QStringLiteral("重新开始"); }
        inline QString nextBattleButton() { return QStringLiteral("下一场"); }
        inline QString returnToMapButton() { return QStringLiteral("返回地图"); }

        inline QString energyLabel() { return QStringLiteral("精力"); }
        inline QString enemyAreaTitle() { return QStringLiteral("敌方单位区"); }
        inline QString portraitPlaceholder() { return QStringLiteral("立绘"); }
        inline QString enemyImagePlaceholder() { return QStringLiteral("敌人图"); }
        inline QString pendingIntent() { return QStringLiteral("意图待定"); }
        inline QString defaultEnemyName() { return QStringLiteral("高数作业"); }
        inline QString defaultEnemyIntent()
        {
            return QStringLiteral("下回合攻击 %1").arg(GameBalance::EnemyStatus::fallbackAttack());
        }

        inline QString battleTitleFormat() { return QStringLiteral("第 %1 战 · %2"); }
        inline QString runClearTitle() { return QStringLiteral("通关 · 总评 A"); }
        inline QString battleWinFormat() { return QStringLiteral("胜利 · 击败 %1 个敌人"); }
        inline QString battleFailTitle() { return QStringLiteral("喜报：您已被退学！"); }
        inline QString victoryTip() { return QStringLiteral("完成作业！助教正在批阅……"); }
        inline QString failTip() { return QStringLiteral("心情归零，本次修读失败。"); }

        inline QString playerHpFormat() { return QStringLiteral("心情 %1/%2"); }
        inline QString enemyHpFormat() { return QStringLiteral("任务量 %1/%2"); }
        inline QString hpFormat() { return enemyHpFormat(); } // 旧接口，后续优先用 playerHpFormat / enemyHpFormat。
        inline QString strengthLabelFormat() { return QStringLiteral("效率值 +%1"); }
        inline QString playerInfoFormat()
        {
            return QStringLiteral("回合 %1  |  精力 %2/%3  |  效率值 +%4");
        }

        inline QString playerBlockStatusName() { return QStringLiteral("抗压值"); }
        inline QString playerBlockStatusRules() { return QStringLiteral("格挡：先抵消敌人造成的心情损失。"); }
        inline QString playerStrengthStatusName() { return QStringLiteral("效率值"); }
        inline QString playerStrengthStatusShort() { return QStringLiteral("效"); }
        inline QString playerStrengthStatusRules() { return QStringLiteral("力量：你的攻击牌造成的任务量增加。"); }
        inline QString statusTooltip(const QString &name, int value, const QString &rulesText)
        {
            return QStringLiteral("%1 x%2\n%3").arg(name).arg(value).arg(rulesText);
        }
        inline QString intentTooltip(const QString &intentText, const QString &rulesText)
        {
            return QStringLiteral("%1\n%2").arg(intentText, rulesText);
        }

        inline QString drawPileButtonFormat() { return QStringLiteral("抽牌堆 %1"); }
        inline QString discardPileButtonFormat() { return QStringLiteral("弃牌堆 %1"); }
        inline QString exhaustPileButtonFormat() { return QStringLiteral("消耗堆 %1"); }
        inline QString emptyPileText() { return QStringLiteral("这里暂时没有牌。"); }
        inline QString exhaustTag() { return QStringLiteral("消耗"); }

        inline QString potionEmptyText() { return QStringLiteral("空"); }
        inline QString potionUseTooltip(const QString &description)
        {
            return QStringLiteral("%1\n拖动到目标使用").arg(description);
        }
        inline QString handTipText() { return QStringLiteral("拖动卡牌到敌人身上出牌。"); }
        inline QString selectedCardTipFormat() { return QStringLiteral("已选择：%1"); }
    }

    // 卡牌文本。改卡牌名字、描述、图片路径优先看这里。
    namespace CardText
    {
        inline QString buttonTextFormat() { return QStringLiteral("%1\n费用 %2\n%3"); }
        inline QString upgradedNameFormat() { return QStringLiteral("%1+"); }

        inline QString strikeName() { return QStringLiteral("与作业战斗！"); }
        inline QString strikeDescription(int damage) { return QStringLiteral("完成 %1 点任务量").arg(damage); }
        inline QString strikeDescription() { return strikeDescription(GameBalance::Cards::strike().damage); }
        inline QString strikeImage() { return QString(); }

        inline QString defendName() { return QStringLiteral("刷会儿b站"); }
        inline QString defendDescription(int block) { return QStringLiteral("获得 %1 点抗压值").arg(block); }
        inline QString defendDescription() { return defendDescription(GameBalance::Cards::defend().block); }
        inline QString defendImage() { return QString(); }

        inline QString bashName() { return QStringLiteral("这题我见过！"); }
        inline QString bashDescription(int damage, int vulnerable)
        {
            return QStringLiteral("完成 %1 点任务量\n给予 %2 层思路突破").arg(damage).arg(vulnerable);
        }
        inline QString bashDescription()
        {
            return bashDescription(GameBalance::Cards::bash().damage, GameBalance::Cards::bash().vulnerable);
        }
        inline QString bashImage() { return QString(); }

        inline QString inflameName() { return QStringLiteral("思路打开"); }
        inline QString inflameDescription(int strengthGain)
        {
            return QStringLiteral("本场战斗效率值 +%1").arg(strengthGain);
        }
        inline QString inflameDescription()
        {
            return inflameDescription(GameBalance::Cards::inflame().strengthGain);
        }
        inline QString inflameImage() { return QString(); }

        inline QString heavyBladeName() { return QStringLiteral("这就是心流？"); }
        inline QString heavyBladeDescription(int damage, int strengthMultiplier)
        {
            return QStringLiteral("完成 %1 点任务量\n效率值加成按 %2 倍计算")
                .arg(damage)
                .arg(strengthMultiplier);
        }
        inline QString heavyBladeDescription()
        {
            return heavyBladeDescription(GameBalance::Cards::heavyBlade().damage,
                                         GameBalance::Cards::heavyBlade().strengthMultiplier);
        }
        inline QString heavyBladeImage() { return QString(); }

        inline QString pommelName() { return QStringLiteral("我觉得我会了"); }
        inline QString pommelDescription(int damage, int draw)
        {
            return QStringLiteral("完成 %1 点任务量\n抽 %2 张牌").arg(damage).arg(draw);
        }
        inline QString pommelDescription()
        {
            return pommelDescription(GameBalance::Cards::pommel().damage, GameBalance::Cards::pommel().draw);
        }
        inline QString pommelImage() { return QString(); }

        inline QString shrugName() { return QStringLiteral("原神，启动！"); }
        inline QString shrugDescription(int block, int draw)
        {
            return QStringLiteral("获得 %1 点抗压值\n抽 %2 张牌").arg(block).arg(draw);
        }
        inline QString shrugDescription()
        {
            return shrugDescription(GameBalance::Cards::shrug().block, GameBalance::Cards::shrug().draw);
        }
        inline QString shrugImage() { return QString(); }

        inline QString angerName() { return QStringLiteral("灵感，稍纵即逝！"); }
        inline QString angerDescription(int damage)
        {
            return QStringLiteral("完成 %1 点任务量\n不消耗精力").arg(damage);
        }
        inline QString angerDescription() { return angerDescription(GameBalance::Cards::anger().damage); }
        inline QString angerImage() { return QString(); }

        inline QString flexName() { return QStringLiteral("来根士力架！"); }
        inline QString flexDescription(int strengthGain, int draw)
        {
            return QStringLiteral("本场战斗效率值 +%1\n抽 %2 张牌").arg(strengthGain).arg(draw);
        }
        inline QString flexDescription()
        {
            return flexDescription(GameBalance::Cards::flex().strengthGain, GameBalance::Cards::flex().draw);
        }
        inline QString flexImage() { return QString(); }

        inline QString cleaveName() { return QStringLiteral("今晚必拿下"); }
        inline QString cleaveDescription(int damage) { return QStringLiteral("完成 %1 点任务量").arg(damage); }
        inline QString cleaveDescription() { return cleaveDescription(GameBalance::Cards::cleave().damage); }
        inline QString cleaveImage() { return QString(); }
    }

    // 敌人文本和图片路径。
    namespace EnemyText
    {
        inline QString attackIntentFormat() { return QStringLiteral("破坏心情 %1"); }
        inline QString healIntentFormat() { return QStringLiteral("回复 %1 任务量"); }
        inline QString buffIntentFormat() { return QStringLiteral("压力值 +%1"); }
        inline QString blockIntentFormat() { return QStringLiteral("增加 %1 复杂度"); }
        inline QString attackAndBuffIntentFormat() { return QStringLiteral("破坏心情 %1，压力值 +%2"); }
        inline QString attackAndBlockIntentFormat() { return QStringLiteral("破坏心情 %1，增加 %2 复杂度"); }

        inline QString attackIntentRules(int damage)
        {
            return QStringLiteral("攻击：敌人行动时会让玩家失去 %1 点心情。").arg(damage);
        }
        inline QString healIntentRules(int amount)
        {
            return QStringLiteral("回复：敌人行动时恢复 %1 点任务量。").arg(amount);
        }
        inline QString buffIntentRules(int amount)
        {
            return QStringLiteral("力量：敌人后续攻击伤害增加 %1。").arg(amount);
        }
        inline QString blockIntentRules(int amount)
        {
            return QStringLiteral("格挡：敌人行动时获得 %1 点护甲，先抵消受到的任务量。").arg(amount);
        }
        inline QString attackAndBuffIntentRules(int damage, int strength)
        {
            return QStringLiteral("攻击 + 力量：先让玩家失去 %1 点心情，再让后续攻击伤害增加 %2。")
                .arg(damage)
                .arg(strength);
        }
        inline QString attackAndBlockIntentRules(int damage, int block)
        {
            return QStringLiteral("攻击 + 格挡：先让玩家失去 %1 点心情，再获得 %2 点护甲。")
                .arg(damage)
                .arg(block);
        }

        inline QString weakStatusName() { return QStringLiteral("疲劳"); }
        inline QString weakStatusShort() { return QStringLiteral("疲"); }
        inline QString weakStatusRules() { return QStringLiteral("虚弱：攻击造成的伤害降低。"); }
        inline QString vulnerableStatusName() { return QStringLiteral("破防"); }
        inline QString vulnerableStatusShort() { return QStringLiteral("破"); }
        inline QString vulnerableStatusRules() { return QStringLiteral("易伤：受到的伤害提高约 50%。"); }
        inline QString strengthStatusName() { return QStringLiteral("压力值"); }
        inline QString strengthStatusShort() { return QStringLiteral("压"); }
        inline QString strengthStatusRules() { return QStringLiteral("力量：后续攻击伤害增加。"); }
        inline QString blockStatusName() { return QStringLiteral("复杂度"); }
        inline QString blockStatusRules() { return QStringLiteral("格挡：先抵消受到的任务量。"); }

        inline QString weakStatusFormat() { return weakStatusName() + QStringLiteral("%1"); }
        inline QString vulnerableStatusFormat() { return vulnerableStatusName() + QStringLiteral("%1"); }
        inline QString strengthStatusFormat() { return strengthStatusName() + QStringLiteral("+%1"); }
        inline QString blockStatusFormat() { return blockStatusName() + QStringLiteral("%1"); }
        inline QString statusSeparator() { return QStringLiteral(" | "); }
        inline QString statusJoiner() { return QStringLiteral(" "); }

        inline QString campusCultistName() { return QStringLiteral("高数作业"); }
        inline QString campusCultistDescription() { return QStringLiteral("数学不会那就是真不会"); }
        inline QString campusCultistImage() { return QString(); }

        inline QString homeworkWormName() { return QStringLiteral("POJ习题"); }
        inline QString homeworkWormDescription()
        {
            return QStringLiteral("邪恶的编程题！");
        }
        inline QString homeworkWormImage() { return QString(); }

        inline QString ddlSlimeName() { return QStringLiteral("水课作业"); }
        inline QString ddlSlimeDescription()
        {
            return QStringLiteral("水课还是好糊弄的");
        }
        inline QString ddlSlimeImage() { return QString(); }

        inline QString projectNobName() { return QStringLiteral("小组汇报"); }
        inline QString projectNobDescription() { return QStringLiteral("“所以我们的主题选什么好呢……？”“谁会做PPT？”“交给AI算了……”“……我不上台讲，别的都好说”“诶？大家怎么都做完了，记得把我名字加上去”……"); }
        inline QString projectNobImage() { return QString(); }

        inline QString finalExamName() { return QStringLiteral("期末考试"); }
        inline QString finalExamDescription()
        {
            return QStringLiteral("不调分！！");
        }
        inline QString finalExamImage() { return QString(); }
    }

    // 随机事件 UI 固定文案。具体事件内容在 EventLibrary.h。
    namespace EventText
    {
        inline QString pageTitle() { return QStringLiteral("随机事件"); }
        inline QString eventImagePlaceholder() { return QStringLiteral("事件图"); }
        inline QString defaultTitle() { return QStringLiteral("未命名事件"); }
        inline QString defaultBody()
        {
            return QStringLiteral("在这里填写事件正文。可以写成北大校园生活里的一个选择场景。");
        }

        inline QString continueButton() { return QStringLiteral("继续"); }
        inline QString finishEventButton() { return QStringLiteral("结束"); }
        inline QString eventChoiceHint() { return QStringLiteral("请选择这次事件的处理方式。"); }
        inline QString choicePrefixFormat() { return QStringLiteral("%1. %2"); }
        inline QString resultFormat() { return QStringLiteral("已选择：%1"); }

        inline QString previewTitle() { return QStringLiteral("图书馆深夜"); }
        inline QString previewBody()
        {
            return QStringLiteral("这里是随机事件正文占位。后续可以改成图书馆、理教、宿舍、食堂、未名湖等主题事件。");
        }
        inline QString previewChoiceA() { return QStringLiteral("继续自习，获得一张学习牌"); }
        inline QString previewChoiceB() { return QStringLiteral("回宿舍休息，回复少量心情"); }
        inline QString previewChoiceC() { return QStringLiteral("去便利店补给，获得临时资源"); }
    }

    // 圣遗物文本。数值来自 GameBalance::Relics。
    namespace RelicText
    {
        inline QString yanyuanMealCardName() { return QStringLiteral("燕园饭卡"); }
        inline QString yanyuanMealCardDescription(int heal)
        {
            return QStringLiteral("战斗胜利后回复 %1 点心情。").arg(heal);
        }
        inline QString yanyuanMealCardDescription()
        {
            return yanyuanMealCardDescription(GameBalance::Relics::yanyuanMealCardHeal());
        }
        inline QString yanyuanMealCardImage() { return QString(); }

        inline QString boyaTowerBadgeName() { return QStringLiteral("博雅塔徽章"); }
        inline QString boyaTowerBadgeDescription(int strength)
        {
            return QStringLiteral("每场战斗开始时，攻击强度 +%1。").arg(strength);
        }
        inline QString boyaTowerBadgeDescription()
        {
            return boyaTowerBadgeDescription(GameBalance::Relics::boyaTowerBadgeStrength());
        }
        inline QString boyaTowerBadgeImage() { return QString(); }

        inline QString librarySeatName() { return QStringLiteral("图书馆座位"); }
        inline QString librarySeatDescription(int block)
        {
            return QStringLiteral("每场战斗开始时，获得 %1 点格挡。").arg(block);
        }
        inline QString librarySeatDescription()
        {
            return librarySeatDescription(GameBalance::Relics::librarySeatBlock());
        }
        inline QString librarySeatImage() { return QString(); }

        inline QString packedSchoolbagName() { return QStringLiteral("满载书包"); }
        inline QString packedSchoolbagDescription(int draw)
        {
            return QStringLiteral("每场战斗开始时，额外抽 %1 张牌。").arg(draw);
        }
        inline QString packedSchoolbagDescription()
        {
            return packedSchoolbagDescription(GameBalance::Relics::packedSchoolbagDraw());
        }
        inline QString packedSchoolbagImage() { return QString(); }

        inline QString luckyCouponName() { return QStringLiteral("百讲折扣券"); }
        inline QString luckyCouponDescription(int discountPercent)
        {
            return QStringLiteral("商店里所有商品价格降低 %1%。").arg(discountPercent);
        }
        inline QString luckyCouponDescription()
        {
            return luckyCouponDescription(GameBalance::Relics::luckyCouponDiscount());
        }
        inline QString luckyCouponImage() { return QString(); }
    }

    // 药水文本。数值来自 GameBalance::Potions。
    namespace PotionText
    {
        inline QString coffeeShotName() { return QStringLiteral("咖啡因冲剂"); }
        inline QString coffeeShotDescription(int damage)
        {
            return QStringLiteral("对敌人造成 %1 点伤害。").arg(damage);
        }
        inline QString coffeeShotDescription()
        {
            return coffeeShotDescription(GameBalance::Potions::coffeeShotDamage());
        }
        inline QString coffeeShotImage() { return QString(); }

        inline QString antiBreakSprayName() { return QStringLiteral("防破防喷雾"); }
        inline QString antiBreakSprayDescription(int block)
        {
            return QStringLiteral("获得 %1 点格挡。").arg(block);
        }
        inline QString antiBreakSprayDescription()
        {
            return antiBreakSprayDescription(GameBalance::Potions::antiBreakBlock());
        }
        inline QString antiBreakSprayImage() { return QString(); }

        inline QString clinicSyrupName() { return QStringLiteral("校医院糖浆"); }
        inline QString clinicSyrupDescription(int heal)
        {
            return QStringLiteral("回复 %1 点心情。").arg(heal);
        }
        inline QString clinicSyrupDescription()
        {
            return clinicSyrupDescription(GameBalance::Potions::clinicSyrupHeal());
        }
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
        inline QString pageTitle() { return QStringLiteral("校园商店"); }
        inline QString leaveButton() { return QStringLiteral("离开商店"); }
        inline QString coinsFormat() { return QStringLiteral("资源点：%1"); }

        inline QString intro(int discountPercent)
        {
            return QStringLiteral("买一点能撑过下个 ddl 的东西。百讲折扣券会让商店价格降低 %1%。")
                .arg(discountPercent);
        }
        inline QString intro() { return intro(GameBalance::Shop::discountPercent()); }

        inline QString relicSectionTitle() { return QStringLiteral("圣遗物"); }
        inline QString cardSectionTitle() { return QStringLiteral("卡牌"); }
        inline QString potionSectionTitle() { return QStringLiteral("药水"); }
        inline QString serviceSectionTitle() { return QStringLiteral("服务"); }

        inline QString buyButtonFormat() { return QStringLiteral("购买 %1"); }
        inline QString boughtButton() { return QStringLiteral("已购买"); }
        inline QString buySuccessFormat() { return QStringLiteral("购买了：%1"); }
        inline QString notEnoughCoinsFormat() { return QStringLiteral("资源点不足，买不起：%1"); }

        inline QString noRelicText() { return QStringLiteral("暂时没有新的圣遗物。"); }
        inline QString noPotionSlotText() { return QStringLiteral("药水栏已满。"); }
        inline QString shopFinishedTip() { return QStringLiteral("离开后该商店节点完成。"); }

        inline QString cardRemovalName() { return QStringLiteral("移除一张卡牌"); }
        inline QString cardRemovalDescription() { return QStringLiteral("从当前牌组中永久删除一张卡牌。"); }
        inline QString cardRemovalDialogTitle() { return QStringLiteral("选择要移除的卡牌"); }
        inline QString cardRemovalSuccess() { return QStringLiteral("卡牌已移除。"); }
        inline QString cardRemovalNotEnoughCoins()
        {
            return QStringLiteral("资源点不足，无法移除卡牌。");
        }
        inline QString emptyDeckText() { return QStringLiteral("当前没有可移除的卡牌。"); }
    }

    namespace RewardText
    {
        inline QString pageTitle() { return QStringLiteral("战斗奖励"); }
        inline QString skipButton() { return QStringLiteral("跳过奖励"); }
        inline QString takeCardButton() { return QStringLiteral("加入牌组"); }
        inline QString finishTip() { return QStringLiteral("奖励结算完成。"); }

        inline QString rewardWithPotion(int coins, const QString &potionName)
        {
            return QStringLiteral("获得 %1 资源点，并得到一瓶%2。选择一张卡加入牌组。")
                .arg(coins)
                .arg(potionName);
        }
        inline QString rewardWithPotion()
        {
            return QStringLiteral("获得 %1 资源点，并得到一瓶咖啡因冲剂。选择一张卡加入牌组。")
                .arg(GameBalance::Rewards::rewardScreenCoins());
        }

        inline QString rewardPotionFull(int coins)
        {
            return QStringLiteral("获得 %1 资源点。药水栏已满，无法获得新的药水。选择一张卡加入牌组。")
                .arg(coins);
        }
        inline QString rewardPotionFull()
        {
            return rewardPotionFull(GameBalance::Rewards::rewardScreenCoins());
        }
    }

    // 调试入口里还没有完整 UI 的占位文本。
    namespace DebugText
    {
        inline QString mapTitle() { return QStringLiteral("地图节点"); }
        inline QString mapBody()
        {
            return QStringLiteral("地图 UI 尚未实现。这里先作为开发占位页，后续接入固定节点路线：战斗、事件、商店、休息、Boss。");
        }
        inline QString rewardTitle() { return QStringLiteral("卡牌奖励"); }
        inline QString rewardBody()
        {
            return QStringLiteral("奖励 UI 尚未实现。后续这里展示三选一卡牌，并把选择结果加入牌组。");
        }
        inline QString shopTitle() { return QStringLiteral("商店"); }
        inline QString shopBody()
        {
            return QStringLiteral("商店 UI 尚未实现。后续这里展示可购买卡牌、遗物和删牌入口。");
        }
        inline QString restTitle() { return QStringLiteral("休息点"); }
        inline QString restBody()
        {
            return QStringLiteral("休息 UI 尚未实现。后续这里提供回复心情、升级卡牌等选项。");
        }
    }

}

#endif // GAMETEXT_H
