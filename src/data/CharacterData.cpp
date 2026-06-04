#include "CharacterData.h"

CharacterData CharacterCatalog::ironclad()
{
    return {
        QStringLiteral("ironclad"),
        QString::fromUtf8("铁甲战士"),
        80,
        99,
        QStringLiteral("ironclad_starter"),
        QStringLiteral(":/pictures/resources/pictures/barbute.png")
    };
}

std::vector<CharacterData> CharacterCatalog::playableCharacters()
{
    return {ironclad()};
}
