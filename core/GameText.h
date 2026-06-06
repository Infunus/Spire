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
        inline QString battleMusic() { return QStringLiteral("resources/audio/battle_ddl.mp3"); }
        inline QString worldMusicBoyaTodo() { return QStringLiteral("resources/audio/world_boya_todo.mp3"); }
        inline QString worldMusicYanyuanDay() { return QStringLiteral("resources/audio/world_yanyuan_day.mp3"); }
        inline QString worldMusicPkuGrinder() { return QStringLiteral("resources/audio/world_pku_grinder.mp3"); }
        inline QString worldMusicLakeWind() { return QStringLiteral("resources/audio/world_lake_wind.mp3"); }
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
        inline QString potionTitle() { return QStringLiteral("饮料"); }

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

        inline QString battleTitleFormat() { return QStringLiteral("第 %1 战"); }
        inline QString runClearTitle() { return QStringLiteral("通关 · 总评"); }
        inline QString runClearTitleFormat() { return QStringLiteral("通关 · 总评 %1"); }
        inline QString battleWinFormat() { return QStringLiteral("胜利 · 击败 %1 个敌人"); }
        inline QString battleFailTitle() { return QStringLiteral("喜报：您已被退学！"); }
        inline QString victoryTip() { return QStringLiteral("完成作业！助教正在批阅……"); }
        inline QString battleVictoryTipFormat()
        {
            return QStringLiteral("完成课程作业，获得 %1 平时分。当前平时分：%2");
        }
        inline QString finalVictoryTipFormat()
        {
            return QStringLiteral("期末成绩：%1；平时分：%2；总评：%3");
        }
        inline QString failTip() { return QStringLiteral("心情归零，本次修读失败"); }

        inline QString playerHpFormat() { return QStringLiteral("心情 %1/%2"); }
        inline QString enemyHpFormat() { return QStringLiteral("任务量 %1/%2"); }
        inline QString hpFormat() { return enemyHpFormat(); } // 旧接口，后续优先用 playerHpFormat / enemyHpFormat。
        inline QString strengthLabelFormat() { return QStringLiteral("效率值 +%1"); }
        inline QString playerInfoFormat()
        {
            return QStringLiteral("回合 %1  |  精力 %2/%3  |  效率值 +%4  |  平时分 %5");
        }
        inline QString usualScoreRewardFormat() { return QStringLiteral("击败 +%1 平时分"); }
        inline QString finalExamScoreFormat() { return QStringLiteral("期末成绩 %1"); }
        inline QString scoreRewardTooltipFormat()
        {
            return QStringLiteral("基础分会随回合数下降：当前第 %1 回合");
        }

        inline QString playerBlockStatusName() { return QStringLiteral("抗压值"); }
        inline QString playerBlockStatusRules() { return QStringLiteral("抗压值：先抵消敌人造成的心情损失"); }
        inline QString playerStrengthStatusName() { return QStringLiteral("效率值"); }
        inline QString playerStrengthStatusShort() { return QStringLiteral("效"); }
        inline QString playerStrengthStatusRules() { return QStringLiteral("效率值：你的攻击牌造成的任务量增加"); }
        inline QString playerVulnerableStatusName() { return QStringLiteral("破防"); }
        inline QString playerVulnerableStatusShort() { return QStringLiteral("破"); }
        inline QString playerVulnerableStatusRules() { return QStringLiteral("破防：受到的心情损失提高 50%"); }
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
        inline QString emptyPileText() { return QStringLiteral("这里暂时没有牌"); }
        inline QString exhaustTag() { return QStringLiteral("消耗"); }

        inline QString potionEmptyText() { return QStringLiteral("空"); }
        inline QString potionUseTooltip(const QString &description)
        {
            return QStringLiteral("%1\n拖动到目标使用").arg(description);
        }
        inline QString handTipText() { return QStringLiteral(""); }
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
        inline QString strikeImage() { return QStringLiteral("resources/cards/card_strike_homework_battle.png"); }

        inline QString defendName() { return QStringLiteral("刷会儿b站"); }
        inline QString defendDescription(int block) { return QStringLiteral("获得 %1 点抗压值").arg(block); }
        inline QString defendDescription() { return defendDescription(GameBalance::Cards::defend().block); }
        inline QString defendImage() { return QStringLiteral("resources/cards/card_defend_video_break.png"); }

        inline QString bashName() { return QStringLiteral("这题我见过！"); }
        inline QString bashDescription(int damage, int vulnerable)
        {
            return QStringLiteral("完成 %1 点任务量\n给予 %2 层思路突破").arg(damage).arg(vulnerable);
        }
        inline QString bashDescription()
        {
            return bashDescription(GameBalance::Cards::bash().damage, GameBalance::Cards::bash().vulnerable);
        }
        inline QString bashImage() { return QStringLiteral("resources/cards/card_bash_seen_problem.png"); }

        inline QString inflameName() { return QStringLiteral("思路打开"); }
        inline QString inflameDescription(int strengthGain)
        {
            return QStringLiteral("本场战斗效率值 +%1").arg(strengthGain);
        }
        inline QString inflameDescription()
        {
            return inflameDescription(GameBalance::Cards::inflame().strengthGain);
        }
        inline QString inflameImage() { return QStringLiteral("resources/cards/card_inflame_idea_open.png"); }

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
        inline QString heavyBladeImage() { return QStringLiteral("resources/cards/card_heavy_blade_flow.png"); }

        inline QString pommelName() { return QStringLiteral("我觉得我会了"); }
        inline QString pommelDescription(int damage, int draw)
        {
            return QStringLiteral("完成 %1 点任务量\n抽 %2 张牌").arg(damage).arg(draw);
        }
        inline QString pommelDescription()
        {
            return pommelDescription(GameBalance::Cards::pommel().damage, GameBalance::Cards::pommel().draw);
        }
        inline QString pommelImage() { return QStringLiteral("resources/cards/card_pommel_i_get_it.png"); }

        inline QString shrugName() { return QStringLiteral("原神，启动！"); }
        inline QString shrugDescription(int block, int draw)
        {
            return QStringLiteral("获得 %1 点抗压值\n抽 %2 张牌").arg(block).arg(draw);
        }
        inline QString shrugDescription()
        {
            return shrugDescription(GameBalance::Cards::shrug().block, GameBalance::Cards::shrug().draw);
        }
        inline QString shrugImage() { return QStringLiteral("resources/cards/card_shrug_game_break.png"); }

        inline QString angerName() { return QStringLiteral("灵感，稍纵即逝！"); }
        inline QString angerDescription(int damage)
        {
            return QStringLiteral("完成 %1 点任务量\n不消耗精力").arg(damage);
        }
        inline QString angerDescription() { return angerDescription(GameBalance::Cards::anger().damage); }
        inline QString angerImage() { return QStringLiteral("resources/cards/card_anger_flash_inspiration.png"); }

        inline QString flexName() { return QStringLiteral("来根士力架！"); }
        inline QString flexDescription(int strengthGain, int draw)
        {
            return QStringLiteral("本场战斗效率值 +%1\n抽 %2 张牌").arg(strengthGain).arg(draw);
        }
        inline QString flexDescription()
        {
            return flexDescription(GameBalance::Cards::flex().strengthGain, GameBalance::Cards::flex().draw);
        }
        inline QString flexImage() { return QStringLiteral("resources/cards/card_flex_energy_bar.png"); }

        inline QString cleaveName() { return QStringLiteral("今晚必拿下"); }
        inline QString cleaveDescription(int damage) { return QStringLiteral("完成 %1 点任务量").arg(damage); }
        inline QString cleaveDescription() { return cleaveDescription(GameBalance::Cards::cleave().damage); }
        inline QString cleaveImage() { return QStringLiteral("resources/cards/card_cleave_tonight_clear.png"); }
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
            return QStringLiteral("破坏心情：敌人行动时会让玩家失去 %1 点心情").arg(damage);
        }
        inline QString healIntentRules(int amount)
        {
            return QStringLiteral("回复：敌人行动时恢复 %1 点任务量").arg(amount);
        }
        inline QString buffIntentRules(int amount)
        {
            return QStringLiteral("压力值：敌人后续攻击伤害增加 %1").arg(amount);
        }
        inline QString blockIntentRules(int amount)
        {
            return QStringLiteral("复杂度：敌人行动时获得 %1 点护甲，先抵消受到的任务量").arg(amount);
        }
        inline QString attackAndBuffIntentRules(int damage, int strength)
        {
            return QStringLiteral("先让玩家失去 %1 点心情，再让后续攻击伤害增加 %2")
                .arg(damage)
                .arg(strength);
        }
        inline QString attackAndBlockIntentRules(int damage, int block)
        {
            return QStringLiteral("先让玩家失去 %1 点心情，再获得 %2 点护甲")
                .arg(damage)
                .arg(block);
        }

        inline QString weakStatusName() { return QStringLiteral("疲劳"); }
        inline QString weakStatusShort() { return QStringLiteral("疲"); }
        inline QString weakStatusRules() { return QStringLiteral("虚弱：攻击造成的伤害降低"); }
        inline QString vulnerableStatusName() { return QStringLiteral("思路突破"); }
        inline QString vulnerableStatusShort() { return QStringLiteral("突"); }
        inline QString vulnerableStatusRules() { return QStringLiteral("思路突破：受到的任务量提高 50%"); }
        inline QString strengthStatusName() { return QStringLiteral("压力值"); }
        inline QString strengthStatusShort() { return QStringLiteral("压"); }
        inline QString strengthStatusRules() { return QStringLiteral("后续攻击伤害增加"); }
        inline QString blockStatusName() { return QStringLiteral("复杂度"); }
        inline QString blockStatusRules() { return QStringLiteral("先抵消受到的任务量"); }

        inline QString weakStatusFormat() { return weakStatusName() + QStringLiteral("%1"); }
        inline QString vulnerableStatusFormat() { return vulnerableStatusName() + QStringLiteral("%1"); }
        inline QString strengthStatusFormat() { return strengthStatusName() + QStringLiteral("+%1"); }
        inline QString blockStatusFormat() { return blockStatusName() + QStringLiteral("%1"); }
        inline QString statusSeparator() { return QStringLiteral(" | "); }
        inline QString statusJoiner() { return QStringLiteral(" "); }

        inline QString campusCultistName() { return QStringLiteral("高数作业"); }
        inline QString campusCultistDescription() { return QStringLiteral("数学不会那就是真不会"); }
        inline QString campusCultistImage() { return QStringLiteral("resources/enemies/enemy_gaoshu_homework.png"); }

        inline QString homeworkWormName() { return QStringLiteral("POJ习题"); }
        inline QString homeworkWormDescription()
        {
            return QStringLiteral("邪恶的编程题！");
        }
        inline QString homeworkWormImage() { return QStringLiteral("resources/enemies/enemy_poj_problem.png"); }

        inline QString ddlSlimeName() { return QStringLiteral("水课作业"); }
        inline QString ddlSlimeDescription()
        {
            return QStringLiteral("水课还是好糊弄的……吧");
        }
        inline QString ddlSlimeImage() { return QStringLiteral("resources/enemies/enemy_easy_course_slime.png"); }

        inline QString projectNobName() { return QStringLiteral("小组汇报"); }
        inline QString projectNobDescription() { return QStringLiteral("“所以我们的主题选什么好呢……？”“谁会做PPT？”“交给AI算了……”“……我不上台讲，别的都好说”“诶？大家怎么都做完了，记得把我名字加上去”……"); }
        inline QString projectNobImage() { return QStringLiteral("resources/enemies/enemy_group_presentation.png"); }

        inline QString morningAlarmName() { return QStringLiteral("早八闹钟"); }
        inline QString morningAlarmDescription()
        {
            return QStringLiteral("血量低，但如果拖着不管，会越响越烦");
        }
        inline QString morningAlarmImage() { return QStringLiteral("resources/enemies/enemy_morning_alarm_louse.png"); }

        inline QString clubFlyerName() { return QStringLiteral("论文阅读报告"); }
        inline QString clubFlyerDescription()
        {
            return QStringLiteral("这个英语课任务量怎么这么大？");
        }
        inline QString clubFlyerImage() { return QStringLiteral("resources/enemies/enemy_club_flyer_fungus.png"); }

        inline QString teachingSentryName() { return QStringLiteral("85km打卡"); }
        inline QString teachingSentryDescription()
        {
            return QStringLiteral("稳定施压，每周都要为此精疲力尽");
        }
        inline QString teachingSentryImage() { return QStringLiteral("resources/enemies/enemy_teaching_sentry.png"); }

        inline QString finalExamName() { return QStringLiteral("期末考试"); }
        inline QString finalExamDescription()
        {
            return QStringLiteral("不调分！！");
        }
        inline QString finalExamImage() { return QStringLiteral("resources/enemies/enemy_final_exam.png"); }
    }

    // 随机事件 UI 固定文案。具体事件内容在 EventLibrary.h。
    namespace EventText
    {
        inline QString pageTitle() { return QStringLiteral("随机事件"); }
        inline QString eventImagePlaceholder() { return QStringLiteral("事件图"); }
        inline QString defaultTitle() { return QStringLiteral("未命名事件"); }
        inline QString defaultBody()
        {
            return QStringLiteral("在这里填写事件正文。可以写成北大校园生活里的一个选择场景");
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
        inline QString previewChoiceC() { return QStringLiteral("去便利店补给，获得 Money"); }
    }

    // 圣遗物文本。数值来自 GameBalance::Relics。
    namespace RelicText
    {
        inline QString yanyuanMealCardName() { return QStringLiteral("原神月卡"); }
        inline QString yanyuanMealCardDescription(int heal)
        {
            return QStringLiteral("战斗胜利后回复 %1 点心情").arg(heal);
        }
        inline QString yanyuanMealCardDescription()
        {
            return yanyuanMealCardDescription(GameBalance::Relics::yanyuanMealCardHeal());
        }
        inline QString yanyuanMealCardImage() { return QString(); }

        inline QString boyaTowerBadgeName() { return QStringLiteral("信科鼠标垫"); }
        inline QString boyaTowerBadgeDescription(int strength)
        {
            return QStringLiteral("每场战斗开始时，效率值 +%1").arg(strength);
        }
        inline QString boyaTowerBadgeDescription()
        {
            return boyaTowerBadgeDescription(GameBalance::Relics::boyaTowerBadgeStrength());
        }
        inline QString boyaTowerBadgeImage() { return QString(); }

        inline QString librarySeatName() { return QStringLiteral("PKU棒球服"); }
        inline QString librarySeatDescription(int block)
        {
            return QStringLiteral("每场战斗开始时，获得 %1 点抗压值").arg(block);
        }
        inline QString librarySeatDescription()
        {
            return librarySeatDescription(GameBalance::Relics::librarySeatBlock());
        }
        inline QString librarySeatImage() { return QString(); }

        inline QString packedSchoolbagName() { return QStringLiteral("红色的帆布袋"); }
        inline QString packedSchoolbagDescription(int draw)
        {
            return QStringLiteral("每场战斗开始时，额外抽 %1 张牌").arg(draw);
        }
        inline QString packedSchoolbagDescription()
        {
            return packedSchoolbagDescription(GameBalance::Relics::packedSchoolbagDraw());
        }
        inline QString packedSchoolbagImage() { return QString(); }

        inline QString luckyCouponName() { return QStringLiteral("物美会员卡"); }
        inline QString luckyCouponDescription(int discountPercent)
        {
            return QStringLiteral("物美里所有商品价格降低 %1%").arg(discountPercent);
        }
        inline QString luckyCouponDescription()
        {
            return luckyCouponDescription(GameBalance::Relics::luckyCouponDiscount());
        }
        inline QString luckyCouponImage() { return QString(); }

        inline QString qingtianObservationGuideName() { return QStringLiteral("青天观测指南"); }
        inline QString qingtianObservationGuideDescription(int strength)
        {
            return QStringLiteral("每场战斗开始时，效率值 +%1").arg(strength);
        }
        inline QString qingtianObservationGuideDescription()
        {
            return qingtianObservationGuideDescription(GameBalance::Relics::qingtianObservationGuideStrength());
        }
        inline QString qingtianObservationGuideImage() { return QString(); }

        inline QString pkuWatermelonName() { return QStringLiteral("北大的瓜"); }
        inline QString pkuWatermelonDescription(int block)
        {
            return QStringLiteral("战斗中每回合开始时，获得 %1 点抗压值").arg(block);
        }
        inline QString pkuWatermelonDescription()
        {
            return pkuWatermelonDescription(GameBalance::Relics::pkuWatermelonTurnBlock());
        }
        inline QString pkuWatermelonImage() { return QString(); }
    }

    // 药水文本。数值来自 GameBalance::Potions。
    namespace PotionText
    {
        inline QString coffeeShotName() { return QStringLiteral("抹茶拿铁"); }
        inline QString coffeeShotDescription(int damage)
        {
            return QStringLiteral("对指定敌人完成 %1 点任务量").arg(damage);
        }
        inline QString coffeeShotDescription()
        {
            return coffeeShotDescription(GameBalance::Potions::coffeeShotDamage());
        }
        inline QString coffeeShotImage() { return QString(); }

        inline QString antiBreakSprayName() { return QStringLiteral("蜜桃四季春"); }
        inline QString antiBreakSprayDescription(int block)
        {
            return QStringLiteral("获得 %1 点抗压值").arg(block);
        }
        inline QString antiBreakSprayDescription()
        {
            return antiBreakSprayDescription(GameBalance::Potions::antiBreakBlock());
        }
        inline QString antiBreakSprayImage() { return QString(); }

        inline QString clinicSyrupName() { return QStringLiteral("可口可乐"); }
        inline QString clinicSyrupDescription(int heal)
        {
            return QStringLiteral("回复 %1 点心情").arg(heal);
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

        inline QString battleNodeName() { return QStringLiteral("课程任务"); }
        inline QString eventNodeName() { return QStringLiteral("神秘事件"); }
        inline QString shopNodeName() { return QStringLiteral("物美超市"); }
        inline QString restNodeName() { return QStringLiteral("宿舍"); }
        inline QString rewardNodeName() { return QStringLiteral("顺手的事"); }
        inline QString bossNodeName() { return QStringLiteral("期末"); }

        inline QString completedNodeSuffix() { return QStringLiteral("已完成"); }
        inline QString lockedTooltip() { return QStringLiteral("先完成上一层节点"); }
        inline QString availableTooltip() { return QStringLiteral("点击进入该节点"); }
        inline QString selectedTooltip() { return QStringLiteral("当前正在处理该节点"); }
    }

    namespace ShopText
    {
        inline QString pageTitle() { return QStringLiteral("物美超市"); }
        inline QString leaveButton() { return QStringLiteral("离开物美"); }
        inline QString coinsFormat() { return QStringLiteral("余额：%1"); }

        inline QString intro(int discountPercent)
        {
            return QStringLiteral("来自物美的神秘小物品。物美会员卡会让商店价格降低 %1%\n 我没收他们广告费")
                .arg(discountPercent);
        }
        inline QString intro() { return intro(GameBalance::Shop::discountPercent()); }

        inline QString relicSectionTitle() { return QStringLiteral("圣遗物"); }
        inline QString cardSectionTitle() { return QStringLiteral("卡牌"); }
        inline QString potionSectionTitle() { return QStringLiteral("饮料"); }
        inline QString serviceSectionTitle() { return QStringLiteral("服务"); }

        inline QString buyButtonFormat() { return QStringLiteral("购买 %1"); }
        inline QString boughtButton() { return QStringLiteral("已购买"); }
        inline QString buySuccessFormat() { return QStringLiteral("购买了：%1"); }
        inline QString notEnoughCoinsFormat() { return QStringLiteral("余额不足，买不起：%1"); }

        inline QString noRelicText() { return QStringLiteral("暂时没有新的圣遗物"); }
        inline QString noPotionSlotText() { return QStringLiteral("饮料栏已满"); }
        inline QString shopFinishedTip() { return QStringLiteral("离开后该商店节点完成"); }

        inline QString cardRemovalName() { return QStringLiteral("移除一张卡牌"); }
        inline QString cardRemovalDescription() { return QStringLiteral("从当前牌组中永久删除一张卡牌"); }
        inline QString cardRemovalDialogTitle() { return QStringLiteral("选择要移除的卡牌"); }
        inline QString cardRemovalSuccess() { return QStringLiteral("卡牌已移除"); }
        inline QString cardRemovalNotEnoughCoins()
        {
            return QStringLiteral("余额不足，无法删牌");
        }
        inline QString emptyDeckText() { return QStringLiteral("当前没有可移除的卡牌"); }
    }

    namespace RewardText
    {
        inline QString pageTitle() { return QStringLiteral("战斗奖励"); }
        inline QString skipButton() { return QStringLiteral("跳过奖励"); }
        inline QString takeCardButton() { return QStringLiteral("加入牌组"); }
        inline QString finishTip() { return QStringLiteral("奖励结算完成"); }

        inline QString rewardWithPotion(int coins, const QString &potionName)
        {
            return QStringLiteral("获得 %1 Money，并得到一瓶 %2。选择一张卡加入牌组")
                .arg(coins)
                .arg(potionName);
        }
        inline QString rewardWithPotion()
        {
            return QStringLiteral("获得 %1 Money，并得到一瓶 抹茶拿铁。选择一张卡加入牌组")
                .arg(GameBalance::Rewards::rewardScreenCoins());
        }

        inline QString rewardPotionFull(int coins)
        {
            return QStringLiteral("获得 %1 Money。饮料栏已满，无法获得新的饮料。选择一张卡加入牌组")
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
