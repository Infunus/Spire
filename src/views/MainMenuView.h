#ifndef MAINMENUVIEW_H
#define MAINMENUVIEW_H

#include <QWidget>

class QPushButton;

class MainMenuView : public QWidget
{
    Q_OBJECT

public:
    explicit MainMenuView(QWidget *parent = nullptr);

signals:
    void startRunRequested();
    void quitRequested();
};

#endif // MAINMENUVIEW_H
