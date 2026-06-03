#ifndef MAPGENERATOR_H
#define MAPGENERATOR_H

#include <QPointF>
#include <vector>

class MapGenerator
{
public:
    static std::vector<QPointF> generateNodePositions(int layerCount, int nodesPerLayer, qreal width, qreal height);
};

#endif // MAPGENERATOR_H
