#ifndef REWARDWIDGET_H
#define REWARDWIDGET_H

#include "../core/CardLibrary.h"
#include "../core/GameBalance.h"
#include "../core/GameRandom.h"
#include "../core/GameState.h"
#include "../core/GameText.h"
#include "../core/Potion.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QList>
#include <QPushButton>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

class RewardWidget : public QWidget
{
public:
    explicit RewardWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_titleLabel(new QLabel(this)),
          m_rewardLabel(new QLabel(this)),
          m_cardLayout(new QHBoxLayout)
    {
        setObjectName("RewardWidget");
        setStyleSheet(
            "#RewardWidget { background: #10131d; }"
            "QLabel { color: #f5ead2; font-size: 18px; }"
            "QPushButton {"
            "  background: rgba(255, 226, 168, 34);"
            "  border: 1px solid rgba(255, 226, 168, 135);"
            "  border-radius: 8px;"
            "  color: #fff1cc;"
            "  font-size: 16px;"
            "  font-weight: 800;"
            "  padding: 10px 14px;"
            "}"
            "QPushButton:hover { background: rgba(235, 184, 88, 96); border-color: rgba(255, 235, 177, 220); }");

        m_titleLabel->setText(GameText::RewardText::pageTitle());
        m_titleLabel->setAlignment(Qt::AlignCenter);
        m_titleLabel->setStyleSheet("font-size: 34px; font-weight: 900; color: #fff6dc;");

        m_rewardLabel->setAlignment(Qt::AlignCenter);
        m_rewardLabel->setWordWrap(true);

        QPushButton *skipButton = new QPushButton(GameText::RewardText::skipButton(), this);
        connect(skipButton, &QPushButton::clicked, this, [this]() {
            finishRewards();
        });

        QVBoxLayout *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(70, 60, 70, 60);
        rootLayout->setSpacing(18);
        rootLayout->addStretch();
        rootLayout->addWidget(m_titleLabel);
        rootLayout->addWidget(m_rewardLabel);
        rootLayout->addLayout(m_cardLayout);
        rootLayout->addWidget(skipButton, 0, Qt::AlignHCenter);
        rootLayout->addStretch();
    }

    void setFinishHandler(const std::function<void()> &handler)
    {
        m_finishHandler = handler;
    }

    void openRewards()
    {
        GameState::instance().addCoins(GameBalance::Rewards::rewardScreenCoins());
        const bool potionAdded = GameState::instance().addPotion(PotionLibrary::createCoffeeShot());
        m_rewardLabel->setText(potionAdded
                                   ? GameText::RewardText::rewardWithPotion(GameBalance::Rewards::rewardScreenCoins(),
                                                                            PotionLibrary::createCoffeeShot().name)
                                   : GameText::RewardText::rewardPotionFull(GameBalance::Rewards::rewardScreenCoins()));

        m_choices.clear();
        QList<Card> pool = CardLibrary::shopCards();
        while (!pool.isEmpty() && m_choices.size() < GameBalance::Rewards::cardChoices()) {
            const int index = GameRandom::instance().bounded(pool.size());
            m_choices.append(pool.takeAt(index));
        }
        rebuildCardChoices();
    }

private:
    void rebuildCardChoices()
    {
        clearLayout(m_cardLayout);
        for (const Card &card : m_choices) {
            QPushButton *button = new QPushButton(card.buttonText(), this);
            button->setMinimumSize(180, 150);
            button->setToolTip(cardLine(card));
            button->setStyleSheet(
                "QPushButton {"
                "  background: qlineargradient(x1:0, y1:0, x2:0, y2:1,"
                "                              stop:0 #fff7df, stop:0.62 #f0d18b, stop:1 #b87938);"
                "  border: 3px solid #6f4928;"
                "  border-radius: 14px;"
                "  color: #2b2118;"
                "  font-size: 14px;"
                "  font-weight: 800;"
                "  padding: 10px;"
                "}"
                "QPushButton:hover { border-color: #ffd27a; }");
            connect(button, &QPushButton::clicked, this, [this, card]() {
                GameState::instance().addCard(card);
                finishRewards();
            });
            m_cardLayout->addWidget(button);
        }
    }

    void finishRewards()
    {
        m_rewardLabel->setText(GameText::RewardText::finishTip());
        if (m_finishHandler) {
            m_finishHandler();
        }
    }

    QString cardLine(const Card &card) const
    {
        return QStringLiteral("%1\n费用 %2\n%3")
            .arg(card.displayName())
            .arg(card.cost())
            .arg(card.description());
    }

    void clearLayout(QLayout *layout)
    {
        while (QLayoutItem *item = layout->takeAt(0)) {
            if (QWidget *widget = item->widget()) {
                widget->deleteLater();
            }
            delete item;
        }
    }

    QLabel *m_titleLabel;
    QLabel *m_rewardLabel;
    QHBoxLayout *m_cardLayout;
    QList<Card> m_choices;
    std::function<void()> m_finishHandler;
};

#endif // REWARDWIDGET_H
