#include "MapNode.h"

#include <QPropertyAnimation>
#include <QPainter>
#include <QGraphicsSceneHoverEvent>
#include <QGraphicsSceneMouseEvent>

MapNode::MapNode(int id, const QPointF &pos, const QPixmap &icon, qreal radius, QGraphicsItem *parent)
    : QGraphicsObject(parent)
    , nodeID(id)
    , radius(radius)
    , baseColor(Qt::red)
    , icon(icon)
{
    setFlags(ItemIsSelectable | ItemIsFocusable);
    setAcceptHoverEvents(true);
    setPos(pos);
    setToolTip(QStringLiteral("节点 %1").arg(id));
}

void MapNode::setIcon(const QPixmap &pixmap)
{
    icon = pixmap;
    update();
}

void MapNode::setDefeatedIcon(const QPixmap &pixmap)
{
    defeatedIcon = pixmap;
    update();
}

void MapNode::setAvailable(bool value)
{
    available = value;
    setOpacity(available || defeated ? 1.0 : 0.9);
    setAcceptedMouseButtons(available && !defeated ? Qt::LeftButton : Qt::NoButton);
    setAcceptHoverEvents(available && !defeated);
    update();
}

void MapNode::setDefeated(bool value)
{
    defeated = value;
    selected = value;
    setOpacity(1.0);
    setAcceptedMouseButtons(Qt::NoButton);
    setAcceptHoverEvents(false);
    update();
}

void MapNode::setAnimationScale(qreal scale)
{
    currentScale = scale;
    setScale(scale);
}

void MapNode::setHoverOpacity(qreal opacity)
{
    currentHoverOpacity = opacity;
    update();
}

void MapNode::setRingProgress(qreal progress)
{
    currentRingProgress = qBound<qreal>(0.0, progress, 1.0);
    update();
}

QRectF MapNode::boundingRect() const
{
    qreal extra = radius * 0.9;
    return QRectF(-radius - extra, -radius - extra, (radius + extra) * 2, (radius + extra) * 2);
}

void MapNode::paint(QPainter *painter, const QStyleOptionGraphicsItem *option, QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)

    painter->setRenderHint(QPainter::Antialiasing);
    QRectF shapeRect(-radius, -radius, radius * 2, radius * 2);
    QRectF iconRect(-radius * 0.68, -radius * 0.68, radius * 1.36, radius * 1.36);

    if (!icon.isNull()) {
        QPixmap pix = icon.scaled(iconRect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPointF offset((iconRect.width() - pix.width()) / 2, (iconRect.height() - pix.height()) / 2);
        painter->drawPixmap(iconRect.topLeft() + offset, pix);

        if (currentHoverOpacity > 0.0) {
            painter->setBrush(QColor(0, 0, 0, static_cast<int>(currentHoverOpacity * 140)));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(shapeRect);
        }
    } else {
        painter->setPen(Qt::NoPen);
        painter->setBrush(baseColor);
        painter->drawEllipse(shapeRect);
    }

    if (defeated && !defeatedIcon.isNull()) {
        QRectF defeatedRect(-radius * 1.12, -radius * 1.12, radius * 2.24, radius * 2.24);
        QPixmap pix = defeatedIcon.scaled(defeatedRect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPointF offset((defeatedRect.width() - pix.width()) / 2, (defeatedRect.height() - pix.height()) / 2);
        painter->drawPixmap(defeatedRect.topLeft() + offset, pix);
    }

    if ((available && !defeated) || currentRingProgress > 0.0) {
        qreal ringRadius = radius + radius * 0.11 + currentRingProgress * radius * 0.19;
        QColor ringColor(120, 200, 255, static_cast<int>(180 * qMin<qreal>(1.0, currentRingProgress + 0.2)));
        QPen pen(ringColor, qMax<qreal>(1.0, radius / 18.0));
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0, 0), ringRadius, ringRadius);
    }
}

void MapNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    if (!available || defeated) {
        event->ignore();
        return;
    }

    QPropertyAnimation *scaleAnimation = new QPropertyAnimation(this, "animationScale");
    scaleAnimation->setDuration(180);
    scaleAnimation->setEndValue(1.2);
    scaleAnimation->setEasingCurve(QEasingCurve::OutQuad);
    scaleAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "hoverOpacity");
    opacityAnimation->setDuration(180);
    opacityAnimation->setEndValue(0.55);
    opacityAnimation->setEasingCurve(QEasingCurve::OutQuad);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MapNode::hoverLeaveEvent(QGraphicsSceneHoverEvent *event)
{
    if (!available || defeated) {
        event->ignore();
        return;
    }

    QPropertyAnimation *scaleAnimation = new QPropertyAnimation(this, "animationScale");
    scaleAnimation->setDuration(180);
    scaleAnimation->setEndValue(1.0);
    scaleAnimation->setEasingCurve(QEasingCurve::OutQuad);
    scaleAnimation->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *opacityAnimation = new QPropertyAnimation(this, "hoverOpacity");
    opacityAnimation->setDuration(180);
    opacityAnimation->setEndValue(0.0);
    opacityAnimation->setEasingCurve(QEasingCurve::OutQuad);
    opacityAnimation->start(QAbstractAnimation::DeleteWhenStopped);
}

void MapNode::mousePressEvent(QGraphicsSceneMouseEvent *event)
{
    if (!available || defeated) {
        event->ignore();
        return;
    }

    if (!selected) {
        selected = true;
        QPropertyAnimation *ringAnimation = new QPropertyAnimation(this, "ringProgress");
        ringAnimation->setDuration(360);
        ringAnimation->setStartValue(currentRingProgress);
        ringAnimation->setEndValue(1.0);
        ringAnimation->setEasingCurve(QEasingCurve::OutCubic);
        ringAnimation->start(QAbstractAnimation::DeleteWhenStopped);
    }
    emit clicked(nodeID);
}
