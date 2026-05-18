#ifndef BATTLEWIDGET_H
#define BATTLEWIDGET_H

#include <QList>
#include <QPushButton>
#include <QStringList>
#include <QWidget>

class QLabel;
class QHBoxLayout;
class QResizeEvent;

class ArcHandWidget : public QWidget
{
    Q_OBJECT

public:
    explicit ArcHandWidget(QWidget *parent = nullptr);
    void setCards(const QStringList &cards);

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    void updateCardLayout();

    QList<QPushButton *> m_cardButtons;
};

class PortraitWidget : public QWidget
{
    Q_OBJECT

public:
    explicit PortraitWidget(const QString &name, QWidget *parent = nullptr);
    void setPortraitImage(const QString &imagePath);
    void setStats(int hp, int armor);

private:
    QLabel *m_portraitLabel;
    QLabel *m_nameLabel;
    QLabel *m_hpLabel;
    QLabel *m_armorLabel;
};

class EnemyCardWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EnemyCardWidget(const QString &name, int hp, QWidget *parent = nullptr);
    void setCardImage(const QString &imagePath);
    void setHp(int hp);
    void setIntent(const QString &intent);

private:
    QLabel *m_imageLabel;
    QLabel *m_nameLabel;
    QLabel *m_intentLabel;
    QLabel *m_hpLabel;
};

class EnergyWidget : public QWidget
{
    Q_OBJECT

public:
    explicit EnergyWidget(QWidget *parent = nullptr);
    void setEnergy(int current, int maximum);

private:
    QHBoxLayout *m_layout;
    QList<QLabel *> m_energyPips;
};

class BattleWidget : public QWidget
{
    Q_OBJECT

public:
    explicit BattleWidget(QWidget *parent = nullptr);
    void setBossBattle(bool isBossBattle);

private:
    QWidget *createArenaPanel();
    QWidget *createEnemyField(QWidget *parent);
    QWidget *createPlayerField(QWidget *parent);
    void setupInitialDemoText();

    bool m_isBossBattle;
    QLabel *m_arenaTitleLabel;
    PortraitWidget *m_bossPortrait;
    PortraitWidget *m_playerPortrait;
    QWidget *m_enemyField;
    QWidget *m_playerField;
    EnergyWidget *m_energyWidget;
    ArcHandWidget *m_handWidget;
    QPushButton *m_endTurnButton;
};

#endif // BATTLEWIDGET_H
