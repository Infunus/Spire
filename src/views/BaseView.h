#ifndef BASEVIEW_H
#define BASEVIEW_H

#include <QWidget>

class BaseView : public QWidget
{
    Q_OBJECT

public:
    explicit BaseView(QWidget *parent = nullptr);
    ~BaseView() override;
};

#endif // BASEVIEW_H
