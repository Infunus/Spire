#ifndef HAND_H
#define HAND_H

#include "Card.h"

#include <QList>
#include <QRandomGenerator>
#include <QStringList>
#include <QtGlobal>

class Hand
{
public:
    void setDeck(const QList<Card> &cards)
    {
        m_drawPile = cards;
        m_discardPile.clear();
        m_cards.clear();
        m_used.clear();
        shuffle(m_drawPile);
    }

    int drawCards(int count, int maxHandSize = 10)
    {
        int drawn = 0;
        for (int i = 0; i < count && m_cards.size() < maxHandSize; ++i) {
            if (m_drawPile.isEmpty()) {
                reshuffleDiscard();
            }
            if (m_drawPile.isEmpty()) {
                return drawn;
            }

            m_cards.append(m_drawPile.takeFirst());
            m_used.append(false);
            ++drawn;
        }
        return drawn;
    }

    Card takeCard(int index)
    {
        if (index < 0 || index >= m_cards.size()) {
            return Card();
        }

        Card card = m_cards.takeAt(index);
        m_used.removeAt(index);
        m_discardPile.append(card);
        return card;
    }

    int discardHand()
    {
        const int count = m_cards.size();
        for (const Card &card : m_cards) {
            m_discardPile.append(card);
        }
        m_cards.clear();
        m_used.clear();
        return count;
    }

    int size() const { return m_cards.size(); }
    int drawPileSize() const { return m_drawPile.size(); }
    int discardPileSize() const { return m_discardPile.size(); }
    bool isEmpty() const { return m_cards.isEmpty(); }

    Card cardAt(int index) const
    {
        if (index < 0 || index >= m_cards.size()) {
            return Card();
        }
        return m_cards.at(index);
    }

    QStringList buttonTexts() const
    {
        QStringList texts;
        for (const Card &card : m_cards) {
            texts << card.buttonText();
        }
        return texts;
    }

private:
    void reshuffleDiscard()
    {
        m_drawPile = m_discardPile;
        m_discardPile.clear();
        shuffle(m_drawPile);
    }

    void shuffle(QList<Card> &cards)
    {
        for (int i = cards.size() - 1; i > 0; --i) {
            const int j = QRandomGenerator::global()->bounded(i + 1);
            qSwap(cards[i], cards[j]);
        }
    }

    QList<Card> m_drawPile;
    QList<Card> m_discardPile;
    QList<Card> m_cards;
    QList<bool> m_used;
};

#endif // HAND_H
