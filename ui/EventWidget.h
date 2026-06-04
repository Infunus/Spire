#ifndef EVENTWIDGET_H
#define EVENTWIDGET_H

#include "../core/EventLibrary.h"
#include "../core/GameText.h"

#include <QCoreApplication>
#include <QDir>
#include <QFileInfo>
#include <QFrame>
#include <QGraphicsDropShadowEffect>
#include <QHBoxLayout>
#include <QLabel>
#include <QLinearGradient>
#include <QPainter>
#include <QPaintEvent>
#include <QPixmap>
#include <QPushButton>
#include <QResizeEvent>
#include <QScrollArea>
#include <QScrollBar>
#include <QString>
#include <QStringList>
#include <QTimer>
#include <QVBoxLayout>
#include <QWidget>
#include <QtGlobal>

#include <functional>

class EventWidget : public QWidget
{
public:
    explicit EventWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setObjectName("EventWidget");
        setAttribute(Qt::WA_StyledBackground, true);

        QVBoxLayout *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(56, 42, 56, 46);
        rootLayout->setSpacing(18);

        QLabel *pageLabel = new QLabel(GameText::EventText::pageTitle(), this);
        pageLabel->setAlignment(Qt::AlignCenter);
        pageLabel->setStyleSheet(
            "color: rgba(255, 241, 204, 220);"
            "font-size: 18px;"
            "font-weight: 900;");

        QFrame *mainPanel = new QFrame(this);
        mainPanel->setObjectName("EventMainPanel");
        mainPanel->setStyleSheet(
            "QFrame#EventMainPanel {"
            "  background: rgba(6, 8, 16, 118);"
            "  border: 1px solid rgba(255, 217, 137, 125);"
            "  border-radius: 14px;"
            "}"
            "QLabel { color: #f7ead0; }");

        QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(mainPanel);
        shadow->setBlurRadius(30);
        shadow->setColor(QColor(0, 0, 0, 150));
        shadow->setOffset(0, 14);
        mainPanel->setGraphicsEffect(shadow);

        QHBoxLayout *mainLayout = new QHBoxLayout(mainPanel);
        mainLayout->setContentsMargins(34, 30, 34, 30);
        mainLayout->setSpacing(34);

        m_eventImageLabel = new QLabel(mainPanel);
        m_eventImageLabel->setAlignment(Qt::AlignCenter);
        m_eventImageLabel->setMinimumSize(360, 430);
        m_eventImageLabel->setStyleSheet(
            "background: rgba(255, 226, 168, 24);"
            "border: 2px solid rgba(255, 226, 168, 150);"
            "border-radius: 12px;"
            "color: rgba(255, 244, 214, 170);"
            "font-size: 24px;"
            "font-weight: 900;");

        QFrame *textPanel = new QFrame(mainPanel);
        textPanel->setObjectName("EventTextPanel");
        textPanel->setStyleSheet(
            "QFrame#EventTextPanel {"
            "  background: rgba(5, 7, 12, 168);"
            "  border: 1px solid rgba(255, 235, 177, 110);"
            "  border-radius: 10px;"
            "}"
            "QScrollArea { background: transparent; }");

        QVBoxLayout *textLayout = new QVBoxLayout(textPanel);
        textLayout->setContentsMargins(26, 24, 26, 22);
        textLayout->setSpacing(14);

        m_titleLabel = new QLabel(textPanel);
        m_titleLabel->setWordWrap(true);
        m_titleLabel->setStyleSheet(
            "color: #fff6dc;"
            "font-size: 30px;"
            "font-weight: 900;");

        m_bodyLabel = new QLabel(textPanel);
        m_bodyLabel->setWordWrap(true);
        m_bodyLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
        m_bodyLabel->setStyleSheet(
            "color: rgba(255, 247, 224, 225);"
            "font-size: 18px;"
            "font-weight: 650;");

        m_bodyScroll = new QScrollArea(textPanel);
        m_bodyScroll->setWidgetResizable(true);
        m_bodyScroll->setFrameShape(QFrame::NoFrame);
        m_bodyScroll->setStyleSheet(
            "QScrollArea { border: none; }"
            "QScrollBar:vertical {"
            "  background: rgba(255, 226, 168, 18);"
            "  width: 8px;"
            "  border-radius: 4px;"
            "}"
            "QScrollBar::handle:vertical {"
            "  background: rgba(255, 226, 168, 140);"
            "  border-radius: 4px;"
            "}"
            "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical { height: 0; }");

