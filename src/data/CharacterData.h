#ifndef CHARACTERDATA_H
#define CHARACTERDATA_H

#include <QString>
#include <vector>

// CharacterData.h
// Put static playable-character definitions here.
// Example: CharacterCatalog::ironclad() defines maxHp = 80 and starterDeckId.
// Store id, display name, max hp, starting gold, starter deck id, and portrait here.
// Example: "maxHp = 80" belongs here, but "currentHp = 42" belongs in RunData.
// Do not put current hp/gold during a run here; those belong in RunData.

struct CharacterData {
    QString id;
    QString name;
    int maxHp = 0;
    int startingGold = 0;
    QString starterDeckId;
    QString portraitPath;
};

class CharacterCatalog
{
public:
    static CharacterData ironclad();
    static std::vector<CharacterData> playableCharacters();
};

#endif // CHARACTERDATA_H
