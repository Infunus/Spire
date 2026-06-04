#ifndef EVENTVIEW_H
#define EVENTVIEW_H

#include <QWidget>

#include "data/EventData.h"

class QLabel;
class QPushButton;
class QResizeEvent;

// EventView is used for non-combat, non-shop rooms.
// Example: RestSite displays text and emits eventFinished() only after the exit button is pressed.
class EventView : public QWidget
{
    Q_OBJECT

public:
    explicit EventView(QWidget *parent = nullptr);
    void setEvent(const EventData &eventData);

signals:
    void eventFinished();

protected:
    void resizeEvent(QResizeEvent *event) override;

private:
    QLabel *iconLabel;
    QLabel *titleLabel;
    QLabel *descriptionLabel;
    QPushButton *exitButton;
};

#endif // EVENTVIEW_H
