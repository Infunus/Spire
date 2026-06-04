#include "RewardView.h"

#include "core/GameManager.h"
#include "data/GameText.h"
#include "data/PotionData.h"

#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

RewardView::RewardView(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , rewardLabel(new QLabel(this))
    , cardLayout(new QHBoxLayout)
{
    setStyleSheet(
        "RewardView { background: #10131d; }"
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

    titleLabel->setText(GameText::Reward::title());
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 34px; font-weight: 900; color: #fff6dc;");
    rewardLabel->setAlignment(Qt::AlignCenter);
    rewardLabel->setWordWrap(true);

    QPushButton *skipButton = new QPushButton(GameText::Reward::skipButton(), this);
    connect(skipButton, &QPushButton::clicked, this, &RewardView::rewardsFinished);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(70, 60, 70, 60);
    layout->setSpacing(18);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(rewardLabel);
    layout->addLayout(cardLayout);
    layout->addWidget(skipButton, 0, Qt::AlignHCenter);
    layout->addStretch();
}

void RewardView::openRewards()
{
    GameManager::instance()->addGold(20);
    const bool potionAdded = GameManager::instance()->addPotion(PotionCatalog::firePotion().id);
    rewardLabel->setText(potionAdded
                             ? GameText::Reward::rewardWithPotion()
                             : GameText::Reward::rewardPotionFull());
    choices = CardCatalog::rewardCards();
    rebuildCardChoices();
}

void RewardView::rebuildCardChoices()
{
    while (QLayoutItem *item = cardLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for (const CardData &card : choices) {
        QPushButton *button = new QPushButton(QStringLiteral("%1\n%2").arg(card.name, card.description), this);
        button->setMinimumSize(180, 150);
        button->setToolTip(card.description);
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
            GameManager::instance()->addCard(card.id);
            emit rewardsFinished();
        });
        cardLayout->addWidget(button);
    }
}
