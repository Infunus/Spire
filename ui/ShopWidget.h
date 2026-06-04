#ifndef SHOPWIDGET_H
#define SHOPWIDGET_H

#include "../core/CardLibrary.h"
#include "../core/GameBalance.h"
#include "../core/GameState.h"
#include "../core/GameText.h"
#include "../core/Potion.h"
#include "../core/Relic.h"

#include <QFrame>
#include <QDialog>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QList>
#include <QPushButton>
#include <QStringList>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

class ShopWidget : public QWidget
{
public:
    explicit ShopWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_coinLabel(new QLabel(this)),
          m_messageLabel(new QLabel(this)),
          m_relicLayout(new QVBoxLayout),
          m_cardLayout(new QVBoxLayout),
          m_potionLayout(new QVBoxLayout),
          m_serviceLayout(new QVBoxLayout),
          m_cardRemovalBought(false)
    {
        setObjectName("ShopWidget");
        setStyleSheet(
            "#ShopWidget { background: #17130f; }"
            "QLabel { color: #f6ead0; }"
            "QFrame#ShopPanel { background: rgba(28, 24, 18, 220); border: 1px solid rgba(255, 226, 168, 105); border-radius: 8px; }"
            "QFrame#ShopItem { background: rgba(255, 235, 190, 24); border: 1px solid rgba(255, 226, 168, 85); border-radius: 8px; }"
            "QPushButton { color: #2a180e; background: #d7a84f; border: 1px solid #ffe3a3; border-radius: 6px; padding: 7px 12px; font-size: 14px; font-weight: 900; }"
            "QPushButton:hover { background: #efc36b; }"
            "QPushButton:disabled { background: #4c463d; border-color: #756b5d; color: #a99b86; }");

        QLabel *titleLabel = new QLabel(GameText::ShopText::pageTitle(), this);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 34px; font-weight: 900;");

        m_coinLabel->setAlignment(Qt::AlignCenter);
        m_coinLabel->setStyleSheet("font-size: 18px; font-weight: 900; color: #ffe1a3;");

        m_messageLabel->setAlignment(Qt::AlignCenter);
        m_messageLabel->setWordWrap(true);
        m_messageLabel->setStyleSheet("font-size: 15px; color: rgba(246, 234, 208, 210);");

        QHBoxLayout *columnsLayout = new QHBoxLayout;
        columnsLayout->setSpacing(14);
        columnsLayout->addWidget(createSection(GameText::ShopText::relicSectionTitle(), m_relicLayout));
        columnsLayout->addWidget(createSection(GameText::ShopText::cardSectionTitle(), m_cardLayout));
        columnsLayout->addWidget(createSection(GameText::ShopText::potionSectionTitle(), m_potionLayout));
        columnsLayout->addWidget(createSection(GameText::ShopText::serviceSectionTitle(), m_serviceLayout));

        QPushButton *leaveButton = new QPushButton(GameText::ShopText::leaveButton(), this);
        leaveButton->setFixedSize(150, 42);
        connect(leaveButton, &QPushButton::clicked, this, [this]() {
            if (m_finishHandler) {
                m_finishHandler();
            }
        });

        QVBoxLayout *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(58, 42, 58, 42);
        rootLayout->setSpacing(16);
        rootLayout->addWidget(titleLabel);
        rootLayout->addWidget(m_coinLabel);
        rootLayout->addWidget(m_messageLabel);
        rootLayout->addLayout(columnsLayout, 1);
        rootLayout->addWidget(leaveButton, 0, Qt::AlignCenter);

        openShop();
    }

    void setFinishHandler(const std::function<void()> &handler)
    {
        m_finishHandler = handler;
    }

    void openShop()
    {
        m_boughtRelicIds.clear();
        m_boughtCardNames.clear();
        m_boughtPotionIds.clear();
        m_cardRemovalBought = false;
        rebuildItems();
        setMessage(GameText::ShopText::intro(GameBalance::Shop::discountPercent()));
    }

private:
    QFrame *createSection(const QString &title, QVBoxLayout *itemsLayout)
    {
        QFrame *panel = new QFrame(this);
        panel->setObjectName("ShopPanel");
        panel->setMinimumWidth(210);

        QLabel *titleLabel = new QLabel(title, panel);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 21px; font-weight: 900; color: #fff3d4;");

        QVBoxLayout *layout = new QVBoxLayout(panel);
        layout->setContentsMargins(14, 14, 14, 14);
        layout->setSpacing(10);
        layout->addWidget(titleLabel);
        layout->addLayout(itemsLayout);
        layout->addStretch();
        return panel;
    }

