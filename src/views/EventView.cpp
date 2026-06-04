#include "EventView.h"

#include "data/GameText.h"

#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>

class EventBackgroundLayer : public QWidget
{
public:
    explicit EventBackgroundLayer(QWidget *parent = nullptr)
        : QWidget(parent)
        , background(GameText::Assets::eventBackground())
    {
        setObjectName(QStringLiteral("EventBackgroundLayer"));
        setAttribute(Qt::WA_TransparentForMouseEvents);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event)

        QPainter painter(this);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);
        if (!background.isNull()) {
            const double scale = qMax(width() / double(background.width()), height() / double(background.height()));
            const QSize targetSize(background.width() * scale, background.height() * scale);
            const QPoint topLeft((width() - targetSize.width()) / 2, (height() - targetSize.height()) / 2);
            painter.drawPixmap(QRect(topLeft, targetSize), background);
        } else {
            QLinearGradient fallback(rect().topLeft(), rect().bottomRight());
            fallback.setColorAt(0.0, QColor(10, 14, 26));
            fallback.setColorAt(0.55, QColor(49, 25, 44));
            fallback.setColorAt(1.0, QColor(13, 35, 43));
            painter.fillRect(rect(), fallback);
        }
        painter.fillRect(rect(), QColor(3, 5, 12, 95));
    }

private:
    QPixmap background;
};

EventView::EventView(QWidget *parent)
    : QWidget(parent)
    , iconLabel(new QLabel(this))
    , titleLabel(new QLabel(this))
    , descriptionLabel(new QLabel(this))
    , exitButton(new QPushButton(GameText::EventText::exitButton(), this))
{
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "EventView { background: #090b12; }"
        "QLabel { color: #f7ead0; }"
        "QPushButton {"
        "  background: rgba(255, 226, 168, 34);"
        "  border: 1px solid rgba(255, 226, 168, 135);"
        "  border-radius: 8px;"
        "  color: #fff1cc;"
        "  font-size: 16px;"
        "  font-weight: 800;"
        "  padding: 10px 18px;"
        "}"
        "QPushButton:hover { background: rgba(235, 184, 88, 96); border-color: rgba(255, 235, 177, 220); }");

    EventBackgroundLayer *backgroundLayer = new EventBackgroundLayer(this);
    backgroundLayer->lower();

    QLabel *pageLabel = new QLabel(GameText::EventText::pageTitle(), this);
    pageLabel->setAlignment(Qt::AlignCenter);
    pageLabel->setStyleSheet("color: rgba(255,241,204,215); font-size: 18px; font-weight: 900;");

    QFrame *mainPanel = new QFrame(this);
    mainPanel->setObjectName("EventMainPanel");
    mainPanel->setStyleSheet(
        "QFrame#EventMainPanel {"
        "  background: rgba(15, 18, 28, 188);"
        "  border: 1px solid rgba(255, 217, 137, 145);"
        "  border-radius: 14px;"
        "}");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainPanel);
    shadow->setBlurRadius(28);
    shadow->setColor(QColor(0, 0, 0, 145));
    shadow->setOffset(0, 12);
    mainPanel->setGraphicsEffect(shadow);

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setMinimumSize(330, 250);
    iconLabel->setStyleSheet(
        "background: rgba(255, 226, 168, 32);"
        "border: 1px solid rgba(255, 226, 168, 125);"
        "border-radius: 12px;");

    titleLabel->setWordWrap(true);
    titleLabel->setStyleSheet("color: #fff6dc; font-size: 34px; font-weight: 900;");

    descriptionLabel->setWordWrap(true);
    descriptionLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    descriptionLabel->setStyleSheet("color: rgba(255,247,224,220); font-size: 17px;");

    QScrollArea *bodyScroll = new QScrollArea(mainPanel);
    bodyScroll->setWidgetResizable(true);
    bodyScroll->setFrameShape(QFrame::NoFrame);
    bodyScroll->setStyleSheet("background: transparent;");
    QWidget *bodyPage = new QWidget(bodyScroll);
    QVBoxLayout *bodyLayout = new QVBoxLayout(bodyPage);
    bodyLayout->setContentsMargins(0, 0, 8, 0);
    bodyLayout->addWidget(descriptionLabel);
    bodyLayout->addStretch();
    bodyScroll->setWidget(bodyPage);

    QWidget *textArea = new QWidget(mainPanel);
    QVBoxLayout *textLayout = new QVBoxLayout(textArea);
    textLayout->setContentsMargins(0, 0, 0, 0);
    textLayout->setSpacing(16);
    textLayout->addWidget(titleLabel);
    textLayout->addWidget(bodyScroll, 1);

    QHBoxLayout *mainLayout = new QHBoxLayout(mainPanel);
    mainLayout->setContentsMargins(24, 24, 24, 24);
    mainLayout->setSpacing(24);
    mainLayout->addWidget(iconLabel, 0, Qt::AlignTop);
    mainLayout->addWidget(textArea, 1);

    QFrame *choicePanel = new QFrame(this);
    choicePanel->setStyleSheet(
        "QFrame { background: rgba(8, 11, 18, 175); border: 1px solid rgba(255, 217, 137, 125); border-radius: 12px; }");
    QVBoxLayout *choiceLayout = new QVBoxLayout(choicePanel);
    choiceLayout->setContentsMargins(18, 16, 18, 16);
    choiceLayout->addWidget(exitButton);

    connect(exitButton, &QPushButton::clicked, this, &EventView::eventFinished);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(56, 42, 56, 46);
    layout->setSpacing(18);
    layout->addWidget(pageLabel);
    layout->addWidget(mainPanel, 1);
    layout->addWidget(choicePanel);
}

void EventView::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (QWidget *layer = findChild<QWidget*>(QStringLiteral("EventBackgroundLayer"))) {
        layer->setGeometry(rect());
    }
}

void EventView::setEvent(const EventData &eventData)
{
    QPixmap icon(eventData.iconPath);
    if (!icon.isNull()) {
        iconLabel->setText(QString());
        iconLabel->setPixmap(icon.scaled(iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        iconLabel->setPixmap(QPixmap());
        iconLabel->setText(GameText::EventText::imagePlaceholder());
    }

    titleLabel->setText(eventData.name);
    descriptionLabel->setText(GameText::EventText::enterFormat().arg(eventData.name, eventData.description));
    exitButton->setText(eventData.kind == EventKind::Death
                            ? GameText::EventText::confirmButton()
                            : GameText::EventText::exitButton());
}
