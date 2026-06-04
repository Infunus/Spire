#ifndef REWARDVIEW_H
#define REWARDVIEW_H

#include <QWidget>
#include <vector>

#include "data/CardData.h"

class QLabel;
class QHBoxLayout;

class RewardView : public QWidget
{
    Q_OBJECT

public:
    explicit RewardView(QWidget *parent = nullptr);
    void openRewards();

signals:
    void rewardsFinished();

private:
    void rebuildCardChoices();

    QLabel *titleLabel;
    QLabel *rewardLabel;
    QHBoxLayout *cardLayout;
    std::vector<CardData> choices;
};

#endif // REWARDVIEW_H
