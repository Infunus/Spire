#ifndef STATUSBARWIDGET_H
#define STATUSBARWIDGET_H

#include <QColor>
#include <QPixmap>
#include <QWidget>
#include <QString>
#include <vector>

class QLabel;
class QPushButton;
class QGridLayout;
class QPaintEvent;

class RelicIconWidget : public QWidget
{
    Q_OBJECT

public:
    explicit RelicIconWidget(QWidget *parent = nullptr);
    void setIcon(const QPixmap &pixmap);
    void setColor(const QColor &color);

protected:
    void paintEvent(QPaintEvent *event) override;

private:
    QPixmap icon;
    QColor color;
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
    void setRelicCount(int count);
    void setRelicIcons(const std::vector<QPixmap> &icons);

signals:
    void mapButtonClicked();
    void deckButtonClicked();
    void settingsButtonClicked();

private:
    QLabel* createStatusLabel(const QString &title, const QString &value, const QColor &color);
    QPushButton* createToolButton(const QString &text, const QColor &color);
    void rebuildRelics(const std::vector<QPixmap> &icons);

    QLabel *healthLabel;
    QLabel *floorLabel;
    QLabel *characterLabel;
    QLabel *goldLabel;
    QLabel *potionsLabel;
    QPushButton *mapButton;
    QPushButton *deckButton;
    QPushButton *settingsButton;
    QWidget *relicPanel;
    QGridLayout *relicLayout;
};

#endif // STATUSBARWIDGET_H
