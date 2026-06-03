#include "MapView.h"
#include "MapNode.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QHBoxLayout>
#include <QDebug>
#include <QTimer>

namespace {
constexpr int BaseLayerCount = 13;
constexpr int FinalBossLayer = BaseLayerCount;
constexpr int ChoicesPerBaseLayer = 3;
constexpr qreal RoomNodeRadius = 28.8;
constexpr qreal FinalBossNodeRadius = 54;

QString randomRoomIcon(bool allowRest)
{
    const int roll = QRandomGenerator::global()->bounded(100);
    if (roll < 10) {
        return QStringLiteral("locked-chest.png");
    }
    if (roll < 70) {
        return QStringLiteral("barbute.png");
    }
    if (roll < 80) {
        return QStringLiteral("brute.png");
    }
    if (roll < 90) {
        return QStringLiteral("camping-tent.png");
    }
    if (allowRest) {
        return QStringLiteral("campfire.png");
    }
    return QStringLiteral("barbute.png");
}
}

MapView::MapView(QWidget *parent)
    : QWidget(parent)
    , view(new QGraphicsView(this))
    , scene(new QGraphicsScene(this))
{
    setMinimumSize(900, 640);

    scene->setSceneRect(0, 0, 2560, 4320);

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

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    layout->addWidget(view);

    setupScene();
    fitWidthToView();
}

QSize MapView::minimumSizeHint() const
{
    return QSize(900, 640);
}

void MapView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    fitWidthToView();
}

void MapView::showEvent(QShowEvent *event)
{
    QWidget::showEvent(event);
    QTimer::singleShot(0, this, [this]() {
        fitWidthToView();
    });
}

void MapView::setupScene()
{
    scene->clear();
    nodes.clear();
    layers.clear();
    hintLines.clear();
    routeLines.clear();
    previousSelectedNode = nullptr;
    currentLayerIndex = 0;

    const QPixmap background = loadPicture(QStringLiteral("background.png"));
    if (!background.isNull()) {
        backgroundItem = scene->addPixmap(background);
        backgroundItem->setZValue(-100);
        scene->setSceneRect(background.rect());
    }

    const qreal width = scene->sceneRect().width();
    const qreal height = scene->sceneRect().height();
    const qreal x = width / 2;
    const qreal topMargin = 190;
    const qreal bottomMargin = 260;
    const qreal step = (height - topMargin - bottomMargin) / BaseLayerCount;
    const QPixmap defeatedIcon = loadPicture(QStringLiteral("ink-swirl.png"));

    const std::vector<std::vector<QPointF>> layerPositions = {
        {{width * 0.36, height - bottomMargin},
         {width * 0.50, height - bottomMargin},
         {width * 0.64, height - bottomMargin}},
        {{width * 0.36, height - bottomMargin - step},
         {width * 0.50, height - bottomMargin - step},
         {width * 0.64, height - bottomMargin - step}},
        {{width * 0.36, height - bottomMargin - step * 2},
         {width * 0.50, height - bottomMargin - step * 2},
         {width * 0.64, height - bottomMargin - step * 2}},
        {{width * 0.36, height - bottomMargin - step * 3},
         {width * 0.50, height - bottomMargin - step * 3},
         {width * 0.64, height - bottomMargin - step * 3}},
        {{width * 0.36, height - bottomMargin - step * 4},
         {width * 0.50, height - bottomMargin - step * 4},
         {width * 0.64, height - bottomMargin - step * 4}},
        {{width * 0.36, height - bottomMargin - step * 5},
         {width * 0.50, height - bottomMargin - step * 5},
         {width * 0.64, height - bottomMargin - step * 5}},
        {{width * 0.36, height - bottomMargin - step * 6},
         {width * 0.50, height - bottomMargin - step * 6},
         {width * 0.64, height - bottomMargin - step * 6}},
        {{width * 0.36, height - bottomMargin - step * 7},
         {width * 0.50, height - bottomMargin - step * 7},
         {width * 0.64, height - bottomMargin - step * 7}},
        {{width * 0.36, height - bottomMargin - step * 8},
         {width * 0.50, height - bottomMargin - step * 8},
         {width * 0.64, height - bottomMargin - step * 8}},
        {{width * 0.36, height - bottomMargin - step * 9},
         {width * 0.50, height - bottomMargin - step * 9},
         {width * 0.64, height - bottomMargin - step * 9}},
        {{width * 0.36, height - bottomMargin - step * 10},
         {width * 0.50, height - bottomMargin - step * 10},
         {width * 0.64, height - bottomMargin - step * 10}},
        {{width * 0.36, height - bottomMargin - step * 11},
         {width * 0.50, height - bottomMargin - step * 11},
         {width * 0.64, height - bottomMargin - step * 11}},
        {{width * 0.36, height - bottomMargin - step * 12},
         {width * 0.50, height - bottomMargin - step * 12},
         {width * 0.64, height - bottomMargin - step * 12}},
        {{x, topMargin}},
    };

    int nodeId = 1;
    for (int layer = 0; layer < BaseLayerCount; ++layer) {
        std::vector<MapNode*> nodeLayer;
        nodeLayer.reserve(ChoicesPerBaseLayer);

        for (int index = 0; index < ChoicesPerBaseLayer; ++index) {
            QString iconName;
            if (layer == 0) {
                iconName = QStringLiteral("barbute.png");
            } else if (layer == BaseLayerCount - 1) {
                iconName = QStringLiteral("campfire.png");
            } else {
                iconName = randomRoomIcon(true);
            }

            MapNode *node = addNode(nodeId++, layerPositions[layer][index], loadPicture(iconName), RoomNodeRadius);
            node->setDefeatedIcon(defeatedIcon);
            node->setAvailable(layer == 0);
            nodeLayer.push_back(node);
            nodes.push_back(node);
        }

        layers.push_back(std::move(nodeLayer));
    }

    MapNode *bossNode = addNode(nodeId, layerPositions.back().front(), loadPicture(QStringLiteral("crowned-skull.png")), FinalBossNodeRadius);
    bossNode->setDefeatedIcon(defeatedIcon);
    bossNode->setAvailable(false);
    nodes.push_back(bossNode);
    layers.push_back({bossNode});
}

