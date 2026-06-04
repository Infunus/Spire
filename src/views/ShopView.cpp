#include "ShopView.h"

#include "core/GameManager.h"

#include <QFrame>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

ShopView::ShopView(QWidget *parent)
    : QWidget(parent)
    , titleLabel(new QLabel(this))
    , messageLabel(new QLabel(this))
    , itemsLayout(new QVBoxLayout)
    , exitButton(new QPushButton(QString::fromUtf8(u8"离开商店"), this))
{
    setStyleSheet(
        "ShopView { background: #1d1b16; }"
        "QLabel { color: #f5ead2; font-size: 17px; }"
        "QFrame#shopItem { background: rgba(255,255,255,24); border: 1px solid rgba(255,255,255,70); border-radius: 6px; }"
        "QPushButton { color: #f8edd4; background: #87662e; border: 1px solid rgba(255,255,255,90);"
        "              border-radius: 4px; padding: 8px 14px; font-size: 15px; font-weight: 700; }"
        "QPushButton:hover { background: #a17a39; }"
        "QPushButton:disabled { background: #555; color: #aaa; border-color: rgba(255,255,255,35); }");

    titleLabel->setText(QString::fromUtf8(u8"商人"));
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 32px; font-weight: 800;");
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
    boughtRelicIds.clear();

    const QStringList ownedRelics = GameManager::instance()->runData().relicIds;
    for (const RelicData &relic : RelicCatalog::testRelics()) {
        if (!ownedRelics.contains(relic.id)) {
            saleRelics.push_back(relic);
        }
    }

    messageLabel->setText(saleRelics.empty()
                              ? QString::fromUtf8(u8"商店暂时没有新的遗物。")
                              : QString::fromUtf8(u8"选择一个遗物购买。拥有幸运护符时，价格降低 10%。"));
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
        rowLayout->setContentsMargins(14, 12, 14, 12);
        rowLayout->setSpacing(12);

        QLabel *label = new QLabel(QString::fromUtf8(u8"%1\n%2").arg(relic.name, relic.effect), row);
        label->setWordWrap(true);

        QPushButton *buyButton = new QPushButton(row);
        const bool bought = boughtRelicIds.contains(relic.id);
        buyButton->setText(bought
                               ? QString::fromUtf8(u8"已购买")
                               : QString::fromUtf8(u8"购买 %1 金币").arg(priceForRelic(relic)));
        buyButton->setEnabled(!bought);

        connect(buyButton, &QPushButton::clicked, this, [this, relic]() {
            const int price = priceForRelic(relic);
            if (!GameManager::instance()->spendGold(price)) {
                messageLabel->setText(QString::fromUtf8(u8"金币不足，买不起 %1。").arg(relic.name));
                return;
            }

            GameManager::instance()->addRelic(relic.id);
            boughtRelicIds.append(relic.id);
            messageLabel->setText(QString::fromUtf8(u8"购买了 %1。").arg(relic.name));
            rebuildItems();
        });

        rowLayout->addWidget(label, 1);
        rowLayout->addWidget(buyButton);
        itemsLayout->addWidget(row);
    }
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
