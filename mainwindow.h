#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "core/AudioManager.h"
#include "core/GameState.h"

#include <QMainWindow>
#include <QString>
#include <QStringList>

class BattleWidget;
class EventWidget;
class MapWidget;
class RewardWidget;
class ShopWidget;
class QStackedWidget;
class QWidget;
struct RandomEventChoice;
struct RandomEventData;

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    QWidget *createMenuPage();
    QWidget *createMapPage();
    QWidget *createBattlePage(bool bossBattle = false, bool fromMap = false);
    QWidget *createEventPreviewPage(bool fromMap = false);
    QWidget *createEventPreviewPage(const RandomEventData &eventData, bool fromMap = false);
    QWidget *createShopPage(bool fromMap = false);
    QWidget *createRewardPage(bool fromMap = false);
    QWidget *createEventCardRewardPage(const RandomEventChoice &choice,
                                       bool fromMap = false,
                                       int rewardIndex = 0,
                                       int rewardCount = 1);
    QWidget *createMapNodePlaceholderPage(const QString &title, const QString &body);
    QWidget *createDebugPlaceholderPage(const QString &title, const QString &body);
    void startNewRunFromMenu();
    void showMapPage(bool resetMap = false);
    void showBattlePage();
    void showBossBattlePage();
    void showEventPreviewPage();
    void showEventPreviewPage(const RandomEventData &eventData);
    void showRewardPage(bool fromMap = false);
    void showEventCardRewardPage(const RandomEventChoice &choice, bool fromMap = false, int rewardIndex = 0);
    void showDebugPlaceholderPage(const QString &title, const QString &body);
    void openMapNode(MapNodeType nodeType);
    void finishMapNode(bool completed);
    void playBattleMusic();
    void playWorldMusic();
    void pauseMusic();
    void stopMusic();
    void stopAllMusic();
    QStringList worldMusicFiles() const;
    QString assetPath(const QString &relativePath) const;

    Ui::MainWindow *ui;
    QStackedWidget *m_pages;
    QWidget *m_mapPage;
    QWidget *m_battlePage;
    QWidget *m_eventPage;
    QWidget *m_shopPage;
    QWidget *m_rewardPage;
    QWidget *m_debugPage;
    QWidget *m_mapNodePage;
    MapWidget *m_mapWidget;
    BattleWidget *m_battleWidget;
    EventWidget *m_eventWidget;
    ShopWidget *m_shopWidget;
    RewardWidget *m_rewardWidget;
    MapNodeType m_activeMapNodeType;
    AudioManager m_battleAudioManager;
    AudioManager m_worldAudioManager;
};
#endif // MAINWINDOW_H
