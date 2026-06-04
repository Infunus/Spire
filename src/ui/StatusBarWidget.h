#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QColor>
#include <QPixmap>
#include <QWidget>
#include <QString>
#include <vector>

#include "data/RelicData.h"
#include "data/PotionData.h"

class QLabel;
class QPushButton;
class QGridLayout;
class QPaintEvent;
class QMouseEvent;

class RelicIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RelicIconWidget(QWidget *parent = nullptr);
    void setIcon(const QPixmap &pixmap);
    void setColor(const QColor &color);
    void setRelic(const RelicData &relic);
    void setPopupText(const QString &title, const QString &description);

signals:
    void clicked(const QString &title, const QString &description);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;

private:
    QPixmap icon;
    QColor color;
    QString popupTitle;
    QString popupDescription;
};

class StatusBarWidget : public QWidget
{
    Q_OBJECT

public:
    explicit StatusBarWidget(QWidget *parent = nullptr);

    void setHealth(int current, int maximum);
    void setFloor(int floor);
    void setCharacterName(const QString &name);
    void setGold(int gold);
    void setPotions(int count);
    void setPotionData(const std::vector<PotionData> &potions);
    void setRelicCount(int count);
    void setRelicIcons(const std::vector<QPixmap> &icons);
    void setRelics(const std::vector<RelicData> &relics);

signals:
    void mapButtonClicked();
    void deckButtonClicked();
    void settingsButtonClicked();
    void potionClicked(int index);
    void relicClicked(const QString &title, const QString &description);

private:
    QLabel* createStatusLabel(const QString &title, const QString &value, const QColor &color);
    QPushButton* createToolButton(const QString &text, const QColor &color);
    void rebuildRelics(const std::vector<QPixmap> &icons);
    void rebuildRelics(const std::vector<RelicData> &relics);
    void rebuildPotions(const std::vector<PotionData> &potions);

    QLabel *healthLabel;
    QLabel *floorLabel;
    QLabel *characterLabel;
    QLabel *goldLabel;
    QLabel *potionsLabel;
    QWidget *potionPanel;
    QGridLayout *potionLayout;
    QPushButton *mapButton;
    QPushButton *deckButton;
    QPushButton *settingsButton;
    QWidget *relicPanel;
    QGridLayout *relicLayout;
};

#endif // STATUSBARWIDGET_H
