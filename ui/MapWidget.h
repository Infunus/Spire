#ifndef MAPWIDGET_H
#define MAPWIDGET_H

#include "../core/GameState.h"
#include "../core/GameBalance.h"
#include "../core/GameRandom.h"
#include "../core/GameText.h"

#include <QColor>
#include <QIcon>
#include <QJsonArray>
#include <QJsonObject>
#include <QList>
#include <QPainter>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>

struct MapRoomData
{
    int id = 0;
    int layer = 0;
    int index = 0;
    MapNodeType type = MapNodeType::Battle;
    QPushButton *button = nullptr;
    bool available = false;
    bool completed = false;
    bool pending = false;
};

class MapCanvasWidget : public QWidget
{
public:
    explicit MapCanvasWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setMinimumSize(900, 2200);
    }

    void setBackgroundImage(const QString &imagePath)
    {
        m_background = QPixmap(imagePath);
        update();
    }

    void setRooms(const QList<MapRoomData *> *rooms)
    {
        m_rooms = rooms;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing, true);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        if (m_background.isNull()) {
            painter.fillRect(rect(), QColor(11, 18, 28));
        } else {
            const QSize sourceSize = m_background.size();
            const double scale = qMax(width() / double(sourceSize.width()),
                                      height() / double(sourceSize.height()));
            const QSize targetSize(sourceSize.width() * scale, sourceSize.height() * scale);
            const QPoint targetTopLeft((width() - targetSize.width()) / 2,
                                       (height() - targetSize.height()) / 2);
            painter.drawPixmap(QRect(targetTopLeft, targetSize), m_background);
        }

        painter.fillRect(rect(), QColor(5, 8, 12, 58));
        drawConnections(&painter);
    }

private:
    QPoint buttonCenter(const QPushButton *button) const
    {
        return button ? button->geometry().center() : QPoint();
    }

    void drawConnections(QPainter *painter)
    {
        if (!m_rooms) {
            return;
        }

        QPen basePen(QColor(210, 184, 124, 90), 4, Qt::SolidLine, Qt::RoundCap);
        QPen activePen(QColor(255, 224, 142, 190), 6, Qt::SolidLine, Qt::RoundCap);

        for (MapRoomData *from : *m_rooms) {
            if (!from || !from->button) {
                continue;
            }
            for (MapRoomData *to : *m_rooms) {
                if (!to || !to->button || to->layer != from->layer + 1) {
                    continue;
                }
                if (qAbs(to->index - from->index) > 1 && to->index != 0 && from->index != 0) {
                    continue;
                }

                const bool highlighted = from->completed || from->pending || to->available;
                painter->setPen(highlighted ? activePen : basePen);
                painter->drawLine(buttonCenter(from->button), buttonCenter(to->button));
            }
        }
    }

    QPixmap m_background;
    const QList<MapRoomData *> *m_rooms = nullptr;
};

class MapWidget : public QWidget
{
public:
    explicit MapWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_scrollArea(new QScrollArea(this)),
          m_canvas(new MapCanvasWidget(this))
    {
        setObjectName("MapWidget");
        setStyleSheet(
            "#MapWidget { background: #07101a; }"
            "QScrollArea { border: 0; background: transparent; }"
            "QPushButton {"
            "  background: rgba(27, 35, 48, 210);"
            "  border: 2px solid rgba(158, 170, 185, 160);"
            "  border-radius: 22px;"
            "  color: #f4ead2;"
            "  font-size: 15px;"
            "  font-weight: 900;"
            "  padding: 5px 10px;"
            "}"
            "QPushButton:hover { background: rgba(72, 87, 110, 230); }"
            "QPushButton:disabled { color: rgba(244, 234, 210, 125); }");

        m_scrollArea->setWidgetResizable(false);
        m_scrollArea->setAlignment(Qt::AlignHCenter | Qt::AlignTop);
        m_scrollArea->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        m_scrollArea->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
        m_scrollArea->setWidget(m_canvas);

        QVBoxLayout *layout = new QVBoxLayout(this);
        layout->setContentsMargins(0, 0, 0, 0);
        layout->addWidget(m_scrollArea);

        m_canvas->setRooms(&m_rooms);
        resetMap();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        updateCanvasSize();
    }

public:
    void setBackgroundImage(const QString &imagePath)
    {
        m_canvas->setBackgroundImage(imagePath);
    }

    void setNodeHandler(const std::function<void(MapNodeType)> &handler)
    {
        m_nodeHandler = handler;
    }

    void setInteractionEnabled(bool enabled)
    {
        if (m_interactionEnabled == enabled) {
            return;
        }
        m_interactionEnabled = enabled;
        refreshButtons();
    }

