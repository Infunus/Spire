#ifndef MAPNODE_H
#define MAPNODE_H

#include <QGraphicsObject>
#include <QPixmap>
#include <QColor>

class QPropertyAnimation;
class QPainter;
class QStyleOptionGraphicsItem;
class QGraphicsSceneHoverEvent;
class QGraphicsSceneMouseEvent;

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
    void setDefeatedIcon(const QPixmap &pixmap);
    void setAvailable(bool available);
    void setDefeated(bool defeated);
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
    int nodeID;
    qreal radius;
    QColor baseColor;
    QPixmap icon;
    QPixmap defeatedIcon;
    bool available = false;
    bool defeated = false;
    bool selected = false;
    qreal currentScale = 1.0;
    qreal currentHoverOpacity = 0.0;
    qreal currentRingProgress = 0.0;
};

#endif // MAPNODE_H
