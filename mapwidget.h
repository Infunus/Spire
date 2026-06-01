#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include <QMainWindow>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <QGraphicsObject>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>
#include <QPixmap>

class QPropertyAnimation;
class QPainter;
class QStyleOptionGraphicsItem;
class QResizeEvent;

class MapNode : public QGraphicsObject
{
    Q_OBJECT
    Q_PROPERTY(qreal animationScale READ animationScale WRITE setAnimationScale)
    Q_PROPERTY(qreal hoverOpacity READ hoverOpacity WRITE setHoverOpacity)
    Q_PROPERTY(qreal ringProgress READ ringProgress WRITE setRingProgress)

public:
    explicit MapNode(int id, const QPointF &pos, const QPixmap &icon = QPixmap(), qreal radius = 24, QGraphicsItem *parent = nullptr);
    int id() const { return nodeID; }
    qreal animationScale() const { return currentScale; }
    qreal hoverOpacity() const { return currentHoverOpacity; }
    qreal ringProgress() const { return currentRingProgress; }
    void setIcon(const QPixmap &pixmap);
    void setAnimationScale(qreal scale);
    void setHoverOpacity(qreal opacity);
    void setRingProgress(qreal progress);

signals:
    void clicked(int nodeId);

protected:
    void hoverEnterEvent(QGraphicsSceneHoverEvent *event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent *event) override;
    void mousePressEvent(QGraphicsSceneMouseEvent *event) override;
    QRectF boundingRect() const override;
    void paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget) override;

private:
    int nodeID;//节点编号
    qreal radius;//节点半径
    QColor baseColor;//节点颜色
    QPixmap icon;//节点图片
    bool selected = false;//节点是否已经选中
    qreal currentScale = 1.0;//当前缩放倍数
    qreal currentHoverOpacity = 0.0;//悬停时阴影透明度
    qreal currentRingProgress = 0.0;//选中动画进度
};

class MapWidget : public QMainWindow
{
    Q_OBJECT

public:
    explicit MapWidget(QWidget *parent = nullptr);

protected:
    void resizeEvent(QResizeEvent *event) override;

private slots:
    void onNodeClicked(int nodeId);

private:
    void setupScene();
    void fitWidthToView();
    MapNode* addNode(int id, const QPointF &pos, const QPixmap &icon = QPixmap());

    QGraphicsView* view;
    QGraphicsScene* scene;
};

#endif // MAPWIDGET_H
