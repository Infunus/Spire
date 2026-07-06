#ifndef RESTWIDGET_H
#define RESTWIDGET_H

#include "../core/CardLibrary.h"
#include "../core/GameBalance.h"
#include "../core/GameState.h"

#include <QDialog>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QLayoutItem>
#include <QList>
#include <QPushButton>
#include <QScrollArea>
#include <QVBoxLayout>
#include <QWidget>

#include <functional>
#include <memory>

class RestWidget : public QWidget
{
public:
    explicit RestWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_messageLabel(new QLabel(this))
    {
        setObjectName("RestWidget");
        setStyleSheet(
            "#RestWidget { background: #111722; }"
            "QLabel { color: #f5ead2; }"
            "QFrame#RestPanel {"
            "  background: rgba(17, 22, 33, 218);"
            "  border: 1px solid rgba(185, 215, 255, 105);"
            "  border-radius: 8px;"
            "}"
            "QFrame#RestChoice {"
            "  background: rgba(255, 235, 190, 25);"
            "  border: 1px solid rgba(255, 226, 168, 95);"
            "  border-radius: 8px;"
            "}"
            "QPushButton {"
            "  color: #21160f;"
            "  background: #d7a84f;"
            "  border: 1px solid #ffe3a3;"
            "  border-radius: 7px;"
            "  padding: 8px 14px;"
            "  font-size: 15px;"
            "  font-weight: 900;"
            "}"
            "QPushButton:hover { background: #efc36b; }"
            "QPushButton:disabled { background: #4a4650; border-color: #6b6674; color: #aaa2b0; }");

        QFrame *panel = new QFrame(this);
        panel->setObjectName("RestPanel");
        panel->setMinimumWidth(760);

        QLabel *titleLabel = new QLabel(QStringLiteral("宿舍"), panel);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 36px; font-weight: 900; color: #fff6dc;");

        m_messageLabel->setAlignment(Qt::AlignCenter);
        m_messageLabel->setWordWrap(true);
        m_messageLabel->setStyleSheet("font-size: 17px; font-weight: 800; color: rgba(245,234,210,210);");
        setMessage(QStringLiteral("回到宿舍，决定今晚怎么安排。"));

        QHBoxLayout *choiceLayout = new QHBoxLayout;
        choiceLayout->setSpacing(14);
        choiceLayout->addWidget(createChoice(QStringLiteral("睡觉"),
                                             QStringLiteral("恢复 %1 点心情").arg(GameBalance::Rest::sleepHeal()),
                                             [this]() {
                                                 GameState::instance().heal(GameBalance::Rest::sleepHeal());
                                                 setMessage(QStringLiteral("睡醒了，心情恢复了 %1 点。")
                                                                .arg(GameBalance::Rest::sleepHeal()));
                                                 finishRest();
                                             }));
        choiceLayout->addWidget(createChoice(QStringLiteral("开卷！"),
                                             QStringLiteral("升级 %1 张卡牌").arg(GameBalance::Rest::upgradeCount()),
                                             [this]() {
                                                 openUpgradeDialog();
                                             },
                                             hasUpgradeableCards()));
        choiceLayout->addWidget(createChoice(QStringLiteral("玩原神"),
                                             QStringLiteral("删除一张卡牌"),
                                             [this]() {
                                                 openRemoveDialog();
                                             },
                                             !GameState::instance().ownedCards().isEmpty()));

        QVBoxLayout *panelLayout = new QVBoxLayout(panel);
        panelLayout->setContentsMargins(34, 30, 34, 34);
        panelLayout->setSpacing(18);
        panelLayout->addWidget(titleLabel);
        panelLayout->addWidget(m_messageLabel);
        panelLayout->addLayout(choiceLayout);

        QVBoxLayout *rootLayout = new QVBoxLayout(this);
        rootLayout->setContentsMargins(50, 44, 50, 44);
        rootLayout->addStretch();
        rootLayout->addWidget(panel, 0, Qt::AlignCenter);
        rootLayout->addStretch();
    }

    void setFinishHandler(const std::function<void()> &handler)
    {
        m_finishHandler = handler;
    }

private:
    QFrame *createChoice(const QString &title,
                         const QString &description,
                         const std::function<void()> &action,
                         bool enabled = true)
    {
        QFrame *choice = new QFrame(this);
        choice->setObjectName("RestChoice");
        choice->setMinimumSize(210, 190);

        QLabel *titleLabel = new QLabel(title, choice);
        titleLabel->setAlignment(Qt::AlignCenter);
        titleLabel->setStyleSheet("font-size: 24px; font-weight: 900; color: #fff5d8;");

        QLabel *descriptionLabel = new QLabel(description, choice);
        descriptionLabel->setAlignment(Qt::AlignCenter);
        descriptionLabel->setWordWrap(true);
        descriptionLabel->setStyleSheet("font-size: 15px; font-weight: 800; color: rgba(245,234,210,205);");

        QPushButton *button = new QPushButton(title, choice);
        button->setEnabled(enabled);
        connect(button, &QPushButton::clicked, this, [action]() {
            action();
        });

        QVBoxLayout *layout = new QVBoxLayout(choice);
        layout->setContentsMargins(16, 16, 16, 16);
        layout->setSpacing(12);
        layout->addWidget(titleLabel);
        layout->addWidget(descriptionLabel, 1);
        layout->addWidget(button);
        return choice;
    }

