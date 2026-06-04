#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>

class MapView;
class MainMenuView;
class EventView;
class BattleView;
class ShopView;
class RewardView;
class StatusBarWidget;
class QStackedWidget;
class QWidget;
class QDialog;

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
    void startNewRunFromMenu();
    void showMainMenu();
    void showDeckDialog();
    void showSettingsDialog();
    void showInfoDialog(const QString &title, const QString &text);
    void handlePotionClicked(int index);
    void startSceneForEvent(int eventKindValue);
    void showRewardScene();
    void finishRewardScene();
    void finishActiveScene();

    MapView *mapView;
    MainMenuView *mainMenuView;
    EventView *eventView;
    BattleView *battleView;
    ShopView *shopView;
    RewardView *rewardView;
    StatusBarWidget *statusBarWidget;
    QStackedWidget *contentStack;
    QWidget *placeholderWidget;
    QDialog *deckDialog = nullptr;
    bool deckDialogLarge = false;
    QWidget *activeSceneWidget = nullptr;
    bool runEnded = false;
    bool mapVisible = false;
};

#endif // MAINWINDOW_H
