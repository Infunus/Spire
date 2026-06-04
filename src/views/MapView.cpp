#include "MapView.h"
#include "MapNode.h"

#include <QGraphicsPixmapItem>
#include <QGraphicsLineItem>
#include <QPainter>
#include <QResizeEvent>
#include <QPropertyAnimation>
#include <QRandomGenerator>
#include <QScrollBar>
#include <QHBoxLayout>
#include <QDebug>
#include <QShowEvent>
#include <QTimer>
#include <utility>

namespace {
constexpr int BaseLayerCount = 13;
constexpr int FinalBossLayer = BaseLayerCount;
constexpr int ChoicesPerBaseLayer = 3;
constexpr qreal RoomNodeRadius = 28.8;
constexpr qreal FinalBossNodeRadius = 54;
constexpr qreal TopMargin = 190;
constexpr qreal BottomMargin = 260;

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

EventKind eventKindForIcon(const QString &iconName)
{
    if (iconName == QStringLiteral("locked-chest.png")) {
        return EventKind::Treasure;
    }
    if (iconName == QStringLiteral("brute.png")) {
        return EventKind::BossBattle;
    }
    if (iconName == QStringLiteral("camping-tent.png")) {
        return EventKind::Merchant;
    }
    if (iconName == QStringLiteral("campfire.png")) {
        return EventKind::RestSite;
    }
    if (iconName == QStringLiteral("crowned-skull.png")) {
        return EventKind::FinalBoss;
    }
    return EventKind::NormalBattle;
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

void MapView::resetMap()
{
    setupScene();
    QTimer::singleShot(0, this, [this]() {
        fitWidthToView();
    });
}

void MapView::setupScene()
{
    scene->clear();
    nodes.clear();
    layers.clear();
    nodeEvents.clear();
    connectionLines.clear();
    backgroundItem = nullptr;
    previousSelectedNode = nullptr;
    pendingSelectedNode = nullptr;
    pendingLayerIndex = -1;
    currentLayerIndex = 0;

    const QPixmap background = loadPicture(QStringLiteral("background.png"));
    if (!background.isNull()) {
        backgroundItem = scene->addPixmap(background);
        backgroundItem->setZValue(-1);
        scene->setSceneRect(background.rect());
    }

    const qreal width = scene->sceneRect().width();
    const qreal height = scene->sceneRect().height();
    const qreal verticalSpacing = (height - TopMargin - BottomMargin) / BaseLayerCount;

    for (int layer = 0; layer <= FinalBossLayer; ++layer) {
        std::vector<MapNode*> nodeLayer;
        const bool isFinalBoss = (layer == FinalBossLayer);
        const bool isLastBaseLayer = (layer == BaseLayerCount - 1);
        const int nodesInLayer = isFinalBoss ? 1 : ChoicesPerBaseLayer;
        bool restNodeFound = false;

        for (int index = 0; index < nodesInLayer; ++index) {
            const qreal xPositions[ChoicesPerBaseLayer] = {width * 0.36, width * 0.50, width * 0.64};
            qreal x = isFinalBoss ? width * 0.5 : xPositions[index];
            qreal y = isFinalBoss ? TopMargin : height - BottomMargin - layer * verticalSpacing;
            QString iconName;
            if (isFinalBoss) {
                iconName = QStringLiteral("crowned-skull.png");
            } else if (layer == 0) {
                iconName = QStringLiteral("barbute.png");
            } else if (isLastBaseLayer) {
                iconName = QStringLiteral("campfire.png");
                restNodeFound = true;
            } else {
                iconName = randomRoomIcon(true);
                if (iconName == QStringLiteral("campfire.png")) {
                    restNodeFound = true;
                }
            }
            MapNode *node = addNode(layer * ChoicesPerBaseLayer + index, QPointF(x, y), loadPicture(iconName), isFinalBoss ? FinalBossNodeRadius : RoomNodeRadius);
            nodeEvents[node->id()] = eventKindForIcon(iconName);
            nodeLayer.push_back(node);
        }

        if (isLastBaseLayer && !restNodeFound) {
            const int forcedIndex = QRandomGenerator::global()->bounded(ChoicesPerBaseLayer);
            MapNode *forcedNode = nodeLayer[forcedIndex];
            forcedNode->setIcon(loadPicture(QStringLiteral("campfire.png")));
            nodeEvents[forcedNode->id()] = EventKind::RestSite;
        }

        if (!nodeLayer.empty()) {
            for (MapNode *node : nodeLayer) {
                node->setDefeatedIcon(loadPicture(QStringLiteral("ink-swirl.png")));
                node->setAvailable(layer == 0);
                nodes.push_back(node);
            }
            layers.push_back(std::move(nodeLayer));
        }
    }
}

void MapView::fitWidthToView()
{
    const qreal sceneWidth = scene ? scene->sceneRect().width() : 0;
    if (sceneWidth <= 0 || !isVisible()) {
        return;
    }

    if (view->viewport()->width() <= 1) {
        QTimer::singleShot(0, this, [this]() {
            fitWidthToView();
        });
        return;
    }

    const int oldScroll = view->verticalScrollBar()->value();
    view->resetTransform();
    const qreal scale = view->viewport()->width() / sceneWidth;
    view->scale(scale, scale);

    if (currentLayerIndex == 0 && !layers.empty() && !layers.front().empty()) {
        view->centerOn(layers.front().front());
    } else {
        view->verticalScrollBar()->setValue(oldScroll);
    }
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
    return QPixmap(QStringLiteral(":/pictures/resources/pictures/%1").arg(fileName));
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

void MapView::clearConnectionLines()
{
    for (QGraphicsLineItem *line : std::as_const(connectionLines)) {
        if (line) {
            scene->removeItem(line);
            delete line;
        }
    }
    connectionLines.clear();
}

void MapView::drawConnections(MapNode *fromNode, int nextLayerIndex)
{
    clearConnectionLines();
    if (!fromNode || nextLayerIndex < 0 || nextLayerIndex >= static_cast<int>(layers.size())) {
        return;
    }
    for (MapNode *target : layers[nextLayerIndex]) {
        QGraphicsLineItem *line = scene->addLine(QLineF(fromNode->pos(), target->pos()), QPen(QColor(135, 135, 170, 180), 6, Qt::SolidLine, Qt::RoundCap));
        line->setZValue(-0.5);
        connectionLines.push_back(line);
    }
}

void MapView::onNodeClicked(int nodeId)
{
    if (pendingSelectedNode) {
        return;
    }

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

    clearConnectionLines();

    for (MapNode *node : layers[currentLayerIndex]) {
        node->setAvailable(false);
    }

    pendingSelectedNode = clickedNode;
    pendingLayerIndex = currentLayerIndex;
    qDebug() << "Map node selected:" << nodeId << "layer" << clickedLayer;
    emit eventActivated(nodeEvents[nodeId]);
    emit nodeActivated(nodeId);
}

void MapView::completeCurrentNode()
{
    if (!pendingSelectedNode || pendingLayerIndex != currentLayerIndex) {
        return;
    }

    pendingSelectedNode->setDefeated(true);
    previousSelectedNode = pendingSelectedNode;

    const int nextLayer = currentLayerIndex + 1;
    if (nextLayer < static_cast<int>(layers.size())) {
        makeLayerAvailable(nextLayer);
        drawConnections(previousSelectedNode, nextLayer);
    }

    currentLayerIndex = nextLayer;
    emit floorChanged(qMin(currentLayerIndex, static_cast<int>(layers.size())));

    pendingSelectedNode = nullptr;
    pendingLayerIndex = -1;

    if (currentLayerIndex < static_cast<int>(layers.size()) && !layers[currentLayerIndex].empty()) {
        view->centerOn(layers[currentLayerIndex].front());
    }
}