    void resetMap()
    {
        for (MapRoomData *room : m_rooms) {
            delete room->button;
            room->button = nullptr;
        }
        qDeleteAll(m_rooms);
        m_rooms.clear();
        m_currentLayer = 0;
        m_pendingRoom = nullptr;

        const int canvasHeight = mapCanvasHeight();
        const int canvasWidth = qMax(900, m_scrollArea->viewport()->width());
        m_canvas->setFixedSize(canvasWidth, canvasHeight);

        int id = 0;
        for (int layer = 0; layer < m_layerCount; ++layer) {
            const bool bossLayer = (layer == m_layerCount - 1);
            const int count = bossLayer ? 1 : GameBalance::Map::normalRoomsPerLayer();
            for (int index = 0; index < count; ++index) {
                MapRoomData *room = new MapRoomData;
                room->id = id++;
                room->layer = layer;
                room->index = bossLayer ? GameBalance::Map::bossRoomIndex() : index;
                room->type = nodeTypeForLayer(layer, index);
                room->available = (layer == 0);

                QPushButton *button = new QPushButton(nodeName(room->type), m_canvas);
                button->setCursor(Qt::PointingHandCursor);
                button->setFixedSize(bossLayer ? QSize(154, 78) : QSize(132, 66));
                button->setIcon(nodeIcon(room->type));
                button->setIconSize(bossLayer ? QSize(30, 30) : QSize(22, 22));
                connect(button, &QPushButton::clicked, this, [this, room]() {
                    activateRoom(room);
                });
                room->button = button;
                m_rooms.append(room);
            }
        }

        layoutRooms();
        refreshButtons();
        QTimer::singleShot(0, this, [this]() {
            m_scrollArea->verticalScrollBar()->setValue(m_scrollArea->verticalScrollBar()->maximum());
        });
    }

    void completeCurrentNode()
    {
        if (!m_pendingRoom) {
            return;
        }

        m_pendingRoom->pending = false;
        m_pendingRoom->completed = true;
        ++m_currentLayer;
        m_pendingRoom = nullptr;

        for (MapRoomData *room : m_rooms) {
            room->available = (room->layer == m_currentLayer);
        }

        refreshButtons();
        centerCurrentLayer();
    }

    void cancelPendingNode()
    {
        if (!m_pendingRoom) {
            return;
        }

        const int layer = m_pendingRoom->layer;
        m_pendingRoom->pending = false;
        m_pendingRoom = nullptr;

        for (MapRoomData *room : m_rooms) {
            room->available = (room->layer == layer && !room->completed);
        }
        refreshButtons();
    }

    QJsonObject toJson() const
    {
        QJsonArray roomArray;
        for (const MapRoomData *room : m_rooms) {
            if (!room) {
                continue;
            }

            QJsonObject roomObject;
            roomObject[QStringLiteral("id")] = room->id;
            roomObject[QStringLiteral("layer")] = room->layer;
            roomObject[QStringLiteral("index")] = room->index;
            roomObject[QStringLiteral("type")] = static_cast<int>(room->type);
            roomObject[QStringLiteral("available")] = room->available;
            roomObject[QStringLiteral("completed")] = room->completed;
            roomObject[QStringLiteral("pending")] = room->pending;
            roomArray.append(roomObject);
        }

        QJsonObject json;
        json[QStringLiteral("currentLayer")] = m_currentLayer;
        json[QStringLiteral("rooms")] = roomArray;
        return json;
    }

