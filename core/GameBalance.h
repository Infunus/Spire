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
    int heal;               // 玩家回复的精力/生命。
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
inline int startMaxHp() { return 70; }      // 新开局玩家最大精力/生命。
inline int startCoins() { return 99; }      // 新开局初始资源点。
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
inline int battleGradeScore() { return 5; }  // 普通战斗胜利增加的评分。
inline int battleCredits() { return 1; }     // 普通战斗胜利增加的学分。
inline int battleCoins() { return 20; }      // 普通战斗胜利增加的资源点。
inline int eventGradeScore() { return 2; }   // 随机事件完成增加的评分。
inline int eventCoins() { return 8; }        // 随机事件完成增加的资源点。
inline int bossGradeScore() { return 20; }   // Boss 胜利增加的评分。
inline int bossCredits() { return 3; }       // Boss 胜利增加的学分。
inline int bossCoins() { return 60; }        // Boss 胜利增加的资源点。
inline int rewardScreenCoins() { return 20; }// 战斗奖励页额外给的资源点。
inline int cardChoices() { return 3; }       // 战斗奖励三选一的卡牌数量。
}

namespace Gpa
{
inline int gradeAThreshold() { return 90; }   // GPA 4.0 的评分线。
inline int gradeAMinusThreshold() { return 85; } // GPA 3.7 的评分线。
inline int gradeBPlusThreshold() { return 80; }  // GPA 3.3 的评分线。
inline int gradeBThreshold() { return 75; }      // GPA 3.0 的评分线。
inline int gradeBMinusThreshold() { return 70; } // GPA 2.7 的评分线。
inline int passThreshold() { return 60; }        // GPA 2.0 的及格线。
inline double gradeAGpa() { return 4.0; }        // A 档绩点。
inline double gradeAMinusGpa() { return 3.7; }   // A- 档绩点。
inline double gradeBPlusGpa() { return 3.3; }    // B+ 档绩点。
inline double gradeBGpa() { return 3.0; }        // B 档绩点。
inline double gradeBMinusGpa() { return 2.7; }   // B- 档绩点。
inline double passGpa() { return 2.0; }          // 及格绩点。
inline double failGpa() { return 0.0; }          // 不及格绩点。
}

namespace Shop
{
inline int discountPercent() { return 10; }      // 百讲折扣券的折扣百分比。
inline int relicPrice() { return 80; }           // 圣遗物基础价格。
inline int zeroCostCardPrice() { return 35; }    // 0 费卡基础价格。
inline int cardBasePrice() { return 45; }        // 非 0 费卡基础价格。
inline int cardCostPriceStep() { return 8; }     // 卡牌每 1 点费用额外增加的价格。
inline int potionPrice() { return 30; }          // 药水基础价格。
inline int cardRemovalPrice() { return 75; }     // 商店删牌服务价格。
}

namespace Map
{
inline int layerCount() { return 7; }             // 地图总层数，最后一层是 Boss。
inline int normalRoomsPerLayer() { return 3; }    // 非 Boss 层每层节点数量。
inline int bossRoomIndex() { return 1; }          // Boss 和强制休息节点所在的中间列。
inline int randomRollMax() { return 100; }        // 地图节点随机数上限。
inline int battleThreshold() { return 45; }       // 随机值小于该数时生成战斗。
inline int eventThreshold() { return 65; }        // 随机值小于该数时生成事件。
inline int shopThreshold() { return 78; }         // 随机值小于该数时生成商店。
inline int rewardThreshold() { return 90; }       // 随机值小于该数时生成奖励。
}

namespace Relics
{
inline int yanyuanMealCardHeal() { return 6; }       // 燕园饭卡：战斗胜利后回复量。
inline int boyaTowerBadgeStrength() { return 1; }    // 博雅塔徽章：战斗开始攻击强度。
inline int librarySeatBlock() { return 10; }         // 图书馆座位：战斗开始格挡。
inline int packedSchoolbagDraw() { return 2; }       // 满载书包：战斗开始额外抽牌。
inline int luckyCouponDiscount() { return Shop::discountPercent(); } // 百讲折扣券：商店折扣。
}

namespace Potions
{
inline int coffeeShotDamage() { return 20; } // 咖啡因冲剂：对敌人造成伤害。
inline int antiBreakBlock() { return 12; }   // 防破防喷雾：获得格挡。
inline int clinicSyrupHeal() { return 15; }  // 校医院糖浆：回复生命/精力。
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
inline int normalEnemyTypes() { return 4; } // 普通战斗随机敌人的种类数。

namespace CampusCultist
{
inline int maxHp() { return 42; }       // 绩点传教同学生命。
inline int firstBuff() { return 3; }    // 第 1 回合强化攻击强度。
inline int attackA() { return 6; }      // 第 2 回合攻击。
inline int attackB() { return 6; }      // 第 3 回合攻击。
}

namespace HomeworkWorm
{
inline int maxHp() { return 48; }       // 高数题面虫生命。
inline int attackA() { return 7; }      // 攻击并格挡行动的攻击值。
inline int blockA() { return 5; }       // 攻击并格挡行动的格挡值。
inline int buff() { return 3; }         // 强化行动增加的攻击强度。
inline int attackB() { return 11; }     // 强化后的攻击。
}

namespace DdlSlime
{
inline int maxHp() { return 36; }       // DDL 史莱姆生命。
inline int attackA() { return 8; }      // 第一次攻击。
inline int block() { return 5; }        // 格挡行动获得的格挡。
inline int attackB() { return 10; }     // 第二次攻击。
}

namespace ProjectNob
{
inline int maxHp() { return 74; }       // 程设大作业精英生命。
inline int attackA() { return 10; }     // 第一次攻击。
inline int attackB() { return 8; }      // 攻击并强化行动的攻击值。
inline int buff() { return 2; }         // 攻击并强化行动增加的攻击强度。
inline int attackC() { return 14; }     // 高压攻击。
}

namespace FinalExam
{
inline int maxHp() { return 110; }      // 期末考试 Boss 生命。
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

inline CardStats pommel() { return {1, 9, 0, 0, 0, 0, 0, 1, 0, 1, false}; }       // 边写边想基础版。
inline CardStats pommelUpgraded() { return {1, 10, 0, 0, 0, 0, 0, 2, 0, 1, false}; } // 边写边想强化版。

inline CardStats shrug() { return {1, 0, 8, 0, 0, 0, 0, 1, 0, 1, false}; }        // 缓口气基础版。
inline CardStats shrugUpgraded() { return {1, 0, 11, 0, 0, 0, 0, 1, 0, 1, false}; } // 缓口气强化版。

inline CardStats anger() { return {0, 4, 0, 0, 0, 0, 0, 0, 0, 1, false}; }        // 灵感速写基础版。
inline CardStats angerUpgraded() { return {0, 6, 0, 0, 0, 0, 0, 0, 0, 1, false}; } // 灵感速写强化版。

inline CardStats flex() { return {0, 0, 0, 0, 0, 0, 0, 1, 3, 1, true}; }          // 临时鸡血基础版。
inline CardStats flexUpgraded() { return {0, 0, 0, 0, 0, 0, 0, 1, 5, 1, true}; }  // 临时鸡血强化版。

inline CardStats cleave() { return {1, 8, 0, 0, 0, 0, 0, 0, 0, 1, false}; }       // 知识点横扫基础版。
inline CardStats cleaveUpgraded() { return {1, 11, 0, 0, 0, 0, 0, 0, 0, 1, false}; } // 知识点横扫强化版。
}
}

#endif // GAMEBALANCE_H
