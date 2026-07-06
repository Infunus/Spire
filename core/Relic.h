#ifndef RELIC_H
#define RELIC_H

#include "GameBalance.h"
#include "GameText.h"

#include <QList>
#include <QString>

enum class RelicEffectType
{
    None,
    StartCombatBlock,
    TurnStartBlock,
    StartCombatStrength,
    StartCombatDraw,
    EndCombatHeal,
    ShopDiscount
};

struct RelicData
{
    QString id;
    QString name;
    QString description;
    QString imagePath;
    RelicEffectType effectType;
    int amount;

    RelicData()
        : effectType(RelicEffectType::None),
          amount(0)
    {
    }

    RelicData(const QString &relicId,
              const QString &relicName,
              const QString &relicDescription,
              const QString &relicImagePath,
              RelicEffectType relicEffectType,
              int relicAmount)
        : id(relicId),
          name(relicName),
          description(relicDescription),
          imagePath(relicImagePath),
          effectType(relicEffectType),
          amount(relicAmount)
    {
    }
};

namespace RelicIds
{
inline QString yanyuanMealCard() { return QStringLiteral("yanyuan_meal_card"); }
inline QString boyaTowerBadge() { return QStringLiteral("boya_tower_badge"); }
inline QString librarySeat() { return QStringLiteral("library_seat"); }
inline QString packedSchoolbag() { return QStringLiteral("packed_schoolbag"); }
inline QString luckyCoupon() { return QStringLiteral("lucky_coupon"); }
inline QString qingtianObservationGuide() { return QStringLiteral("qingtian_observation_guide"); }
inline QString pkuWatermelon() { return QStringLiteral("pku_watermelon"); }
}

namespace RelicLibrary
{
inline RelicData createYanyuanMealCard()
{
    return RelicData(RelicIds::yanyuanMealCard(),
                     GameText::RelicText::yanyuanMealCardName(),
                     GameText::RelicText::yanyuanMealCardDescription(GameBalance::Relics::yanyuanMealCardHeal()),
                     GameText::RelicText::yanyuanMealCardImage(),
                     RelicEffectType::EndCombatHeal,
                     GameBalance::Relics::yanyuanMealCardHeal());
}

inline RelicData createBoyaTowerBadge()
{
    return RelicData(RelicIds::boyaTowerBadge(),
                     GameText::RelicText::boyaTowerBadgeName(),
                     GameText::RelicText::boyaTowerBadgeDescription(GameBalance::Relics::boyaTowerBadgeStrength()),
                     GameText::RelicText::boyaTowerBadgeImage(),
                     RelicEffectType::StartCombatStrength,
                     GameBalance::Relics::boyaTowerBadgeStrength());
}

inline RelicData createLibrarySeat()
{
    return RelicData(RelicIds::librarySeat(),
                     GameText::RelicText::librarySeatName(),
                     GameText::RelicText::librarySeatDescription(GameBalance::Relics::librarySeatBlock()),
                     GameText::RelicText::librarySeatImage(),
                     RelicEffectType::StartCombatBlock,
                     GameBalance::Relics::librarySeatBlock());
}

inline RelicData createPackedSchoolbag()
{
    return RelicData(RelicIds::packedSchoolbag(),
                     GameText::RelicText::packedSchoolbagName(),
                     GameText::RelicText::packedSchoolbagDescription(GameBalance::Relics::packedSchoolbagDraw()),
                     GameText::RelicText::packedSchoolbagImage(),
                     RelicEffectType::StartCombatDraw,
                     GameBalance::Relics::packedSchoolbagDraw());
}

inline RelicData createLuckyCoupon()
{
    return RelicData(RelicIds::luckyCoupon(),
                     GameText::RelicText::luckyCouponName(),
                     GameText::RelicText::luckyCouponDescription(GameBalance::Relics::luckyCouponDiscount()),
                     GameText::RelicText::luckyCouponImage(),
                     RelicEffectType::ShopDiscount,
                     GameBalance::Relics::luckyCouponDiscount());
}

inline RelicData createQingtianObservationGuide()
{
    return RelicData(RelicIds::qingtianObservationGuide(),
                     GameText::RelicText::qingtianObservationGuideName(),
                     GameText::RelicText::qingtianObservationGuideDescription(
                         GameBalance::Relics::qingtianObservationGuideStrength()),
                     GameText::RelicText::qingtianObservationGuideImage(),
                     RelicEffectType::StartCombatStrength,
                     GameBalance::Relics::qingtianObservationGuideStrength());
}

inline RelicData createPkuWatermelon()
{
    return RelicData(RelicIds::pkuWatermelon(),
                     GameText::RelicText::pkuWatermelonName(),
                     GameText::RelicText::pkuWatermelonDescription(GameBalance::Relics::pkuWatermelonTurnBlock()),
                     GameText::RelicText::pkuWatermelonImage(),
                     RelicEffectType::TurnStartBlock,
                     GameBalance::Relics::pkuWatermelonTurnBlock());
}

inline RelicData starterRelic()
{
    return createYanyuanMealCard();
}

inline QList<RelicData> allRelics()
{
    QList<RelicData> relics;
    relics << createYanyuanMealCard()
           << createBoyaTowerBadge()
           << createLibrarySeat()
           << createPackedSchoolbag()
           << createLuckyCoupon()
           << createQingtianObservationGuide()
           << createPkuWatermelon();
    return relics;
}

inline RelicData findById(const QString &relicId)
{
    const QList<RelicData> relics = allRelics();
    for (const RelicData &relic : relics) {
        if (relic.id == relicId) {
            return relic;
        }
    }
    return RelicData();
}
}

#endif // RELIC_H
