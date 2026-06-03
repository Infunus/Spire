#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include "../core/GameText.h"

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QList>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QString>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

#include <functional>

struct RandomEventChoice
{
    QString text;

    // 事件效果先用备注字符串占位。后续可以改成回血、加牌、给遗物等真实逻辑。
    QString effectHint;
};

struct RandomEventData
{
    QString title;
    QString body;

    // 每个事件可以有自己的事件图。为空时显示占位图框。
    QString imagePath;

    QList<RandomEventChoice> choices;
};

class EventWidget : public QWidget
{
public:
    explicit EventWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_titleLabel(nullptr),
          m_bodyLabel(nullptr),
          m_eventImageLabel(nullptr),
          m_choiceLayout(nullptr),
          m_resultLabel(nullptr)
    {
        setObjectName("EventWidget");
        setAttribute(Qt::WA_StyledBackground, true);

        QVBoxLayout *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(56, 42, 56, 46);
        rootLayout->setSpacing(18);

        QLabel *pageLabel = new QLabel(GameText::EventText::pageTitle(), this);
        pageLabel->setAlignment(Qt::AlignCenter);
        pageLabel->setStyleSheet(
            "color: rgba(255, 241, 204, 215);"
            "font-size: 18px;"
            "font-weight: 900;"
            "letter-spacing: 1px;");

        QFrame *mainPanel = new QFrame(this);
        mainPanel->setObjectName("EventMainPanel");
        mainPanel->setStyleSheet(
            "QFrame#EventMainPanel {"
            "  background: rgba(15, 18, 28, 188);"
            "  border: 1px solid rgba(255, 217, 137, 145);"
            "  border-radius: 14px;"
            "}"
            "QLabel { color: #f7ead0; }");

        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainPanel);
        shadow->setBlurRadius(28);
        shadow->setColor(QColor(0, 0, 0, 145));
        shadow->setOffset(0, 12);
        mainPanel->setGraphicsEffect(shadow);

        QHBoxLayout *mainLayout = new QHBoxLayout(mainPanel);
        mainLayout->setContentsMargins(24, 24, 24, 24);
        mainLayout->setSpacing(24);

        m_eventImageLabel = new QLabel(mainPanel);
        m_eventImageLabel->setAlignment(Qt::AlignCenter);
        m_eventImageLabel->setMinimumSize(330, 250);
        m_eventImageLabel->setStyleSheet(
            "background: rgba(255, 226, 168, 32);"
            "border: 1px solid rgba(255, 226, 168, 125);"
            "border-radius: 12px;"
            "color: rgba(255, 244, 214, 170);"
            "font-size: 22px;"
            "font-weight: 900;");

        QWidget *textArea = new QWidget(mainPanel);
        QVBoxLayout *textLayout = new QVBoxLayout(textArea);
        textLayout->setContentsMargins(0, 0, 0, 0);
        textLayout->setSpacing(16);

        m_titleLabel = new QLabel(textArea);
        m_titleLabel->setWordWrap(true);
        m_titleLabel->setStyleSheet(
            "color: #fff6dc;"
            "font-size: 34px;"
            "font-weight: 900;");

        m_bodyLabel = new QLabel(textArea);
        m_bodyLabel->setWordWrap(true);
        m_bodyLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        m_bodyLabel->setStyleSheet(
            "color: rgba(255, 247, 224, 220);"
            "font-size: 17px;"
            "line-height: 150%;");

        QScrollArea *bodyScroll = new QScrollArea(textArea);
        bodyScroll->setWidgetResizable(true);
        bodyScroll->setFrameShape(QFrame::NoFrame);
        bodyScroll->setStyleSheet("background: transparent;");
        QWidget *bodyPage = new QWidget(bodyScroll);
        QVBoxLayout *bodyLayout = new QVBoxLayout(bodyPage);
        bodyLayout->setContentsMargins(0, 0, 8, 0);
        bodyLayout->addWidget(m_bodyLabel);
        bodyLayout->addStretch();
        bodyScroll->setWidget(bodyPage);

        textLayout->addWidget(m_titleLabel);
        textLayout->addWidget(bodyScroll, 1);

        mainLayout->addWidget(m_eventImageLabel, 0, Qt::AlignTop);
        mainLayout->addWidget(textArea, 1);

        QFrame *choicePanel = new QFrame(this);
        choicePanel->setObjectName("ChoicePanel");
        choicePanel->setStyleSheet(
            "QFrame#ChoicePanel {"
            "  background: rgba(8, 11, 18, 175);"
            "  border: 1px solid rgba(255, 217, 137, 125);"
            "  border-radius: 12px;"
            "}");

        m_choiceLayout = new QVBoxLayout(choicePanel);
        m_choiceLayout->setContentsMargins(18, 16, 18, 16);
        m_choiceLayout->setSpacing(10);