        QWidget *bodyPage = new QWidget(m_bodyScroll);
        QVBoxLayout *bodyLayout = new QVBoxLayout(bodyPage);
        bodyLayout->setContentsMargins(0, 0, 10, 0);
        bodyLayout->addWidget(m_bodyLabel);
        bodyLayout->addStretch();
        m_bodyScroll->setWidget(bodyPage);

        m_choicePanel = new QFrame(textPanel);
        m_choicePanel->setObjectName("ChoicePanel");
        m_choicePanel->setStyleSheet(
            "QFrame#ChoicePanel {"
            "  background: rgba(255, 226, 168, 18);"
            "  border: 1px solid rgba(255, 226, 168, 110);"
            "  border-radius: 8px;"
            "}");

        m_choiceLayout = new QVBoxLayout(m_choicePanel);
        m_choiceLayout->setContentsMargins(14, 12, 14, 12);
        m_choiceLayout->setSpacing(10);

        m_resultLabel = new QLabel(textPanel);
        m_resultLabel->setWordWrap(true);
        m_resultLabel->setStyleSheet(
            "color: rgba(255, 241, 204, 190);"
            "font-size: 14px;"
            "font-weight: 700;");

        m_continueButton = new QPushButton(GameText::EventText::continueButton(), textPanel);
        m_continueButton->setCursor(Qt::PointingHandCursor);
        m_continueButton->setMinimumHeight(44);
        m_continueButton->setMinimumWidth(150);
        m_continueButton->setStyleSheet(
            "QPushButton {"
            "  background: rgba(255, 226, 168, 42);"
            "  border: 1px solid rgba(255, 226, 168, 145);"
            "  border-radius: 6px;"
            "  color: #fff1cc;"
            "  font-size: 16px;"
            "  font-weight: 900;"
            "  padding: 8px 18px;"
            "}"
            "QPushButton:hover { background: rgba(235, 184, 88, 105); }"
            "QPushButton:disabled { color: rgba(255, 241, 204, 95); background: rgba(255, 226, 168, 18); }");

        textLayout->addWidget(m_titleLabel);
        textLayout->addWidget(m_bodyScroll, 1);
        textLayout->addWidget(m_choicePanel);
        textLayout->addWidget(m_resultLabel);
        textLayout->addWidget(m_continueButton, 0, Qt::AlignRight);

        mainLayout->addWidget(m_eventImageLabel, 3);
        mainLayout->addWidget(textPanel, 2);

        rootLayout->addWidget(pageLabel);
        rootLayout->addWidget(mainPanel, 1);

        connect(m_continueButton, &QPushButton::clicked, this, [this]() {
            advanceStory();
        });

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
        m_storyPages = m_event.storyPages;
        if (m_storyPages.isEmpty()) {
            m_storyPages << (m_event.body.isEmpty() ? GameText::EventText::defaultBody() : m_event.body);
        }

        m_currentPageIndex = 0;
        m_selectedChoiceIndex = -1;
        m_selectedChoice = RandomEventChoice();
        m_waitingForFinish = false;
        m_finished = false;

        m_titleLabel->setText(m_event.title.isEmpty() ? GameText::EventText::defaultTitle() : m_event.title);
        m_resultLabel->clear();
        m_continueButton->setEnabled(true);
        m_continueButton->setText(GameText::EventText::continueButton());
        m_continueButton->show();
        m_choicePanel->hide();

        updateStoryText();
        updateEventImage();
        rebuildChoices();

        if (m_storyPages.size() <= 1) {
            showChoicesOrFinish();
        }
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
            fallback.setColorAt(0.0, QColor(8, 12, 24));
            fallback.setColorAt(0.55, QColor(27, 23, 42));
            fallback.setColorAt(1.0, QColor(12, 30, 38));
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

        painter.fillRect(rect(), QColor(1, 3, 9, 42));
    }

    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        updateEventImage();
    }

