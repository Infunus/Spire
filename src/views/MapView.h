#ifndef MAPVIEW_H
#define MAPVIEW_H

#include <QWidget>
#include <QGraphicsView>
#include <QGraphicsScene>
#include <vector>

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

signals:
    void nodeActivated(int nodeId);
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
    void clearHintLines();
    void clearRouteLines();
    void drawHintLines(MapNode *fromNode, int nextLayerIndex);
    void drawRouteLine(MapNode *fromNode, MapNode *toNode);

    QGraphicsView *view;
    QGraphicsScene *scene;
    QGraphicsPixmapItem *backgroundItem = nullptr;
    std::vector<MapNode*> nodes;
    std::vector<std::vector<MapNode*>> layers;
    std::vector<QGraphicsLineItem*> hintLines;
    std::vector<QGraphicsLineItem*> routeLines;
    MapNode *previousSelectedNode = nullptr;
    int currentLayerIndex = 0;
};

#endif // MAPVIEW_H