    void rebuildItems()
    {
        clearLayout(m_relicLayout);
        clearLayout(m_cardLayout);
        clearLayout(m_potionLayout);
        clearLayout(m_serviceLayout);

        bool hasRelicForSale = false;
        const QList<RelicData> relics = RelicLibrary::allRelics();
        for (const RelicData &relic : relics) {
            if (GameState::instance().hasRelic(relic.id)) {
                continue;
            }
            hasRelicForSale = true;
            m_relicLayout->addWidget(createRelicItem(relic));
        }
        if (!hasRelicForSale) {
            m_relicLayout->addWidget(createInfoLabel(GameText::ShopText::noRelicText()));
        }

        const QList<Card> cards = CardLibrary::shopCards();
        for (const Card &card : cards) {
            m_cardLayout->addWidget(createCardItem(card));
        }

        const QList<PotionData> potions = PotionLibrary::allPotions();
        for (const PotionData &potion : potions) {
            m_potionLayout->addWidget(createPotionItem(potion));
        }

        m_serviceLayout->addWidget(createCardRemovalItem());

        refreshCoinLabel();
    }

    QWidget *createRelicItem(const RelicData &relic)
    {
        return createShopItem(relic.name,
                              relic.description,
                              priceForRelic(relic),
                              m_boughtRelicIds.contains(relic.id),
                              [this, relic]() {
                                  const int price = priceForRelic(relic);
                                  if (!GameState::instance().spendCoins(price)) {
                                      setMessage(GameText::ShopText::notEnoughCoinsFormat().arg(relic.name));
                                      return;
                                  }
                                  GameState::instance().addRelic(relic);
                                  m_boughtRelicIds << relic.id;
                                  setMessage(GameText::ShopText::buySuccessFormat().arg(relic.name));
                                  rebuildItems();
                              });
    }

    QWidget *createCardItem(const Card &card)
    {
        return createShopItem(card.displayName(),
                              card.description(),
                              priceForCard(card),
                              m_boughtCardNames.contains(card.id()),
                              [this, card]() {
                                  const int price = priceForCard(card);
                                  if (!GameState::instance().spendCoins(price)) {
                                      setMessage(GameText::ShopText::notEnoughCoinsFormat().arg(card.displayName()));
                                      return;
                                  }
                                  GameState::instance().addCard(card);
                                  m_boughtCardNames << card.id();
                                  setMessage(GameText::ShopText::buySuccessFormat().arg(card.displayName()));
                                  rebuildItems();
                              });
    }

    QWidget *createPotionItem(const PotionData &potion)
    {
        const bool noSlot = GameState::instance().potions().size() >= GameState::instance().maxPotions();
        return createShopItem(potion.name,
                              potion.description,
                              priceForPotion(potion),
                              m_boughtPotionIds.contains(potion.id) || noSlot,
                              [this, potion]() {
                                  if (GameState::instance().potions().size() >= GameState::instance().maxPotions()) {
                                      setMessage(GameText::ShopText::noPotionSlotText());
                                      return;
                                  }

                                  const int price = priceForPotion(potion);
                                  if (!GameState::instance().spendCoins(price)) {
                                      setMessage(GameText::ShopText::notEnoughCoinsFormat().arg(potion.name));
                                      return;
                                  }
                                  GameState::instance().addPotion(potion);
                                  m_boughtPotionIds << potion.id;
                                  setMessage(GameText::ShopText::buySuccessFormat().arg(potion.name));
                                  rebuildItems();
                              });
    }

    QWidget *createCardRemovalItem()
    {
        return createShopItem(GameText::ShopText::cardRemovalName(),
                              GameText::ShopText::cardRemovalDescription(),
                              priceForCardRemoval(),
                              m_cardRemovalBought,
                              [this]() {
                                  openCardRemovalDialog(priceForCardRemoval());
                              });
    }

    QWidget *createShopItem(const QString &name,
                            const QString &description,
                            int price,
                            bool bought,
                            const std::function<void()> &buyAction)
    {
        QFrame *row = new QFrame(this);
        row->setObjectName("ShopItem");

        QLabel *label = new QLabel(QStringLiteral("%1\n%2").arg(name, description), row);
        label->setWordWrap(true);
        label->setStyleSheet("font-size: 14px; font-weight: 700;");

        QPushButton *buyButton = new QPushButton(row);
        buyButton->setText(bought
                               ? GameText::ShopText::boughtButton()
                               : GameText::ShopText::buyButtonFormat().arg(price));
        buyButton->setEnabled(!bought);
        connect(buyButton, &QPushButton::clicked, this, [buyAction]() {
            buyAction();
        });

        QHBoxLayout *layout = new QHBoxLayout(row);
        layout->setContentsMargins(12, 10, 12, 10);
        layout->setSpacing(10);
        layout->addWidget(label, 1);
        layout->addWidget(buyButton);
        return row;
    }

