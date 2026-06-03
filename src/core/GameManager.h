#ifndef GAMEMANAGER_H
#define GAMEMANAGER_H

#include <QObject>
#include "RunData.h"

class GameManager : public QObject
{
    Q_OBJECT

public:
    static GameManager* instance();
    const RunData& runData() const;
    void startNewRun();

signals:
    void runDataChanged();

private:
    explicit GameManager(QObject *parent = nullptr);
    RunData currentRun;
    static GameManager *s_instance;
};

#endif // GAMEMANAGER_H
