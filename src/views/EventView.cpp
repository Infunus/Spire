#include "EventView.h"

#include <QLabel>
#include <QPixmap>
#include <QPushButton>
#include <QVBoxLayout>

EventView::EventView(QWidget *parent)
    : QWidget(parent)
    , iconLabel(new QLabel(this))
    , titleLabel(new QLabel(this))
    , descriptionLabel(new QLabel(this))
    , exitButton(new QPushButton(QString::fromUtf8(u8"退出事件"), this))
{
    setStyleSheet(
        "EventView { background: #171923; }"
        "QLabel { color: #f5ead2; }"
        "QPushButton { color: #f8edd4; background: #5d6f9f; border: 1px solid rgba(255,255,255,90);"
        "              border-radius: 4px; padding: 9px 18px; font-size: 16px; font-weight: 700; }"
        "QPushButton:hover { background: #7184ba; }");

    iconLabel->setAlignment(Qt::AlignCenter);
    iconLabel->setFixedSize(150, 150);

    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 30px; font-weight: 800;");

    descriptionLabel->setAlignment(Qt::AlignCenter);
    descriptionLabel->setWordWrap(true);
    descriptionLabel->setStyleSheet("font-size: 18px;");
    descriptionLabel->setMinimumWidth(520);

    connect(exitButton, &QPushButton::clicked, this, &EventView::eventFinished);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(60, 60, 60, 60);
    layout->setSpacing(20);
    layout->addStretch();
    layout->addWidget(iconLabel, 0, Qt::AlignHCenter);
    layout->addWidget(titleLabel);
    layout->addWidget(descriptionLabel, 0, Qt::AlignHCenter);
    layout->addWidget(exitButton, 0, Qt::AlignHCenter);
    layout->addStretch();
}

void EventView::setEvent(const EventData &eventData)
{
    QPixmap icon(eventData.iconPath);
    if (!icon.isNull()) {
        iconLabel->setPixmap(icon.scaled(iconLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        iconLabel->clear();
    }

    titleLabel->setText(eventData.name);
    descriptionLabel->setText(QString::fromUtf8(u8"你进入了：%1\n\n%2").arg(eventData.name, eventData.description));
    exitButton->setText(eventData.kind == EventKind::Death
                            ? QString::fromUtf8(u8"确认")
                            : QString::fromUtf8(u8"退出事件"));
}
