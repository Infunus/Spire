#include "ShopView.h"

#include "core/GameManager.h"
#include "data/CardData.h"
#include "data/GameText.h"

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ShopView::ShopView(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , messageLabel(new QLabel(this))
    , itemsLayout(new QVBoxLayout)
    , exitButton(new QPushButton(GameText::Shop::leaveButton(), this))
{
    setStyleSheet(
        "ShopView { background: #10131d; }"
        "QLabel { color: #f5ead2; font-size: 17px; }"
        "QFrame#shopItem {"
        "  background: rgba(8, 11, 18, 185);"
        "  border: 1px solid rgba(185, 215, 255, 125);"
        "  border-radius: 10px;"
        "}"
        "QPushButton {"
        "  background: rgba(255, 226, 168, 34);"
        "  border: 1px solid rgba(255, 226, 168, 135);"
        "  border-radius: 8px;"
        "  color: #fff1cc;"
        "  font-size: 16px;"
        "  font-weight: 800;"
        "  padding: 10px 14px;"
        "}"
        "QPushButton:hover { background: rgba(235, 184, 88, 96); border-color: rgba(255, 235, 177, 220); }"
        "QPushButton:disabled { background: #3b201c; border-color: #7f5f47; color: #7d6655; }");

    titleLabel->setText(GameText::Shop::title());
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 34px; font-weight: 900; color: #fff6dc;");
    messageLabel->setAlignment(Qt::AlignCenter);
    messageLabel->setWordWrap(true);

    connect(exitButton, &QPushButton::clicked, this, &ShopView::shopFinished);

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(80, 60, 80, 60);
    rootLayout->setSpacing(18);
    rootLayout->addStretch();
    rootLayout->addWidget(titleLabel);
    rootLayout->addWidget(messageLabel);
    rootLayout->addLayout(itemsLayout);
    rootLayout->addWidget(exitButton, 0, Qt::AlignHCenter);
    rootLayout->addStretch();
}

void ShopView::openShop()
{
    saleRelics.clear();
    salePotions = PotionCatalog::rewardPotions();
    boughtRelicIds.clear();
    boughtPotionIds.clear();
    cardRemovalBought = false;

    const QStringList ownedRelics = GameManager::instance()->runData().relicIds;
    for (const RelicData &relic : RelicCatalog::testRelics()) {
        if (!ownedRelics.contains(relic.id)) {
            saleRelics.push_back(relic);
        }
    }

    messageLabel->setText(saleRelics.empty()
                              ? GameText::Shop::emptyText()
                              : GameText::Shop::chooseText());
    rebuildItems();
}

void ShopView::rebuildItems()
{
    while (QLayoutItem *item = itemsLayout->takeAt(0)) {
        if (QWidget *widget = item->widget()) {
            widget->deleteLater();
        }
        delete item;
    }

    for (const RelicData &relic : saleRelics) {
        QFrame *row = new QFrame(this);
        row->setObjectName("shopItem");
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(18, 14, 18, 14);
        rowLayout->setSpacing(14);

        QLabel *label = new QLabel(QString::fromUtf8(u8"%1\n%2").arg(relic.name, relic.effect), row);
        label->setWordWrap(true);
        label->setStyleSheet("font-size: 16px; color: rgba(255,247,224,220);");

        QPushButton *buyButton = new QPushButton(row);
        const bool bought = boughtRelicIds.contains(relic.id);
        buyButton->setText(bought
                               ? GameText::Shop::boughtButton()
                               : GameText::Shop::buyButtonFormat().arg(priceForRelic(relic)));
        buyButton->setEnabled(!bought);

        connect(buyButton, &QPushButton::clicked, this, [this, relic]() {
            const int price = priceForRelic(relic);
            if (!GameManager::instance()->spendGold(price)) {
                messageLabel->setText(GameText::Shop::notEnoughGoldFormat().arg(relic.name));
                return;
            }

            GameManager::instance()->addRelic(relic.id);
            boughtRelicIds.append(relic.id);
            messageLabel->setText(GameText::Shop::boughtFormat().arg(relic.name));
            rebuildItems();
        });

        rowLayout->addWidget(label, 1);
        rowLayout->addWidget(buyButton);
        itemsLayout->addWidget(row);
    }

    for (const PotionData &potion : salePotions) {
        QFrame *row = new QFrame(this);
        row->setObjectName("shopItem");
        QHBoxLayout *rowLayout = new QHBoxLayout(row);
        rowLayout->setContentsMargins(18, 14, 18, 14);
        QLabel *label = new QLabel(QString::fromUtf8(u8"药水：%1\n%2").arg(potion.name, potion.effect), row);
        label->setWordWrap(true);
        QPushButton *buyButton = new QPushButton(GameText::Shop::buyButtonFormat().arg(priceForPotion(potion)), row);
        const bool bought = boughtPotionIds.contains(potion.id);
        buyButton->setEnabled(!bought);
        if (bought) {
            buyButton->setText(GameText::Shop::boughtButton());
        }
        connect(buyButton, &QPushButton::clicked, this, [this, potion]() {
            const int price = priceForPotion(potion);
            if (!GameManager::instance()->spendGold(price)) {
                messageLabel->setText(GameText::Shop::notEnoughGoldFormat().arg(potion.name));
                return;
            }
            if (!GameManager::instance()->addPotion(potion.id)) {
                GameManager::instance()->addGold(price);
                messageLabel->setText(QString::fromUtf8(u8"药水栏已满，无法购买。"));
                return;
            }
            boughtPotionIds.append(potion.id);
            messageLabel->setText(GameText::Shop::boughtFormat().arg(potion.name));
            rebuildItems();
        });
        rowLayout->addWidget(label, 1);
        rowLayout->addWidget(buyButton);
        itemsLayout->addWidget(row);
    }

    QFrame *removeRow = new QFrame(this);
    removeRow->setObjectName("shopItem");
    QHBoxLayout *removeLayout = new QHBoxLayout(removeRow);
    removeLayout->setContentsMargins(18, 14, 18, 14);
    QLabel *removeLabel = new QLabel(QString::fromUtf8(u8"服务：移除一张卡牌\n从当前牌组中永久删除一张卡牌。"), removeRow);
    removeLabel->setWordWrap(true);
    QPushButton *removeButton = new QPushButton(GameText::Shop::buyButtonFormat().arg(priceForCardRemoval()), removeRow);
    removeButton->setEnabled(!cardRemovalBought);
    if (cardRemovalBought) {
        removeButton->setText(GameText::Shop::boughtButton());
    }
    connect(removeButton, &QPushButton::clicked, this, [this]() {
        const int price = priceForCardRemoval();
        if (!GameManager::instance()->spendGold(price)) {
            messageLabel->setText(QString::fromUtf8(u8"金币不足，无法移除卡牌。"));
            return;
        }
        QDialog *dialog = new QDialog(this);
        dialog->setWindowTitle(QString::fromUtf8(u8"选择要移除的卡牌"));
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        QGridLayout *grid = new QGridLayout(dialog);
        const QStringList deck = GameManager::instance()->runData().deckIds;
        for (int i = 0; i < deck.size(); ++i) {
            const CardData card = CardCatalog::byId(deck[i]);
            QPushButton *button = new QPushButton(QStringLiteral("%1\n%2").arg(card.name, card.description), dialog);
            button->setMinimumSize(150, 90);
            connect(button, &QPushButton::clicked, this, [this, dialog, i]() {
                if (GameManager::instance()->removeCardAt(i)) {
                    cardRemovalBought = true;
                    messageLabel->setText(QString::fromUtf8(u8"卡牌已移除。"));
                    dialog->close();
                    rebuildItems();
                }
            });
            grid->addWidget(button, i / 3, i % 3);
        }
        dialog->resize(620, 420);
        dialog->show();
    });
    removeLayout->addWidget(removeLabel, 1);
    removeLayout->addWidget(removeButton);
    itemsLayout->addWidget(removeRow);
}

int ShopView::priceForRelic(const RelicData &relic) const
{
    Q_UNUSED(relic)

    int price = 80;
    if (GameManager::instance()->runData().relicIds.contains(QStringLiteral("lucky_charm"))) {
        price = 72;
    }
    return price;
}

int ShopView::priceForPotion(const PotionData &potion) const
{
    Q_UNUSED(potion)
    return 45;
}

int ShopView::priceForCardRemoval() const
{
    return 75;
}
