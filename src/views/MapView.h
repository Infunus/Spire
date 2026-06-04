#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <unordered_map>
#include <vector>

#include "data/EventData.h"

class MapNode;
class QResizeEvent;
class QShowEvent;
class QGraphicsPixmapItem;
class QGraphicsLineItem;

class MapView : public QWidget
{
    Q_OBJECT

public:
    explicit MapView(QWidget *parent = nullptr);
    QSize minimumSizeHint() const override;
    void resetMap();
    void completeCurrentNode();

signals:
    void nodeActivated(int nodeId);
    void eventActivated(EventKind eventKind);
    void floorChanged(int floor);

private slots:
    void onNodeClicked(int nodeId);

protected:
    void resizeEvent(QResizeEvent *event) override;
    void showEvent(QShowEvent *event) override;

private:
    void setupScene();
    void fitWidthToView();
    MapNode* addNode(int id, const QPointF &pos, const QPixmap &icon = QPixmap(), qreal radius = 28.8);
    QPixmap loadPicture(const QString &fileName) const;
    void makeLayerAvailable(int layerIndex);
    void clearConnectionLines();
    void drawConnections(MapNode *fromNode, int nextLayerIndex);

    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *backgroundItem = nullptr;
    std::vector<MapNode*> nodes;
    std::vector<std::vector<MapNode*>> layers;
    std::unordered_map<int, EventKind> nodeEvents;
    std::vector<QGraphicsLineItem*> connectionLines;
    MapNode *previousSelectedNode = nullptr;
    MapNode *pendingSelectedNode = nullptr;
    int pendingLayerIndex = -1;
    int currentLayerIndex = 0;
};

#endif // MAPVIEW_H
