#ifndef CARDLIBRARY_H
#define CARDLIBRARY_H

#include "Card.h"
#include "GameBalance.h"
#include "GameText.h"

#include <QList>
#include <QString>

namespace CardIds
{
inline QString strike() { return QStringLiteral("strike"); }
inline QString defend() { return QStringLiteral("defend"); }
inline QString bash() { return QStringLiteral("bash"); }
inline QString inflame() { return QStringLiteral("inflame"); }
inline QString heavyBlade() { return QStringLiteral("heavy_blade"); }
inline QString pommel() { return QStringLiteral("pommel"); }
inline QString shrug() { return QStringLiteral("shrug"); }
inline QString anger() { return QStringLiteral("anger"); }
inline QString flex() { return QStringLiteral("flex"); }
inline QString cleave() { return QStringLiteral("cleave"); }
}

namespace CardLibrary
{
inline Card makeCard(const QString &id,
                     const QString &name,
                     const QString &description,
                     const GameBalance::CardStats &stats,
                     const QString &imagePath,
                     bool upgraded)
{
    return Card(name,
                stats.cost,
                stats.damage,
                stats.block,
                stats.heal,
                stats.selfDamage,
                description,
                stats.weak,
                stats.vulnerable,
                stats.draw,
                stats.strengthGain,
                stats.strengthMultiplier,
                imagePath,
                stats.exhaust,
                id,
                upgraded);
}

inline Card strike(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::strikeUpgraded()
                                                  : GameBalance::Cards::strike();
    return makeCard(CardIds::strike(),
                    GameText::CardText::strikeName(),
                    GameText::CardText::strikeDescription(stats.damage),
                    stats,
                    GameText::CardText::strikeImage(),
                    upgraded);
}

inline Card defend(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::defendUpgraded()
                                                  : GameBalance::Cards::defend();
    return makeCard(CardIds::defend(),
                    GameText::CardText::defendName(),
                    GameText::CardText::defendDescription(stats.block),
                    stats,
                    GameText::CardText::defendImage(),
                    upgraded);
}

inline Card bash(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::bashUpgraded()
                                                  : GameBalance::Cards::bash();
    return makeCard(CardIds::bash(),
                    GameText::CardText::bashName(),
                    GameText::CardText::bashDescription(stats.damage, stats.vulnerable),
                    stats,
                    GameText::CardText::bashImage(),
                    upgraded);
}

inline Card inflame(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::inflameUpgraded()
                                                  : GameBalance::Cards::inflame();
    return makeCard(CardIds::inflame(),
                    GameText::CardText::inflameName(),
                    GameText::CardText::inflameDescription(stats.strengthGain),
                    stats,
                    GameText::CardText::inflameImage(),
                    upgraded);
}

inline Card heavyBlade(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::heavyBladeUpgraded()
                                                  : GameBalance::Cards::heavyBlade();
    return makeCard(CardIds::heavyBlade(),
                    GameText::CardText::heavyBladeName(),
                    GameText::CardText::heavyBladeDescription(stats.damage, stats.strengthMultiplier),
                    stats,
                    GameText::CardText::heavyBladeImage(),
                    upgraded);
}

inline Card pommel(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::pommelUpgraded()
                                                  : GameBalance::Cards::pommel();
    return makeCard(CardIds::pommel(),
                    GameText::CardText::pommelName(),
                    GameText::CardText::pommelDescription(stats.damage, stats.draw),
                    stats,
                    GameText::CardText::pommelImage(),
                    upgraded);
}

inline Card shrug(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::shrugUpgraded()
                                                  : GameBalance::Cards::shrug();
    return makeCard(CardIds::shrug(),
                    GameText::CardText::shrugName(),
                    GameText::CardText::shrugDescription(stats.block, stats.draw),
                    stats,
                    GameText::CardText::shrugImage(),
                    upgraded);
}

inline Card anger(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::angerUpgraded()
                                                  : GameBalance::Cards::anger();
    return makeCard(CardIds::anger(),
                    GameText::CardText::angerName(),
                    GameText::CardText::angerDescription(stats.damage),
                    stats,
                    GameText::CardText::angerImage(),
                    upgraded);
}

inline Card flex(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::flexUpgraded()
                                                  : GameBalance::Cards::flex();
    return makeCard(CardIds::flex(),
                    GameText::CardText::flexName(),
                    GameText::CardText::flexDescription(stats.strengthGain, stats.draw),
                    stats,
                    GameText::CardText::flexImage(),
                    upgraded);
}

inline Card cleave(bool upgraded = false)
{
    const GameBalance::CardStats stats = upgraded ? GameBalance::Cards::cleaveUpgraded()
                                                  : GameBalance::Cards::cleave();
    return makeCard(CardIds::cleave(),
                    GameText::CardText::cleaveName(),
                    GameText::CardText::cleaveDescription(stats.damage),
                    stats,
                    GameText::CardText::cleaveImage(),
                    upgraded);
}

inline Card byId(const QString &id, bool upgraded = false)
{
    if (id == CardIds::strike()) {
        return strike(upgraded);
    }
    if (id == CardIds::defend()) {
        return defend(upgraded);
    }
    if (id == CardIds::bash()) {
        return bash(upgraded);
    }
    if (id == CardIds::inflame()) {
        return inflame(upgraded);
    }
    if (id == CardIds::heavyBlade()) {
        return heavyBlade(upgraded);
    }
    if (id == CardIds::pommel()) {
        return pommel(upgraded);
    }
    if (id == CardIds::shrug()) {
        return shrug(upgraded);
    }
    if (id == CardIds::anger()) {
        return anger(upgraded);
    }
    if (id == CardIds::flex()) {
        return flex(upgraded);
    }
    if (id == CardIds::cleave()) {
        return cleave(upgraded);
    }
    return Card();
}

inline bool canUpgrade(const Card &card)
{
    return !card.id().isEmpty() && !card.upgraded();
}

inline Card upgradedCard(const Card &card)
{
    if (!canUpgrade(card)) {
        return card;
    }
    return byId(card.id(), true);
}

inline QList<Card> starterDeck()
{
    QList<Card> deck;
    deck << strike() << strike() << strike() << strike()
         << defend() << defend() << defend() << defend()
         << bash()
         << inflame()
         << heavyBlade()
         << pommel()
         << shrug()
         << anger()
         << flex();
    return deck;
}

inline QList<Card> shopCards()
{
    QList<Card> cards;
    cards << inflame()
          << heavyBlade()
          << pommel()
          << shrug()
          << flex()
          << cleave();
    return cards;
}
}

#endif // CARDLIBRARY_H