private:
    void advanceStory()
    {
        if (m_finished) {
            return;
        }

        if (m_waitingForFinish) {
            finishEvent();
            return;
        }

        if (m_currentPageIndex + 1 < m_storyPages.size()) {
            ++m_currentPageIndex;
            updateStoryText();
        }

        if (m_currentPageIndex + 1 >= m_storyPages.size()) {
            showChoicesOrFinish();
        }
    }

    void showChoicesOrFinish()
    {
        if (m_event.choices.isEmpty()) {
            m_waitingForFinish = true;
            m_resultLabel->clear();
            m_continueButton->setText(GameText::EventText::finishEventButton());
            m_continueButton->show();
            return;
        }

        m_choicePanel->show();
        m_resultLabel->setText(GameText::EventText::eventChoiceHint());
        m_continueButton->hide();
    }

    void handleChoice(int index, const RandomEventChoice &choice)
    {
        m_selectedChoiceIndex = index;
        m_selectedChoice = choice;
        m_choicePanel->hide();

        const QString resultText = choice.resultText.isEmpty()
                                       ? GameText::EventText::resultFormat().arg(choice.text)
                                       : choice.resultText;
        if (!resultText.isEmpty()) {
            m_storyPages << resultText;
            m_currentPageIndex = m_storyPages.size() - 1;
            updateStoryText();
        }

        m_waitingForFinish = true;
        m_resultLabel->clear();
        m_continueButton->setText(GameText::EventText::finishEventButton());
        m_continueButton->show();
    }

    void finishEvent()
    {
        m_finished = true;
        m_waitingForFinish = false;
        m_continueButton->setEnabled(false);

        if (m_choiceHandler) {
            if (m_selectedChoiceIndex >= 0) {
                m_choiceHandler(m_selectedChoiceIndex, m_selectedChoice);
            } else {
                RandomEventChoice emptyChoice;
                emptyChoice.text = GameText::EventText::finishEventButton();
                m_choiceHandler(-1, emptyChoice);
            }
        }
    }

    void updateStoryText()
    {
        QStringList visiblePages;
        const int lastIndex = qMin(m_currentPageIndex, m_storyPages.size() - 1);
        for (int i = 0; i <= lastIndex; ++i) {
            visiblePages << m_storyPages.at(i);
        }

        m_bodyLabel->setText(visiblePages.join(QStringLiteral("\n\n")));

        QTimer::singleShot(0, this, [this]() {
            if (m_bodyScroll && m_bodyScroll->verticalScrollBar()) {
                QScrollBar *bar = m_bodyScroll->verticalScrollBar();
                bar->setValue(bar->maximum());
            }
        });
    }

    void updateEventImage()
    {
        const QPixmap eventImage(resolveAssetPath(m_event.imagePath));
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
                m_choicePanel);
            button->setCursor(Qt::PointingHandCursor);
            button->setMinimumHeight(56);
            button->setStyleSheet(
                "QPushButton {"
                "  background: rgba(255, 226, 168, 26);"
                "  border: 1px solid rgba(255, 226, 168, 125);"
                "  border-radius: 6px;"
                "  color: #fff1cc;"
                "  font-size: 16px;"
                "  font-weight: 850;"
                "  padding: 11px 14px;"
                "  text-align: left;"
                "}"
                "QPushButton:hover {"
                "  background: rgba(235, 184, 88, 96);"
                "  border-color: rgba(255, 235, 177, 220);"
                "}"
                "QPushButton:pressed { background: rgba(174, 119, 48, 130); }");

            connect(button, &QPushButton::clicked, this, [this, i, choice]() {
                handleChoice(i, choice);
            });
            m_choiceLayout->addWidget(button);
        }
    }

    QString resolveAssetPath(const QString &path) const
    {
        if (path.isEmpty() || QFileInfo(path).isAbsolute()) {
            return path;
        }

        QStringList candidates;
        auto addCandidatesFrom = [&candidates, &path](const QDir &startDir) {
            QDir dir(startDir);
            for (int i = 0; i < 8; ++i) {
                candidates << dir.filePath(path);
                candidates << dir.filePath(QStringLiteral("Spire/%1").arg(path));
                if (!dir.cdUp()) {
                    break;
                }
            }
        };

        addCandidatesFrom(QDir(QCoreApplication::applicationDirPath()));
        addCandidatesFrom(QDir::current());
        addCandidatesFrom(QDir(QFileInfo(QString::fromUtf8(__FILE__)).absolutePath()));

        for (const QString &candidate : candidates) {
            if (QFileInfo::exists(candidate)) {
                return candidate;
            }
        }
        return path;
    }

    RandomEventData m_event;
    QStringList m_storyPages;
    QPixmap m_background;
    QLabel *m_titleLabel = nullptr;
    QLabel *m_bodyLabel = nullptr;
    QLabel *m_eventImageLabel = nullptr;
    QScrollArea *m_bodyScroll = nullptr;
    QFrame *m_choicePanel = nullptr;
    QVBoxLayout *m_choiceLayout = nullptr;
    QLabel *m_resultLabel = nullptr;
    QPushButton *m_continueButton = nullptr;
    RandomEventChoice m_selectedChoice;
    int m_currentPageIndex = 0;
    int m_selectedChoiceIndex = -1;
    bool m_waitingForFinish = false;
    bool m_finished = false;
    std::function<void(int, const RandomEventChoice &)> m_choiceHandler;
};

#endif // EVENTWIDGET_H