void MapView::fitWidthToView()
{
    const qreal sceneWidth = scene->sceneRect().width();
    if (sceneWidth <= 0) {
        return;
    }

    if (!isVisible()) {
        return;
    }

    if (view->viewport()->width() <= 1) {
        QTimer::singleShot(0, this, [this]() {
            fitWidthToView();
        });
        return;
    }

    view->resetTransform();
    const qreal scale = view->viewport()->width() / sceneWidth;
    view->scale(scale, scale);
}

MapNode* MapView::addNode(int id, const QPointF &pos, const QPixmap &icon, qreal radius)
{
    MapNode *node = new MapNode(id, pos, icon, radius);
    node->setDefeatedIcon(loadPicture(QStringLiteral("ink-swirl.png")));
    connect(node, &MapNode::clicked, this, &MapView::onNodeClicked);
    scene->addItem(node);
    return node;
}

QPixmap MapView::loadPicture(const QString &fileName) const
{
    QString normalized = fileName;
    normalized.replace("\\", "/");
    if (normalized.startsWith(QStringLiteral("resources/pictures/"))) {
        normalized.remove(0, QStringLiteral("resources/pictures/").length());
    }

    QPixmap pixmap(QStringLiteral(":/pictures/resources/pictures/%1").arg(normalized));
    if (pixmap.isNull()) {
        pixmap.load(QStringLiteral("resources/pictures/%1").arg(normalized));
    }
    return pixmap;
}

void MapView::makeLayerAvailable(int layerIndex)
{
    if (layerIndex < 0 || layerIndex >= static_cast<int>(layers.size())) {
        return;
    }
    for (MapNode *node : layers[layerIndex]) {
        node->setAvailable(true);
    }
}

void MapView::clearHintLines()
{
    for (QGraphicsLineItem *line : qAsConst(hintLines)) {
        if (line) {
            scene->removeItem(line);
            delete line;
        }
    }
    hintLines.clear();
}

void MapView::clearRouteLines()
{
    for (QGraphicsLineItem *line : qAsConst(routeLines)) {
        if (line) {
            scene->removeItem(line);
            delete line;
        }
    }
    routeLines.clear();
}

void MapView::drawHintLines(MapNode *fromNode, int nextLayerIndex)
{
    clearHintLines();
    if (!fromNode || nextLayerIndex < 0 || nextLayerIndex >= static_cast<int>(layers.size())) {
        return;
    }
    for (MapNode *target : layers[nextLayerIndex]) {
        QGraphicsLineItem *line = scene->addLine(QLineF(fromNode->pos(), target->pos()),
                                                 QPen(QColor(120, 170, 230, 115), 5, Qt::DashLine, Qt::RoundCap));
        line->setZValue(-50);
        hintLines.push_back(line);
    }
}

void MapView::drawRouteLine(MapNode *fromNode, MapNode *toNode)
{
    if (!fromNode || !toNode) {
        return;
    }

    QGraphicsLineItem *line = scene->addLine(QLineF(fromNode->pos(), toNode->pos()),
                                             QPen(QColor(225, 225, 245, 220), 6, Qt::SolidLine, Qt::RoundCap));
    line->setZValue(-45);
    routeLines.push_back(line);
}

void MapView::onNodeClicked(int nodeId)
{
    MapNode *clickedNode = nullptr;
    int clickedLayer = -1;
    for (int layer = 0; layer < static_cast<int>(layers.size()); ++layer) {
        for (MapNode *node : layers[layer]) {
            if (node->id() == nodeId) {
                clickedNode = node;
                clickedLayer = layer;
                break;
            }
        }
        if (clickedNode) {
            break;
        }
    }

    if (!clickedNode || clickedLayer != currentLayerIndex) {
        return;
    }

    clearHintLines();

    if (previousSelectedNode) {
        drawRouteLine(previousSelectedNode, clickedNode);
    }

    clickedNode->setDefeated(true);
    for (MapNode *node : layers[currentLayerIndex]) {
        if (node != clickedNode) {
            node->setAvailable(false);
        }
    }

    previousSelectedNode = clickedNode;
    const int nextLayer = currentLayerIndex + 1;
    if (nextLayer < static_cast<int>(layers.size())) {
        makeLayerAvailable(nextLayer);
        drawHintLines(previousSelectedNode, nextLayer);
    }

    currentLayerIndex = nextLayer;
    emit floorChanged(qMin(currentLayerIndex + 1, static_cast<int>(layers.size())));
    qDebug() << "Map node clicked:" << nodeId << "layer" << clickedLayer;
    emit nodeActivated(nodeId);
}
