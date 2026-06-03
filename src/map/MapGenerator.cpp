#include "MapGenerator.h"

std::vector<QPointF> MapGenerator::generateNodePositions(int layerCount, int nodesPerLayer, qreal width, qreal height)
{
    std::vector<QPointF> positions;
    const qreal layerSpacing = height / qMax(1, layerCount);
    for (int layer = 0; layer < layerCount; ++layer) {
        for (int index = 0; index < nodesPerLayer; ++index) {
            qreal x = width * (0.15 + index * 0.7 / qMax(1, nodesPerLayer - 1));
            qreal y = 120 + layer * layerSpacing;
            positions.emplace_back(x, y);
        }
    }
    return positions;
}