    bool hasUpgradeableCards() const
    {
        const QList<Card> cards = GameState::instance().ownedCards();
        for (int i = 0; i < cards.size(); ++i) {
            if (GameState::instance().canUpgradeCardAt(i)) {
                return true;
            }
        }
        return false;
    }

    QString cardLine(const Card &card) const
    {
        return QStringLiteral("%1\n费用 %2\n%3")
            .arg(card.displayName())
            .arg(card.cost())
            .arg(card.description());
    }

    void openUpgradeDialog()
    {
        if (!hasUpgradeableCards()) {
            setMessage(QStringLiteral("没有可以升级的卡牌。"));
            return;
        }

        QDialog *dialog = createCardDialog(QStringLiteral("开卷！选择要升级的卡牌"));
        QVBoxLayout *rootLayout = new QVBoxLayout(dialog);
        rootLayout->setContentsMargins(14, 14, 14, 14);
        rootLayout->setSpacing(10);

        QLabel *hintLabel = new QLabel(dialog);
        hintLabel->setAlignment(Qt::AlignCenter);
        hintLabel->setStyleSheet("color: #fff2d4; font-size: 16px; font-weight: 900;");
        rootLayout->addWidget(hintLabel);

        QScrollArea *scrollArea = new QScrollArea(dialog);
        scrollArea->setWidgetResizable(true);
        scrollArea->setFrameShape(QFrame::NoFrame);
        QWidget *gridHost = new QWidget(scrollArea);
        QGridLayout *grid = new QGridLayout(gridHost);
        grid->setContentsMargins(6, 6, 6, 6);
        grid->setSpacing(10);
        scrollArea->setWidget(gridHost);
        rootLayout->addWidget(scrollArea, 1);

        const std::shared_ptr<int> remaining = std::make_shared<int>(GameBalance::Rest::upgradeCount());
        const QList<Card> cards = GameState::instance().ownedCards();
        for (int i = 0; i < cards.size(); ++i) {
            const Card card = cards.at(i);
            QPushButton *button = createCardButton(card, dialog);
            button->setEnabled(GameState::instance().canUpgradeCardAt(i));
            connect(button, &QPushButton::clicked, this, [this, dialog, hintLabel, button, i, card, remaining]() {
                if (!GameState::instance().upgradeCardAt(i)) {
                    button->setEnabled(false);
                    return;
                }
                --(*remaining);
                button->setEnabled(false);
                button->setText(QStringLiteral("%1\n已升级").arg(card.displayName()));
                hintLabel->setText(QStringLiteral("还可以升级 %1 张卡牌").arg(qMax(0, *remaining)));

                if (*remaining <= 0 || !hasUpgradeableCards()) {
                    dialog->close();
                    setMessage(QStringLiteral("开卷完成，牌组变强了。"));
                    finishRest();
                }
            });
            grid->addWidget(button, i / 3, i % 3);
        }

        hintLabel->setText(QStringLiteral("还可以升级 %1 张卡牌").arg(*remaining));
        dialog->show();
    }

    void openRemoveDialog()
    {
        const QList<Card> cards = GameState::instance().ownedCards();
        if (cards.isEmpty()) {
            setMessage(QStringLiteral("牌组里没有可以删除的卡牌。"));
            return;
        }

        QDialog *dialog = createCardDialog(QStringLiteral("玩原神，选择要删除的卡牌"));
        QGridLayout *grid = new QGridLayout(dialog);
        grid->setContentsMargins(14, 14, 14, 14);
        grid->setSpacing(10);

        for (int i = 0; i < cards.size(); ++i) {
            const Card card = cards.at(i);
            QPushButton *button = createCardButton(card, dialog);
            connect(button, &QPushButton::clicked, this, [this, dialog, i, card]() {
                if (GameState::instance().removeCardAt(i)) {
                    dialog->close();
                    setMessage(QStringLiteral("摸了一晚鱼，忘掉了：%1。").arg(card.displayName()));
                    finishRest();
                }
            });
            grid->addWidget(button, i / 3, i % 3);
        }

        dialog->show();
    }

    QDialog *createCardDialog(const QString &title)
    {
        QDialog *dialog = new QDialog(this);
        dialog->setAttribute(Qt::WA_DeleteOnClose);
        dialog->setWindowTitle(title);
        dialog->resize(720, 500);
        dialog->setStyleSheet(
            "QDialog { background: #111722; }"
            "QPushButton {"
            "  background: #f0d18b;"
            "  border: 2px solid #6f4928;"
            "  border-radius: 10px;"
            "  color: #26180f;"
            "  padding: 9px;"
            "  font-size: 13px;"
            "  font-weight: 900;"
            "}"
            "QPushButton:hover { border-color: #ffd27a; background: #ffe3a3; }"
            "QPushButton:disabled { background: #4a4650; border-color: #6b6674; color: #aaa2b0; }");
        return dialog;
    }

    QPushButton *createCardButton(const Card &card, QWidget *parent)
    {
        QPushButton *button = new QPushButton(QStringLiteral("%1\n费用 %2\n%3")
                                                  .arg(card.displayName())
                                                  .arg(card.cost())
                                                  .arg(card.description()),
                                              parent);
        button->setMinimumSize(190, 112);
        button->setToolTip(cardLine(card));
        return button;
    }

    void setMessage(const QString &message)
    {
        m_messageLabel->setText(message);
    }

    void finishRest()
    {
        if (m_finishHandler) {
            m_finishHandler();
        }
    }

    QLabel *m_messageLabel;
    std::function<void()> m_finishHandler;
};

#endif // RESTWIDGET_H
