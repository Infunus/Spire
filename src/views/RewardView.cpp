#include "RewardView.h"

#include "core/GameManager.h"
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
        "RewardView { background: #181f1a; }"
        "QLabel { color: #f5ead2; font-size: 18px; }"
        "QPushButton { color: #f8edd4; background: #4f7c57; border: 1px solid rgba(255,255,255,90);"
        "              border-radius: 4px; padding: 10px 16px; font-size: 15px; font-weight: 700; }"
        "QPushButton:hover { background: #609568; }");

    titleLabel->setText(QString::fromUtf8(u8"战斗奖励"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: 900;");
    rewardLabel->setAlignment(Qt::AlignCenter);
    rewardLabel->setWordWrap(true);

    QPushButton *skipButton = new QPushButton(QString::fromUtf8(u8"不选择卡牌"), this);
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
                             ? QString::fromUtf8(u8"获得 20 金币和一瓶火焰药水。选择一张卡牌加入牌组，或直接跳过。")
                             : QString::fromUtf8(u8"获得 20 金币。药水栏已满，无法获得新药水。选择一张卡牌加入牌组，或直接跳过。"));
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
        button->setMinimumSize(180, 120);
        button->setToolTip(card.description);
        connect(button, &QPushButton::clicked, this, [this, card]() {
            GameManager::instance()->addCard(card.id);
            emit rewardsFinished();
        });
        cardLayout->addWidget(button);
    }
}