    bool loadFromJson(const QJsonObject &json)
    {
        const QJsonArray roomArray = json.value(QStringLiteral("rooms")).toArray();
        if (roomArray.isEmpty()) {
            return false;
        }

        for (MapRoomData *room : m_rooms) {
            delete room->button;
            room->button = nullptr;
        }
        qDeleteAll(m_rooms);
        m_rooms.clear();
        m_pendingRoom = nullptr;
        m_currentLayer = qBound(0,
                                json.value(QStringLiteral("currentLayer")).toInt(0),
                                m_layerCount);

        const int canvasHeight = mapCanvasHeight();
        const int canvasWidth = qMax(900, m_scrollArea->viewport()->width());
        m_canvas->setFixedSize(canvasWidth, canvasHeight);

        for (const QJsonValue &value : roomArray) {
            const QJsonObject roomObject = value.toObject();
            const int layer = roomObject.value(QStringLiteral("layer")).toInt(-1);
            if (layer < 0 || layer >= m_layerCount) {
                continue;
            }

            const int typeValue = qBound(static_cast<int>(MapNodeType::Battle),
                                         roomObject.value(QStringLiteral("type"))
                                             .toInt(static_cast<int>(MapNodeType::Battle)),
                                         static_cast<int>(MapNodeType::Boss));

            MapRoomData *room = new MapRoomData;
            room->id = roomObject.value(QStringLiteral("id")).toInt(m_rooms.size());
            room->layer = layer;
            room->index = roomObject.value(QStringLiteral("index")).toInt(0);
            room->type = static_cast<MapNodeType>(typeValue);
            room->available = roomObject.value(QStringLiteral("available")).toBool(false);
            room->completed = roomObject.value(QStringLiteral("completed")).toBool(false);
            room->pending = roomObject.value(QStringLiteral("pending")).toBool(false);

            const bool bossRoom = room->type == MapNodeType::Boss;
            QPushButton *button = new QPushButton(nodeName(room->type), m_canvas);
            button->setCursor(Qt::PointingHandCursor);
            button->setFixedSize(bossRoom ? QSize(154, 78) : QSize(132, 66));
            button->setIcon(nodeIcon(room->type));
            button->setIconSize(bossRoom ? QSize(30, 30) : QSize(22, 22));
            connect(button, &QPushButton::clicked, this, [this, room]() {
                activateRoom(room);
            });
            room->button = button;

            if (room->pending && !m_pendingRoom) {
                m_pendingRoom = room;
            } else if (room->pending) {
                room->pending = false;
            }

            m_rooms.append(room);
        }

        if (m_rooms.isEmpty()) {
            resetMap();
            return false;
        }

        layoutRooms();
        refreshButtons();
        QTimer::singleShot(0, this, [this]() {
            centerCurrentLayer();
        });
        return true;
    }

private:
    void updateCanvasSize()
    {
        if (!m_canvas || !m_scrollArea) {
            return;
        }

        const int newWidth = qMax(900, m_scrollArea->viewport()->width());
        const int newHeight = mapCanvasHeight();
        if (newWidth != m_canvas->width() || newHeight != m_canvas->height()) {
            m_canvas->setFixedSize(newWidth, newHeight);
            layoutRooms();
            m_canvas->update();
        }
    }

    int mapCanvasHeight() const
    {
        return qMax(2200, 360 + m_layerCount * 260);
    }

    MapNodeType nodeTypeForLayer(int layer, int index) const
    {
        if (layer == m_layerCount - 1) {
            return MapNodeType::Boss;
        }
        if (layer == 0) {
            return MapNodeType::Battle;
        }
        if (layer == m_layerCount - 2) {
            return MapNodeType::Rest;
        }

        const int roll = GameRandom::instance().bounded(GameBalance::Map::randomRollMax());
        if (roll < GameBalance::Map::battleThreshold()) {
            return MapNodeType::Battle;
        }
        if (roll < GameBalance::Map::eventThreshold()) {
            return MapNodeType::Event;
        }
        if (roll < GameBalance::Map::shopThreshold()) {
            return MapNodeType::Shop;
        }
        if (roll < GameBalance::Map::rewardThreshold()) {
            return MapNodeType::Reward;
        }
        return index == GameBalance::Map::bossRoomIndex() ? MapNodeType::Rest : MapNodeType::Battle;
    }

    QString nodeName(MapNodeType type) const
    {
        if (type == MapNodeType::Event) {
            return GameText::MapText::eventNodeName();
        }
        if (type == MapNodeType::Shop) {
            return GameText::MapText::shopNodeName();
        }
        if (type == MapNodeType::Rest) {
            return GameText::MapText::restNodeName();
        }
        if (type == MapNodeType::Reward) {
            return GameText::MapText::rewardNodeName();
        }
        if (type == MapNodeType::Boss) {
            return GameText::MapText::bossNodeName();
        }
        return GameText::MapText::battleNodeName();
    }

