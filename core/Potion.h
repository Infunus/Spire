#ifndef POTION_H
#define POTION_H

#include "GameBalance.h"
#include "GameText.h"

#include <QList>
#include <QString>

enum class PotionEffectType
{
    None,
    DamageEnemy,
    GainBlock,
    Heal
};

struct PotionData
{
    QString id;
    QString name;
    QString description;
    QString imagePath;
    PotionEffectType effectType;
    int amount;

    PotionData()
        : effectType(PotionEffectType::None),
          amount(0)
    {
    }

    PotionData(const QString &potionId,
               const QString &potionName,
               const QString &potionDescription,
               const QString &potionImagePath,
               PotionEffectType potionEffectType,
               int potionAmount)
        : id(potionId),
          name(potionName),
          description(potionDescription),
          imagePath(potionImagePath),
          effectType(potionEffectType),
          amount(potionAmount)
    {
    }
};

namespace PotionIds
{
inline QString coffeeShot() { return QStringLiteral("coffee_shot"); }
inline QString antiBreakSpray() { return QStringLiteral("anti_break_spray"); }
inline QString clinicSyrup() { return QStringLiteral("clinic_syrup"); }
}

namespace PotionLibrary
{
inline PotionData createCoffeeShot()
{
    return PotionData(PotionIds::coffeeShot(),
                      GameText::PotionText::coffeeShotName(),
                      GameText::PotionText::coffeeShotDescription(GameBalance::Potions::coffeeShotDamage()),
                      GameText::PotionText::coffeeShotImage(),
                      PotionEffectType::DamageEnemy,
                      GameBalance::Potions::coffeeShotDamage());
}

inline PotionData createAntiBreakSpray()
{
    return PotionData(PotionIds::antiBreakSpray(),
                      GameText::PotionText::antiBreakSprayName(),
                      GameText::PotionText::antiBreakSprayDescription(GameBalance::Potions::antiBreakBlock()),
                      GameText::PotionText::antiBreakSprayImage(),
                      PotionEffectType::GainBlock,
                      GameBalance::Potions::antiBreakBlock());
}

inline PotionData createClinicSyrup()
{
    return PotionData(PotionIds::clinicSyrup(),
                      GameText::PotionText::clinicSyrupName(),
                      GameText::PotionText::clinicSyrupDescription(GameBalance::Potions::clinicSyrupHeal()),
                      GameText::PotionText::clinicSyrupImage(),
                      PotionEffectType::Heal,
                      GameBalance::Potions::clinicSyrupHeal());
}

inline QList<PotionData> allPotions()
{
    QList<PotionData> potions;
    potions << createCoffeeShot()
            << createAntiBreakSpray()
            << createClinicSyrup();
    return potions;
}

inline PotionData findById(const QString &potionId)
{
    const QList<PotionData> potions = allPotions();
    for (const PotionData &potion : potions) {
        if (potion.id == potionId) {
            return potion;
        }
    }
    return PotionData();
}
}

#endif // POTION_H
