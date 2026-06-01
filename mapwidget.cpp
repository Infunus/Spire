#include "mapwidget.h"

#include <QGraphicsView>
#include <QGraphicsPixmapItem>
#include <QPainter>
#include <QDebug>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <vector>

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
    setToolTip(QString("节点 %1").arg(id));
}

void MapNode::setIcon(const QPixmap &pixmap)//设置图片
{
    icon = pixmap;
    update();
}

void MapNode::setAnimationScale(qreal scale)//改变节点大小
{
    currentScale = scale;
    setScale(scale);
}

void MapNode::setHoverOpacity(qreal opacity)//记录透明度
{
    currentHoverOpacity = opacity;
    update();
}

void MapNode::setRingProgress(qreal progress)//控制选中后的扩散光圈
{
    currentRingProgress = qBound<qreal>(0.0, progress, 1.0);
    update();
}

QRectF MapNode::boundingRect() const//告诉QT绘制范围
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

    if (!icon.isNull()) {
        QPixmap pix = icon.scaled(shapeRect.size().toSize(), Qt::KeepAspectRatio, Qt::SmoothTransformation);
        QPointF offset((shapeRect.width() - pix.width()) / 2, (shapeRect.height() - pix.height()) / 2);
        painter->drawPixmap(shapeRect.topLeft() + offset, pix);

        if (currentHoverOpacity > 0.0) {
            painter->setBrush(QColor(0, 0, 0, static_cast<int>(currentHoverOpacity * 140)));
            painter->setPen(Qt::NoPen);
            painter->drawEllipse(shapeRect);
        }
    } else {
        painter->setPen(Qt::NoPen);
        painter->setBrush(baseColor);
        painter->drawEllipse(shapeRect);
    }//没有图标时按照背景色绘制圆

    if (!icon.isNull()) {
        painter->setPen(Qt::white);
        QFont font = painter->font();
        font.setBold(true);
        font.setPointSizeF(qMax<qreal>(8.0, radius / 2.0));
        painter->setFont(font);
        painter->drawText(shapeRect, Qt::AlignCenter, QString::number(nodeID));
    }

    if (selected || currentRingProgress > 0.0) {
        qreal ringRadius = radius + 8 + currentRingProgress * 14;
        QColor ringColor(120, 200, 255, static_cast<int>(180 * qMin<qreal>(1.0, currentRingProgress + 0.2)));
        QPen pen(ringColor, 4);
        pen.setCosmetic(true);
        painter->setPen(pen);
        painter->setBrush(Qt::NoBrush);
        painter->drawEllipse(QPointF(0, 0), ringRadius, ringRadius);
    }
}

void MapNode::hoverEnterEvent(QGraphicsSceneHoverEvent *event)
{
    Q_UNUSED(event)

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
    Q_UNUSED(event)

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
    Q_UNUSED(event)
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

MapWidget::MapWidget(QWidget *parent)
    : QMainWindow(parent)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
{
    setWindowTitle(tr("地图"));
    resize(2560, 1440);

    scene->setSceneRect(0, 0, 2560, 4320);
    scene->setBackgroundBrush(QColor(40, 40, 60));//背景颜色

    view->setScene(scene);
    view->setRenderHint(QPainter::Antialiasing);
    view->setRenderHint(QPainter::SmoothPixmapTransform);
    view->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    view->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    view->setAlignment(Qt::AlignTop | Qt::AlignHCenter);
    view->setResizeAnchor(QGraphicsView::AnchorViewCenter);
    view->setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    view->setDragMode(QGraphicsView::NoDrag);
    view->setInteractive(true);
    setCentralWidget(view);

    setupScene();
    fitWidthToView();
}

void MapWidget::setupScene()
{
    QPixmap background(":/pictures/resources/pictures/background.png");
    if (background.isNull()) {
        background.load("resources/pictures/background.png");
    }

    if (!background.isNull()) {
        scene->setSceneRect(background.rect());
        QGraphicsPixmapItem *backgroundItem = scene->addPixmap(background);
        backgroundItem->setZValue(-100);
    }

    const qreal width = scene->sceneRect().width();

    const std::vector<QPointF> positions = {
        {width / 2, 120},
        {width / 4, 320},
        {width * 3 / 4, 320},
        {width / 2, 520},
        {width / 4, 720},
        {width * 3 / 4, 720},
        {width / 2, 920},
        {width / 4, 1120},
        {width * 3 / 4, 1120},
    };

    for (int i = 0; i < static_cast<int>(positions.size()); ++i) {
        addNode(i + 1, positions[i]);
    }

    for (int i = 0; i < static_cast<int>(positions.size()) - 1; ++i) {
        scene->addLine(QLineF(positions[i], positions[i + 1]), QPen(QColor(160, 160, 220), 3, Qt::SolidLine, Qt::RoundCap));
    }
}

void MapWidget::fitWidthToView()
{
    const qreal sceneWidth = scene->sceneRect().width();
    if (sceneWidth <= 0 || view->viewport()->width() <= 0) {
        return;
    }

    view->resetTransform();
    view->scale(view->viewport()->width() / sceneWidth, view->viewport()->width() / sceneWidth);
}

MapNode* MapWidget::addNode(int id, const QPointF &pos, const QPixmap &icon)
{
    MapNode *node = new MapNode(id, pos, icon);
    connect(node, &MapNode::clicked, this, &MapWidget::onNodeClicked);
    scene->addItem(node);
    return node;
}

void MapWidget::resizeEvent(QResizeEvent *event)
{
    QMainWindow::resizeEvent(event);
    fitWidthToView();
}

void MapWidget::onNodeClicked(int nodeId)
{
    qDebug() << "地图节点点击:" << nodeId;
}
