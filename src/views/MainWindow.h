#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MapView;
class StatusBarWidget;
class QStackedWidget;

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void toggleMapVisibility();

private:
    void setupUi();
    void updateStatusBar();

    MapView *mapView;
    StatusBarWidget *statusBarWidget;
    QStackedWidget *contentStack;
    QWidget *placeholderWidget;
    bool mapVisible = false;
};

#endif // MAINWINDOW_H
