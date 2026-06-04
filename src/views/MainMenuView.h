#ifndef MAINMENUVIEW_H
#define MAINMENUVIEW_H

#include <QWidget>

class QPushButton;
class QResizeEvent;

class MainMenuView : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuView(QWidget *parent = nullptr);

signals:
    void startRunRequested();
    void debugBattleRequested();
    void debugBossRequested();
    void debugShopRequested();
    void debugEventRequested();
    void debugRewardRequested();
    void debugRestRequested();
    void quitRequested();

protected:
    void resizeEvent(QResizeEvent *event) override;
};

#endif // MAINMENUVIEW_H
