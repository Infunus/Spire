#ifndef SHOPVIEW_H
#define SHOPVIEW_H

#include <QWidget>
#include <QStringList>
#include <vector>

#include "data/RelicData.h"
#include "data/PotionData.h"

class QLabel;
class QPushButton;
class QVBoxLayout;

// ShopView is a basic merchant test scene.
// Example: openShop() lists test relics; buying tiny_chest spends gold and adds its relic id.
// The room is completed only when shopFinished() emits from the exit button.
class ShopView : public QWidget
{
    Q_OBJECT

public:
    explicit ShopView(QWidget *parent = nullptr);
    void openShop();

signals:
    void shopFinished();

private:
    void rebuildItems();
    int priceForRelic(const RelicData &relic) const;
    int priceForPotion(const PotionData &potion) const;
    int priceForCardRemoval() const;

    QLabel *titleLabel;
    QLabel *messageLabel;
    QVBoxLayout *itemsLayout;
    QPushButton *exitButton;
    std::vector<RelicData> saleRelics;
    std::vector<PotionData> salePotions;
    QStringList boughtRelicIds;
    QStringList boughtPotionIds;
    bool cardRemovalBought = false;
};

#endif // SHOPVIEW_H