    QIcon nodeIcon(MapNodeType type) const
    {
        QPixmap pixmap(48, 48);
        pixmap.fill(Qt::transparent);

        QPainter painter(&pixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        QColor fill(94, 46, 48);
        QColor line(255, 216, 140);
        if (type == MapNodeType::Event) {
            fill = QColor(52, 68, 106);
            line = QColor(185, 215, 255);
        } else if (type == MapNodeType::Shop) {
            fill = QColor(86, 66, 35);
            line = QColor(255, 221, 142);
        } else if (type == MapNodeType::Rest) {
            fill = QColor(44, 86, 71);
            line = QColor(171, 236, 190);
        } else if (type == MapNodeType::Reward) {
            fill = QColor(96, 76, 35);
            line = QColor(255, 231, 142);
        } else if (type == MapNodeType::Boss) {
            fill = QColor(96, 35, 47);
            line = QColor(255, 183, 128);
        }

        painter.setPen(QPen(line, 3));
        painter.setBrush(fill);
        painter.drawEllipse(QRectF(5, 5, 38, 38));

        painter.setPen(QPen(QColor(255, 246, 218), 4, Qt::SolidLine, Qt::RoundCap, Qt::RoundJoin));
        if (type == MapNodeType::Shop) {
            painter.drawLine(QPointF(17, 21), QPointF(31, 21));
            painter.drawLine(QPointF(18, 21), QPointF(20, 33));
            painter.drawLine(QPointF(30, 21), QPointF(28, 33));
            painter.drawLine(QPointF(20, 33), QPointF(28, 33));
        } else if (type == MapNodeType::Rest) {
            painter.drawLine(QPointF(24, 16), QPointF(24, 33));
            painter.drawLine(QPointF(18, 28), QPointF(30, 28));
        } else if (type == MapNodeType::Event) {
            painter.drawEllipse(QPointF(24, 19), 3, 3);
            painter.drawLine(QPointF(24, 27), QPointF(24, 32));
        } else if (type == MapNodeType::Reward) {
            painter.drawRect(QRectF(16, 20, 16, 14));
            painter.drawLine(QPointF(16, 20), QPointF(32, 20));
            painter.drawLine(QPointF(24, 20), QPointF(24, 34));
        } else if (type == MapNodeType::Boss) {
            painter.drawLine(QPointF(17, 31), QPointF(31, 17));
            painter.drawLine(QPointF(20, 16), QPointF(32, 28));
        } else {
            painter.drawLine(QPointF(17, 31), QPointF(31, 17));
            painter.drawLine(QPointF(19, 17), QPointF(31, 29));
        }

        return QIcon(pixmap);
    }

    void layoutRooms()
    {
        const int top = 110;
        const int bottom = 130;
        const int usableHeight = m_canvas->height() - top - bottom;
        const int layerSpacing = usableHeight / qMax(1, m_layerCount - 1);

        for (MapRoomData *room : m_rooms) {
            const int y = m_canvas->height() - bottom - room->layer * layerSpacing;
            const int x = room->type == MapNodeType::Boss
                              ? m_canvas->width() / 2
                              : m_canvas->width() * (32 + room->index * 18) / 100;
            room->button->move(x - room->button->width() / 2,
                               y - room->button->height() / 2);
        }
    }

    void activateRoom(MapRoomData *room)
    {
        if (!m_interactionEnabled) {
            return;
        }

        if (room && room->pending && m_pendingRoom == room) {
            if (m_nodeHandler) {
                m_nodeHandler(room->type);
            }
            return;
        }

        if (!room || !room->available || room->completed || m_pendingRoom) {
            return;
        }

        for (MapRoomData *other : m_rooms) {
            if (other->layer == room->layer) {
                other->available = false;
            }
        }
        room->pending = true;
        m_pendingRoom = room;
        refreshButtons();

        if (m_nodeHandler) {
            m_nodeHandler(room->type);
        }
    }

    void refreshButtons()
    {
        for (MapRoomData *room : m_rooms) {
            QString text = nodeName(room->type);
            if (room->completed) {
                text += QStringLiteral("\n") + GameText::MapText::completedNodeSuffix();
            }
            room->button->setText(text);
            room->button->setEnabled(m_interactionEnabled
                                     && ((room->available && !room->completed && !room->pending)
                                      || room->pending));

            QString style;
            if (room->completed) {
                style = "background: rgba(39, 82, 72, 220); border-color: rgba(154, 228, 185, 190); color: #dfffe8;";
                room->button->setToolTip(GameText::MapText::completedNodeSuffix());
            } else if (room->pending) {
                style = "background: rgba(119, 85, 36, 235); border-color: rgba(255, 219, 128, 220); color: #fff0c6;";
                room->button->setToolTip(GameText::MapText::selectedTooltip());
            } else if (room->available) {
                style = "background: rgba(94, 46, 48, 230); border-color: rgba(255, 216, 140, 220); color: #fff5db;";
                room->button->setToolTip(GameText::MapText::availableTooltip());
            } else {
                style = "background: rgba(25, 31, 40, 170); border-color: rgba(120, 130, 145, 105); color: rgba(244, 234, 210, 125);";
                room->button->setToolTip(GameText::MapText::lockedTooltip());
            }
            room->button->setStyleSheet(style);
        }
        m_canvas->update();
    }

    void centerCurrentLayer()
    {
        const int top = 110;
        const int bottom = 130;
        const int usableHeight = m_canvas->height() - top - bottom;
        const int layerSpacing = usableHeight / qMax(1, m_layerCount - 1);
        const int y = m_canvas->height() - bottom - m_currentLayer * layerSpacing;
        m_scrollArea->ensureVisible(m_canvas->width() / 2, y, 60, 220);
    }

    QScrollArea *m_scrollArea;
    MapCanvasWidget *m_canvas;
    QList<MapRoomData *> m_rooms;
    MapRoomData *m_pendingRoom = nullptr;
    std::function<void(MapNodeType)> m_nodeHandler;
    bool m_interactionEnabled = true;
    int m_currentLayer = 0;
    const int m_layerCount = GameBalance::Map::layerCount();
};

#endif // MAPWIDGET_H