        m_resultLabel = new QLabel(this);
        m_resultLabel->setAlignment(Qt::AlignCenter);
        m_resultLabel->setStyleSheet(
            "color: rgba(255, 241, 204, 185);"
            "font-size: 15px;"
            "font-weight: 700;");

        rootLayout->addWidget(pageLabel);
        rootLayout->addWidget(mainPanel, 1);
        rootLayout->addWidget(choicePanel);
        rootLayout->addWidget(m_resultLabel);

        RandomEventData emptyEvent;
        emptyEvent.title = GameText::EventText::defaultTitle();
        emptyEvent.body = GameText::EventText::defaultBody();
        setEvent(emptyEvent);
    }

    void setBackgroundImage(const QString &imagePath)
    {
        m_background = QPixmap(imagePath);
        update();
    }

    void setEvent(const RandomEventData &eventData)
    {
        m_event = eventData;
        m_titleLabel->setText(m_event.title.isEmpty() ? GameText::EventText::defaultTitle() : m_event.title);
        m_bodyLabel->setText(m_event.body.isEmpty() ? GameText::EventText::defaultBody() : m_event.body);
        m_resultLabel->clear();
        updateEventImage();
        rebuildChoices();
    }

    void setChoiceHandler(const std::function<void(int, const RandomEventChoice &)> &handler)
    {
        m_choiceHandler = handler;
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

        if (m_background.isNull()) {
            QLinearGradient fallback(rect().topLeft(), rect().bottomRight());
            fallback.setColorAt(0.0, QColor(10, 14, 26));
            fallback.setColorAt(0.55, QColor(49, 25, 44));
            fallback.setColorAt(1.0, QColor(13, 35, 43));
            painter.fillRect(rect(), fallback);
        } else {
            const QSize sourceSize = m_background.size();
            const double scale = qMax(width() / double(sourceSize.width()),
                                      height() / double(sourceSize.height()));
            const QSize targetSize(sourceSize.width() * scale, sourceSize.height() * scale);
            const QPoint targetTopLeft((width() - targetSize.width()) / 2,
                                       (height() - targetSize.height()) / 2);
            painter.drawPixmap(QRect(targetTopLeft, targetSize), m_background);
        }

        painter.fillRect(rect(), QColor(3, 5, 12, 92));
    }

private:
    void updateEventImage()
    {
        const QPixmap eventImage(m_event.imagePath);
        if (eventImage.isNull()) {
            m_eventImageLabel->setPixmap(QPixmap());
            m_eventImageLabel->setText(GameText::EventText::eventImagePlaceholder());
            return;
        }

        m_eventImageLabel->setText(QString());
        m_eventImageLabel->setPixmap(eventImage.scaled(m_eventImageLabel->size(),
                                                       Qt::KeepAspectRatioByExpanding,
                                                       Qt::SmoothTransformation));
    }

    void rebuildChoices()
    {
        while (QLayoutItem *item = m_choiceLayout->takeAt(0)) {
            delete item->widget();
            delete item;
        }

        for (int i = 0; i < m_event.choices.size(); ++i) {
            const RandomEventChoice choice = m_event.choices.at(i);
            QPushButton *button = new QPushButton(
                GameText::EventText::choicePrefixFormat().arg(i + 1).arg(choice.text),
                this);
            button->setCursor(Qt::PointingHandCursor);
            button->setMinimumHeight(48);
            button->setStyleSheet(
                "QPushButton {"
                "  background: rgba(255, 226, 168, 34);"
                "  border: 1px solid rgba(255, 226, 168, 135);"
                "  border-radius: 8px;"
                "  color: #fff1cc;"
                "  font-size: 16px;"
                "  font-weight: 800;"
                "  padding: 10px 14px;"
                "  text-align: left;"
                "}"
                "QPushButton:hover {"
                "  background: rgba(235, 184, 88, 96);"
                "  border-color: rgba(255, 235, 177, 220);"
                "}"
                "QPushButton:pressed { background: rgba(174, 119, 48, 130); }");

            connect(button, &QPushButton::clicked, this, [this, i, choice]() {
                m_resultLabel->setText(GameText::EventText::resultFormat().arg(choice.text));

                // 后续在这里或外部 handler 中接入真实事件效果。
                // 例如：回血、删牌、加牌、获得遗物、进入商店、跳过下一场战斗等。
                if (m_choiceHandler) {
                    m_choiceHandler(i, choice);
                }
            });
            m_choiceLayout->addWidget(button);
        }
    }

    RandomEventData m_event;
    QPixmap m_background;
    QLabel *m_titleLabel;
    QLabel *m_bodyLabel;
    QLabel *m_eventImageLabel;
    QVBoxLayout *m_choiceLayout;
    QLabel *m_resultLabel;
    std::function<void(int, const RandomEventChoice &)> m_choiceHandler;
};

#endif // EVENTWIDGET_H
