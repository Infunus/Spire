#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QString>

class BattleWidget;
class EventWidget;
class QStackedWidget;
class QWidget;

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
    QWidget *createBattlePage(bool bossBattle = false);
    QWidget *createEventPreviewPage();
    QWidget *createDebugPlaceholderPage(const QString &title, const QString &body);
    void showBattlePage();
    void showBossBattlePage();
    void showEventPreviewPage();
    void showDebugPlaceholderPage(const QString &title, const QString &body);
    QString assetPath(const QString &relativePath) const;

    Ui::MainWindow *ui;
    QStackedWidget *m_pages;
    QWidget *m_battlePage;
    QWidget *m_eventPage;
    QWidget *m_debugPage;
    BattleWidget *m_battleWidget;
    EventWidget *m_eventWidget;
};
#endif // MAINWINDOW_H