    QLabel *createInfoLabel(const QString &text)
    {
        QLabel *label = new QLabel(text, this);
        label->setWordWrap(true);
        label->setAlignment(Qt::AlignCenter);
        label->setStyleSheet("color: rgba(246, 234, 208, 165); font-size: 14px;");
        return label;
    }

    int discountedPrice(int basePrice) const
    {
        if (GameState::instance().hasRelic(RelicIds::luckyCoupon())) {
            return basePrice * (100 - GameBalance::Shop::discountPercent()) / 100;
        }
        return basePrice;
    }

    int priceForRelic(const RelicData &relic) const
    {
        Q_UNUSED(relic);
        return discountedPrice(GameBalance::Shop::relicPrice());
    }

    int priceForCard(const Card &card) const
    {
        return discountedPrice(card.cost() == 0
                                   ? GameBalance::Shop::zeroCostCardPrice()
                                   : GameBalance::Shop::cardBasePrice()
                                         + card.cost() * GameBalance::Shop::cardCostPriceStep());
    }

    int priceForPotion(const PotionData &potion) const
    {
        Q_UNUSED(potion);
        return discountedPrice(GameBalance::Shop::potionPrice());
    }

    int priceForCardRemoval() const
    {
        return discountedPrice(GameBalance::Shop::cardRemovalPrice());
    }

    void openCardRemovalDialog(int price)
    {
        const QList<Card> cards = GameState::instance().ownedCards();
        if (cards.isEmpty()) {
            setMessage(GameText::ShopText::emptyDeckText());
            return;
        }

        QDialog *dialog = new QDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowTitle(GameText::ShopText::cardRemovalDialogTitle());
        dialog->resize(640, 430);
        dialog->setStyleSheet(
            "QDialog { background: #17130f; }"
            "QPushButton { color: #2a180e; background: #d7a84f; border: 1px solid #ffe3a3; border-radius: 8px; padding: 8px; font-size: 13px; font-weight: 900; }"
            "QPushButton:hover { background: #efc36b; }");

        QGridLayout *grid = new QGridLayout(dialog);
        grid->setContentsMargins(14, 14, 14, 14);
        grid->setSpacing(10);
        for (int i = 0; i < cards.size(); ++i) {
            const Card card = cards.at(i);
            QPushButton *button = new QPushButton(QStringLiteral("%1\n%2").arg(card.displayName(), card.description()), dialog);
            button->setMinimumSize(150, 96);
            button->setToolTip(cardLine(card));
            connect(button, &QPushButton::clicked, this, [this, dialog, i, price, card]() {
                if (!GameState::instance().spendCoins(price)) {
                    setMessage(GameText::ShopText::cardRemovalNotEnoughCoins());
                    return;
                }
                if (GameState::instance().removeCardAt(i)) {
                    m_cardRemovalBought = true;
                    setMessage(GameText::ShopText::cardRemovalSuccess() + QStringLiteral(" ") + card.displayName());
                    dialog->close();
                    rebuildItems();
                }
            });
            grid->addWidget(button, i / 3, i % 3);
        }
        dialog->show();
    }

    QString cardLine(const Card &card) const
    {
        return QStringLiteral("%1\n费用 %2\n%3")
            .arg(card.displayName())
            .arg(card.cost())
            .arg(card.description());
    }

    void refreshCoinLabel()
    {
        m_coinLabel->setText(GameText::ShopText::coinsFormat().arg(GameState::instance().coins()));
    }

    void setMessage(const QString &message)
    {
        m_messageLabel->setText(message + QStringLiteral("\n") + GameText::ShopText::shopFinishedTip());
        refreshCoinLabel();
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

    QLabel *m_coinLabel;
    QLabel *m_messageLabel;
    QVBoxLayout *m_relicLayout;
    QVBoxLayout *m_cardLayout;
    QVBoxLayout *m_potionLayout;
    QVBoxLayout *m_serviceLayout;
    QStringList m_boughtRelicIds;
    QStringList m_boughtCardNames;
    QStringList m_boughtPotionIds;
    bool m_cardRemovalBought;
    std::function<void()> m_finishHandler;
};

#endif // SHOPWIDGET_H
