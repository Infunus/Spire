#include "MainMenuView.h"

#include <QApplication>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>

MainMenuView::MainMenuView(QWidget *parent)
    : QWidget(parent)
{
    setStyleSheet(
        "MainMenuView { background: #141821; }"
        "QLabel { color: #f5ead2; }"
        "QPushButton { color: #f8edd4; background: #5d6f9f; border: 1px solid rgba(255,255,255,90);"
        "              border-radius: 4px; padding: 12px 26px; font-size: 18px; font-weight: 800; }"
        "QPushButton:hover { background: #7184ba; }");

    QLabel *titleLabel = new QLabel(QString::fromUtf8(u8"Slay The Boya"), this);
    titleLabel->setAlignment(Qt::AlignCenter);
    titleLabel->setStyleSheet("font-size: 46px; font-weight: 900;");

    QPushButton *startButton = new QPushButton(QString::fromUtf8(u8"开始游戏"), this);
    QPushButton *quitButton = new QPushButton(QString::fromUtf8(u8"退出"), this);

    connect(startButton, &QPushButton::clicked, this, &MainMenuView::startRunRequested);
    connect(quitButton, &QPushButton::clicked, qApp, &QApplication::quit);

    QVBoxLayout *layout = new QVBoxLayout(this);
    layout->setContentsMargins(80, 80, 80, 80);
    layout->setSpacing(18);
    layout->addStretch();
    layout->addWidget(titleLabel);
    layout->addWidget(startButton, 0, Qt::AlignHCenter);
    layout->addWidget(quitButton, 0, Qt::AlignHCenter);
    layout->addStretch();
}
