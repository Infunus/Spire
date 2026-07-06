#ifndef GAMEBALANCE_H
#define GAMEBALANCE_H

// 所有“影响玩法”的数值集中放在这里，方便快速调平衡。
// UI 尺寸、颜色、边距等纯界面数值暂时不放这里，避免把平衡表弄得太乱。
namespace GameBalance
{
struct CardStats
{
    int cost;               // 打出这张牌消耗的精力。
    int damage;             // 对敌人造成的基础伤害。
    int block;              // 玩家获得的格挡。
    int heal;               // 玩家回复的心情。
    int selfDamage;         // 玩家打出此牌时受到的自伤。
    int weak;               // 给予敌人的虚弱层数。
    int vulnerable;         // 给予敌人的易伤层数。
    int draw;               // 打出后额外抽牌数量。
    int strengthGain;       // 打出后获得的攻击强度。
    int strengthMultiplier; // 攻击强度对本牌伤害的倍率，普通攻击牌通常为 1。
    bool exhaust;           // 打出后是否进入消耗堆，不再洗回抽牌堆。
};

namespace Player
{
inline int startMaxHp() { return 70; }      // 新开局玩家最大心情。
inline int startCoins() { return 60; }      // 新开局初始余额，足够购买一张普通卡牌。
inline int maxPotions() { return 3; }       // 药水栏最大容量。
}

namespace Battle
{
inline int energyPerTurn() { return 3; }             // 每回合默认精力。
inline int cardsDrawnEachTurn() { return 5; }        // 每回合默认抽牌数。
inline int maxHandSize() { return 10; }              // 玩家手牌上限。
inline int bossBattleNumber() { return 4; }          // 第几场战斗视为 Boss 战。
inline int victoryReturnDelayMs() { return 650; }    // 地图战斗胜利后进入奖励页的延迟。
inline int failReturnDelayMs() { return 250; }       // 地图战斗失败后返回地图的延迟。
inline int slashEffectMs() { return 320; }           // 玩家攻击命中敌人的斩击特效时长。
inline int enemyProjectileMs() { return 420; }       // 敌人回合光球飞行动画时长。
inline int enemyActionPauseMs() { return 220; }      // 多个敌人行动之间的停顿时长。
inline int hitFlashMs() { return 230; }              // 受击闪光特效时长。
}

namespace Rewards
{
inline int battleGradeScore() { return 5; }  // 普通战斗胜利增加的平时分。
inline int battleCoins() { return 12; }      // 普通战斗胜利增加的余额。
inline int eventGradeScore() { return 2; }   // 旧接口保留；事件不再默认奖励平时分。
inline int eventCoins() { return 8; }        // 旧接口保留；事件不再默认奖励余额。
inline int bossGradeScore() { return 20; }   // 旧接口保留：Boss 胜利增加的平时分。
inline int bossCoins() { return 30; }        // Boss 胜利增加的余额。
inline int rewardScreenCoins() { return 8; } // 奖励页额外给的余额。
inline int cardChoices() { return 3; }       // 战斗奖励三选一的卡牌数量。
}

namespace CourseGrade
{
inline int usualScoreMax() { return 100; }            // 平时分上限。
inline int finalExamScoreMax() { return 100; }        // 期末成绩上限。
inline int finalTotalScoreMax() { return 100; }       // 总评上限。
inline int enemyScoreDecayPerTurn() { return 1; }     // 每拖 1 回合，敌人可得平时分减少多少。
inline int enemyMinimumScore() { return 2; }          // 单个敌人最低可得平时分。
inline int campusCultistScore() { return 8; }         // 高数作业基础平时分。
inline int homeworkWormScore() { return 10; }         // POJ 习题基础平时分。
inline int ddlSlimeScore() { return 6; }              // 水课作业基础平时分。
inline int projectNobScore() { return 14; }           // 小组汇报基础平时分。
inline int morningAlarmScore() { return 5; }          // 早八闹钟基础平时分。
inline int clubFlyerScore() { return 6; }             // 论文阅读报告基础平时分。
inline int teachingSentryScore() { return 9; }        // 85km打卡基础平时分。
inline int finalExamStartScore() { return 100; }      // 期末考试第 1 回合击败时的成绩。
inline int finalExamDecayPerTurn() { return 4; }      // 期末考试每拖 1 回合扣多少分。
inline int finalExamMinimumScore() { return 60; }     // 击败期末考试后的最低期末成绩。
inline double gpaForTotalScore(int score)
{
    const int boundedScore = score < 0 ? 0 : (score > finalTotalScoreMax() ? finalTotalScoreMax() : score);
    const double delta = 100.0 - boundedScore;
    const double gpa = 4.0 - 3.0 * delta * delta / 1600.0;
    return gpa < 0.0 ? 0.0 : (gpa > 4.0 ? 4.0 : gpa);
}
}

namespace Events
{
inline int libraryNightHeal() { return 8; }           // 图书馆深夜：回宿舍休息回复心情。
inline int libraryNightCoins() { return 12; }         // 图书馆深夜：便利店补给获得资源点。
inline int dormPowerOutageHeal() { return 10; }       // 宿舍停电：顺势早睡回复心情。
inline int elevatorWaitLoseHp() { return 5; }         // 理科楼电梯：继续等待损失心情。
inline int weimingLakeRestHeal() { return 12; }       // 未名湖夜行：放空休息回复心情。
inline int studentAffairsWaitLoseHp() { return 5; }   // 学工群新锅：误触领锅损失心情。
inline int studentAffairsVolunteerLoseHp() { return 10; } // 学工群新锅：主动领锅损失心情。
inline int openDayGoOutHeal() { return 10; }          // 开放日：和朋友出去玩回复心情。
inline int openDayRandomPotionCount() { return 1; }   // 开放日：出去玩获得随机饮料数量。
inline int openDayStudyLoseHp() { return 15; }        // 开放日：图书馆自习损失心情。
inline int openDayVolunteerLoseHp() { return 5; }     // 开放日：志愿者工作损失心情。
inline int ideologyPreSeriousLoseHp() { return 8; }   // 思政课 Pre：认真完成损失心情。
inline int ideologyPreSeriousUsualScore() { return 5; } // 思政课 Pre：认真完成获得平时分。
inline int ideologyPreCoastLoseHp() { return 1; }     // 思政课 Pre：开摆混完损失心情。
inline int ideologyPreFriendsLoseHp() { return 3; }   // 思政课 Pre：朋友组队损失心情。
inline int ideologyPreFriendsUsualScore() { return 5; } // 思政课 Pre：朋友组队获得平时分。
inline int gaokaoBlessingShootLoseHp() { return 1; }  // 高考祝福视频：去拍摄损失心情。
inline int gaokaoBlessingFocusStrength() { return 2; } // 高考祝福视频：婉拒后下回合效率值。
inline int gaokaoBlessingFakeSickLoseHp() { return 5; } // 高考祝福视频：装病被发现损失心情。
inline int clubFairYuanhuoHeal() { return 10; }       // 百团大战：加入元火回复心情。
inline int clubFairYuanhuoCardCount() { return 2; }   // 百团大战：加入元火获得卡牌数量。
inline int clubFairRailwayHeal() { return 10; }       // 百团大战：加入铁协回复心情。
inline int clubFairRailwayCardCount() { return 1; }   // 百团大战：加入铁协获得卡牌数量。
inline int clubFairFreeTimeTurnBlock() { return 5; }  // 百团大战：我再想想，下场战斗每回合抗压值。
inline int wusiNightRunHeal() { return 10; }           // 五四夜奔：参加长跑回复心情。
inline int wusiNightRunXiGaiLoseHp() { return 3; }     // 五四夜奔：上习概损失心情。
inline int wusiNightRunXiGaiUsualScore() { return 3; } // 五四夜奔：上习概获得平时分。
inline int weishenPlainLunchLoseHp() { return 3; }     // 塑料袋里的数学：朴素午餐硬学损失心情。
inline int weishenPlainLunchStrength() { return 2; }   // 塑料袋里的数学：朴素午餐硬学获得效率值。
inline int weishenPlainLunchHeal() { return 8; }       // 塑料袋里的数学：好好吃饭回复心情。
inline int securityBoothTalkHeal() { return 5; }       // 夜课：聊几句回复心情。
inline int securityBoothTalkUsualScore() { return 4; } // 夜课：受到启发获得平时分。
inline int securityBoothStudyLoseHp() { return 5; }    // 夜课：回宿舍继续学习损失心情。
inline int securityBoothStudyUsualScore() { return 5; } // 夜课：回宿舍继续学习获得平时分。
inline int securityBoothRestHeal() { return 8; }       // 夜课：今晚先休息回复心情。
inline int baijiangTicketLoseHp() { return 4; }        // 百讲最后一张票：蹲票损失心情。
inline int baijiangTicketCost() { return 18; }         // 百讲最后一张票：准点蹲票买票花费余额。
inline int baijiangTreeholeHeal() { return 6; }        // 百讲最后一张票：树洞买到票回复心情。
inline int baijiangTreeholeCost() { return 32; }       // 百讲最后一张票：树洞出票略贵，花费余额。
inline int baijiangFocusStrength() { return 2; }       // 百讲最后一张票：把夜晚还给作业获得效率值。
}

namespace Shop
{
inline int discountPercent() { return 15; }      // 物美会员卡的折扣百分比。
inline int relicPrice() { return 75; }           // 圣遗物基础价格。
inline int zeroCostCardPrice() { return 30; }    // 0 费卡基础价格。
inline int cardBasePrice() { return 38; }        // 非 0 费卡基础价格。
inline int cardCostPriceStep() { return 8; }     // 卡牌每 1 点费用额外增加的价格。
inline int potionPrice() { return 25; }          // 药水基础价格。
inline int cardRemovalPrice() { return 60; }     // 商店删牌服务价格。
}

namespace Map
{
inline int layerCount() { return 19; }            // 地图总层数，最后一层是 Boss。
inline int normalRoomsPerLayer() { return 3; }    // 非 Boss 层每层节点数量。
inline int bossRoomIndex() { return 1; }          // Boss 和强制休息节点所在的中间列。
inline int randomRollMax() { return 100; }        // 地图节点随机数上限。
inline int battleThreshold() { return 45; }       // 随机值小于该数时生成战斗。
inline int eventThreshold() { return 65; }        // 随机值小于该数时生成事件。
inline int shopThreshold() { return 78; }         // 随机值小于该数时生成商店。
inline int rewardThreshold() { return 90; }       // 随机值小于该数时生成奖励。
}

namespace Rest
{
inline int sleepHeal() { return 16; }
inline int upgradeCount() { return 2; }
}

namespace Relics
{
inline int yanyuanMealCardHeal() { return 4; }       // 原神月卡：战斗胜利后回复量。
inline int boyaTowerBadgeStrength() { return 1; }    // 博雅塔徽章：战斗开始攻击强度。
inline int librarySeatBlock() { return 8; }          // PKU 棒球服：战斗开始获得抗压值。
inline int packedSchoolbagDraw() { return 1; }       // 红色帆布袋：战斗开始额外抽牌。
inline int luckyCouponDiscount() { return Shop::discountPercent(); } // 物美会员卡：商店折扣。
inline int qingtianObservationGuideStrength() { return 1; } // 青天观测指南：战斗开始效率值。
inline int pkuWatermelonTurnBlock() { return 3; }     // 北大的瓜：每回合开始抗压值。
}

namespace Potions
{
inline int coffeeShotDamage() { return 16; } // 抹茶拿铁：对敌人造成任务量。
inline int antiBreakBlock() { return 10; }   // 蜜桃四季春：获得抗压值。
inline int clinicSyrupHeal() { return 12; }  // 可口可乐：回复心情。
}

namespace EnemyStatus
{
inline int vulnerableBonusDivisor() { return 2; } // 易伤：额外受到 damage / 2 的伤害。
inline int weakDamageNumerator() { return 3; }    // 虚弱：敌人伤害倍率分子。
inline int weakDamageDenominator() { return 4; }  // 虚弱：敌人伤害倍率分母。
inline int fallbackAttack() { return 6; }         // 敌人没有行动表时的默认攻击。
}

namespace Enemies
{
inline int normalEnemyTypes() { return 7; } // 普通战斗随机敌人的种类数。
inline int earlyFloorLimit() { return 6; }  // 低于该层数时使用前期遭遇池。
inline int middleFloorLimit() { return 12; }// 低于该层数时使用中期遭遇池。
inline int earlyEncounterTypes() { return 4; }
inline int middleEncounterTypes() { return 5; }
inline int lateEncounterTypes() { return 5; }

namespace CampusCultist
{
inline int maxHp() { return 42; }       // 绩点传教同学任务量。
inline int firstBuff() { return 3; }    // 第 1 回合强化攻击强度。
inline int attackA() { return 6; }      // 第 2 回合攻击。
inline int attackB() { return 6; }      // 第 3 回合攻击。
}

namespace HomeworkWorm
{
inline int maxHp() { return 48; }       // 高数题面虫任务量。
inline int attackA() { return 7; }      // 攻击并格挡行动的攻击值。
inline int blockA() { return 5; }       // 攻击并格挡行动的格挡值。
inline int buff() { return 3; }         // 强化行动增加的攻击强度。
inline int attackB() { return 11; }     // 强化后的攻击。
}

namespace DdlSlime
{
inline int maxHp() { return 36; }       // DDL 史莱姆任务量。
inline int attackA() { return 8; }      // 第一次攻击。
inline int block() { return 5; }        // 格挡行动获得的格挡。
inline int attackB() { return 10; }     // 第二次攻击。
}

namespace ProjectNob
{
inline int maxHp() { return 74; }       // 程设大作业精英任务量。
inline int attackA() { return 10; }     // 第一次攻击。
inline int attackB() { return 8; }      // 攻击并强化行动的攻击值。
inline int buff() { return 2; }         // 攻击并强化行动增加的攻击强度。
inline int attackC() { return 14; }     // 高压攻击。
}

namespace MorningAlarm
{
inline int maxHp() { return 13; }       // 参考杀尖虱虫的小血量。
inline int attackA() { return 6; }      // Bite：咬一口。
inline int buff() { return 3; }         // Grow：力量 +3。
inline int attackB() { return 6; }      // 再次攻击。
}

namespace ClubFlyer
{
inline int maxHp() { return 28; }       // 参考杀尖真菌兽的高血量区间。
inline int attack() { return 6; }       // Bite：稳定攻击 6。
inline int buff() { return 3; }         // Grow：力量 +3。
}

namespace TeachingSentry
{
inline int maxHp() { return 39; }       // 参考杀尖哨卫。
inline int attack() { return 9; }       // Beam：攻击 9。
inline int block() { return 8; }        // 用复杂度近似原版塞废牌带来的压力。
}

namespace FinalExam
{
inline int maxHp() { return 110; }      // 期末考试 Boss 任务量。
inline int attackA() { return 14; }     // 第一次攻击。
inline int attackB() { return 10; }     // 攻击并格挡行动的攻击值。
inline int block() { return 8; }        // 攻击并格挡行动的格挡值。
inline int buff() { return 4; }         // 强化行动增加的攻击强度。
inline int attackC() { return 20; }     // 高压攻击。
}
}

namespace Cards
{
inline CardStats strike() { return {1, 6, 0, 0, 0, 0, 0, 0, 0, 1, false}; }       // 刷题基础版。
inline CardStats strikeUpgraded() { return {1, 9, 0, 0, 0, 0, 0, 0, 0, 1, false}; } // 刷题强化版。

inline CardStats defend() { return {1, 0, 5, 0, 0, 0, 0, 0, 0, 1, false}; }       // 防破防基础版。
inline CardStats defendUpgraded() { return {1, 0, 8, 0, 0, 0, 0, 0, 0, 1, false}; } // 防破防强化版。

inline CardStats bash() { return {2, 8, 0, 0, 0, 0, 2, 0, 0, 1, false}; }         // 当堂点名基础版。
inline CardStats bashUpgraded() { return {2, 10, 0, 0, 0, 0, 3, 0, 0, 1, false}; } // 当堂点名强化版。

inline CardStats inflame() { return {1, 0, 0, 0, 0, 0, 0, 0, 2, 1, true}; }       // 鸡血动员基础版。
inline CardStats inflameUpgraded() { return {1, 0, 0, 0, 0, 0, 0, 0, 3, 1, true}; } // 鸡血动员强化版。

inline CardStats heavyBlade() { return {2, 14, 0, 0, 0, 0, 0, 0, 0, 3, false}; }  // 大题重击基础版。
inline CardStats heavyBladeUpgraded() { return {2, 14, 0, 0, 0, 0, 0, 0, 0, 5, false}; } // 大题重击强化版。

inline CardStats pommel() { return {1, 7, 0, 0, 0, 0, 0, 1, 0, 1, false}; }       // 边写边想基础版。
inline CardStats pommelUpgraded() { return {1, 9, 0, 0, 0, 0, 0, 2, 0, 1, false}; } // 边写边想强化版。

inline CardStats shrug() { return {1, 0, 7, 0, 0, 0, 0, 1, 0, 1, false}; }        // 缓口气基础版。
inline CardStats shrugUpgraded() { return {1, 0, 10, 0, 0, 0, 0, 1, 0, 1, false}; } // 缓口气强化版。

inline CardStats anger() { return {0, 4, 0, 0, 0, 0, 0, 0, 0, 1, false}; }        // 灵感速写基础版。
inline CardStats angerUpgraded() { return {0, 6, 0, 0, 0, 0, 0, 0, 0, 1, false}; } // 灵感速写强化版。

inline CardStats flex() { return {0, 0, 0, 0, 0, 0, 0, 1, 1, 1, true}; }          // 临时鸡血基础版。
inline CardStats flexUpgraded() { return {0, 0, 0, 0, 0, 0, 0, 1, 2, 1, true}; }  // 临时鸡血强化版。

inline CardStats cleave() { return {1, 8, 0, 0, 0, 0, 0, 0, 0, 1, false}; }       // 知识点横扫基础版。
inline CardStats cleaveUpgraded() { return {1, 11, 0, 0, 0, 0, 0, 0, 0, 1, false}; } // 知识点横扫强化版。
}
}

#endif // GAMEBALANCE_H
