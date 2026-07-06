#include "BattleWidget.h"
#include "../core/CardLibrary.h"
#include "../core/GameBalance.h"
#include "../core/GameRandom.h"
#include "../core/GameState.h"
#include "../core/GameText.h"
#include "../core/Relic.h"

#include <QAbstractAnimation>
#include <QApplication>
#include <QCoreApplication>
#include <QDebug>
#include <QDialog>
#include <QDir>
#include <QEvent>
#include <QFileInfo>
#include <QFrame>
#include <QGraphicsOpacityEffect>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QImage>
#include <QLabel>
#include <QLayoutItem>
#include <QMouseEvent>
#include <QPainter>
#include <QPainterPath>
#include <QPixmap>
#include <QProgressBar>
#include <QPropertyAnimation>
#include <QResizeEvent>
#include <QSizePolicy>
#include <QStringList>
#include <QTextEdit>
#include <QTimer>
#include <QVBoxLayout>

namespace
{
QString findProjectAssetPath(const QString &relativePath)
{
    if (relativePath.isEmpty() || QFileInfo(relativePath).isAbsolute()) {
        return relativePath;
    }

    QStringList candidates;
    auto addCandidatesFrom = [&candidates, &relativePath](const QDir &startDir) {
        QDir dir(startDir);
        for (int i = 0; i < 8; ++i) {
            candidates << dir.filePath(relativePath);
            candidates << dir.filePath(QStringLiteral("Spire/%1").arg(relativePath));
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
    return relativePath;
}

QString battleStatusShortText(const QString &name)
{
    if (name == GameText::Battle::playerStrengthStatusName()) {
        return GameText::Battle::playerStrengthStatusShort();
    }
    if (name == GameText::Battle::playerVulnerableStatusName()) {
        return GameText::Battle::playerVulnerableStatusShort();
    }
    if (name == GameText::EnemyText::weakStatusName()) {
        return GameText::EnemyText::weakStatusShort();
    }
    if (name == GameText::EnemyText::vulnerableStatusName()) {
        return GameText::EnemyText::vulnerableStatusShort();
    }
    if (name == GameText::EnemyText::strengthStatusName()) {
        return GameText::EnemyText::strengthStatusShort();
    }
    return name.left(1);
}

QString battleStatusRulesText(const QString &name)
{
    if (name == GameText::Battle::playerStrengthStatusName()) {
        return GameText::Battle::playerStrengthStatusRules();
    }
    if (name == GameText::Battle::playerVulnerableStatusName()) {
        return GameText::Battle::playerVulnerableStatusRules();
    }
    if (name == GameText::EnemyText::weakStatusName()) {
        return GameText::EnemyText::weakStatusRules();
    }
    if (name == GameText::EnemyText::vulnerableStatusName()) {
        return GameText::EnemyText::vulnerableStatusRules();
    }
    if (name == GameText::EnemyText::strengthStatusName()) {
        return GameText::EnemyText::strengthStatusRules();
    }
    if (name == GameText::EnemyText::blockStatusName()) {
        return GameText::EnemyText::blockStatusRules();
    }
    if (name == GameText::Battle::playerBlockStatusName()) {
        return GameText::Battle::playerBlockStatusRules();
    }
    return QString();
}

QString battleStatusToolTip(const QString &name, int value)
{
    const QString rulesText = battleStatusRulesText(name);
    if (rulesText.isEmpty()) {
        return QStringLiteral("%1 x%2").arg(name).arg(value);
    }
    return GameText::Battle::statusTooltip(name, value, rulesText);
}

QString defaultCardImagePath(const Card &card)
{
    if (!card.imagePath().isEmpty()) {
        return card.imagePath();
    }

    // 旧牌组里可能已经创建了没有 imagePath 的 Card，这里按 id 补默认图，避免手牌空图。
    if (card.id() == CardIds::strike()) {
        return GameText::CardText::strikeImage();
    }
    if (card.id() == CardIds::defend()) {
        return GameText::CardText::defendImage();
    }
    if (card.id() == CardIds::bash()) {
        return GameText::CardText::bashImage();
    }
    if (card.id() == CardIds::inflame()) {
        return GameText::CardText::inflameImage();
    }
    if (card.id() == CardIds::heavyBlade()) {
        return GameText::CardText::heavyBladeImage();
    }
    if (card.id() == CardIds::pommel()) {
        return GameText::CardText::pommelImage();
    }
    if (card.id() == CardIds::shrug()) {
        return GameText::CardText::shrugImage();
    }
    if (card.id() == CardIds::anger()) {
        return GameText::CardText::angerImage();
    }
    if (card.id() == CardIds::flex()) {
        return GameText::CardText::flexImage();
    }
    if (card.id() == CardIds::cleave()) {
        return GameText::CardText::cleaveImage();
    }
    return QString();
}
}

class BattleCardButton : public QWidget
{
public:
    explicit BattleCardButton(const Card &card, QWidget *parent = nullptr)
        : QWidget(parent),
          m_card(card),
          m_artPath(defaultCardImagePath(card)),
          m_resolvedArtPath(findProjectAssetPath(m_artPath)),
          m_angle(0),
          m_art(m_resolvedArtPath),
          m_reportedMissingArt(false),
          m_artLabel(new QLabel(this))
    {
        setObjectName("BattleCardButton");
        setMouseTracking(true);
        setAttribute(Qt::WA_StyledBackground, false);
        setAutoFillBackground(false);
        m_artLabel->setAlignment(Qt::AlignCenter);
        m_artLabel->setAttribute(Qt::WA_TransparentForMouseEvents);
        m_artLabel->setStyleSheet("background: transparent; border: 0;");
        updateArtLabel();
    }

    void setPaintAngle(qreal value)
    {
        if (qFuzzyCompare(m_angle, value)) {
            return;
        }
        m_angle = value;
        update();
    }

protected:
    void resizeEvent(QResizeEvent *event) override
    {
        QWidget::resizeEvent(event);
        updateArtLabel();
    }

    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        const QRectF cardRect = rect().adjusted(4, 4, -4, -4);
        const bool active = isEnabled();
        const bool hovered = underMouse() && active;

        QPainterPath cardPath;
        cardPath.addRoundedRect(cardRect, 17, 17);

        QLinearGradient cardFill(cardRect.topLeft(), cardRect.bottomLeft());
        cardFill.setColorAt(0.0, active ? QColor(45, 107, 132) : QColor(87, 84, 78));
        cardFill.setColorAt(0.12, active ? QColor(245, 238, 206) : QColor(158, 151, 136));
        cardFill.setColorAt(0.23, active ? QColor(215, 176, 96) : QColor(126, 115, 96));
        cardFill.setColorAt(0.72, active ? QColor(116, 63, 34) : QColor(87, 76, 65));
        cardFill.setColorAt(1.0, active ? QColor(239, 204, 137) : QColor(130, 119, 100));
        painter.fillPath(cardPath, cardFill);

        painter.setPen(QPen(hovered ? QColor(70, 225, 235)
                                    : (active ? QColor(111, 73, 40) : QColor(78, 70, 61)),
                            hovered ? 5 : 4));
        painter.drawPath(cardPath);

        const QRectF titleRect(cardRect.left() + 25, cardRect.top() + 10, cardRect.width() - 36, 30);
        QPainterPath titlePath;
        titlePath.addRoundedRect(titleRect, 9, 9);
        painter.fillPath(titlePath, QColor(255, 238, 182, 224));
        painter.setPen(QPen(QColor(105, 63, 38), 2));
        painter.drawPath(titlePath);

        QFont titleFont = painter.font();
        titleFont.setPointSize(9);
        titleFont.setBold(true);
        painter.setFont(titleFont);
        painter.setPen(QColor(39, 27, 18));
        painter.drawText(titleRect.adjusted(4, 0, -4, 0),
                         Qt::AlignCenter | Qt::TextWordWrap,
                         m_card.displayName());

        const QPointF costCenter(cardRect.left() + 24, cardRect.top() + 24);
        QRadialGradient costFill(costCenter, 18);
        costFill.setColorAt(0.0, QColor(255, 251, 210));
        costFill.setColorAt(0.55, QColor(238, 122, 44));
        costFill.setColorAt(1.0, QColor(153, 39, 30));
        painter.setBrush(costFill);
        painter.setPen(QPen(QColor(255, 223, 103), 3));
        painter.drawEllipse(costCenter, 17, 17);

        QFont costFont = painter.font();
        costFont.setPointSize(12);
        costFont.setBold(true);
        painter.setFont(costFont);
        painter.setPen(Qt::white);
        painter.drawText(QRectF(costCenter.x() - 14, costCenter.y() - 13, 28, 26),
                         Qt::AlignCenter,
                         QString::number(m_card.cost()));

        const QRectF artRect(cardRect.left() + 13, cardRect.top() + 45, cardRect.width() - 26, cardRect.height() * 0.46);
        QPainterPath artPath;
        artPath.addRoundedRect(artRect, 10, 10);
        if (m_art.isNull()) {
            if (!m_reportedMissingArt && !m_artPath.isEmpty()) {
                qWarning() << "Failed to load card art" << m_card.id() << m_card.displayName()
                           << "from" << m_artPath << "resolved as" << m_resolvedArtPath;
                m_reportedMissingArt = true;
            }
            QLinearGradient fallback(artRect.topLeft(), artRect.bottomRight());
            fallback.setColorAt(0.0, QColor(87, 32, 35));
            fallback.setColorAt(1.0, QColor(31, 22, 24));
            painter.fillPath(artPath, fallback);
            painter.setPen(QColor(255, 232, 180));
            QFont missingFont = painter.font();
            missingFont.setPointSize(8);
            missingFont.setBold(true);
            painter.setFont(missingFont);
            painter.drawText(artRect.adjusted(6, 6, -6, -6),
                             Qt::AlignCenter | Qt::TextWordWrap,
                             m_artPath.isEmpty()
                                 ? QStringLiteral("缺图")
                                 : QStringLiteral("缺图\n%1").arg(QFileInfo(m_artPath).fileName()));
        }
        painter.setPen(QPen(QColor(91, 55, 37), 3));
        painter.drawPath(artPath);

        const QRectF textRect(cardRect.left() + 13,
                              artRect.bottom() + 8,
                              cardRect.width() - 26,
                              cardRect.bottom() - artRect.bottom() - 17);
        QPainterPath textPath;
        textPath.addRoundedRect(textRect, 9, 9);
        QLinearGradient textFill(textRect.topLeft(), textRect.bottomLeft());
        textFill.setColorAt(0.0, QColor(255, 241, 198, 232));
        textFill.setColorAt(1.0, QColor(227, 178, 104, 232));
        painter.fillPath(textPath, textFill);
        painter.setPen(QPen(QColor(111, 66, 39), 2));
        painter.drawPath(textPath);

        QFont descFont = painter.font();
        descFont.setPointSize(8);
        descFont.setBold(true);
        painter.setFont(descFont);
        painter.setPen(QColor(42, 26, 17));
        painter.drawText(textRect.adjusted(7, 5, -7, -5),
                         Qt::AlignCenter | Qt::TextWordWrap,
                         m_card.description());

        if (!active) {
            painter.fillPath(cardPath, QColor(45, 41, 38, 118));
        }
    }

private:
    QRect artLabelRect() const
    {
        const QRectF cardRect = rect().adjusted(4, 4, -4, -4);
        const QRectF artRect(cardRect.left() + 13,
                             cardRect.top() + 45,
                             cardRect.width() - 26,
                             cardRect.height() * 0.46);
        return artRect.toAlignedRect().adjusted(3, 3, -3, -3);
    }

    void updateArtLabel()
    {
        if (m_art.isNull() && !m_resolvedArtPath.isEmpty()) {
            m_art.load(m_resolvedArtPath);
        }
        if (m_art.isNull() || width() <= 0 || height() <= 0) {
            m_artLabel->hide();
            return;
        }

        const QRect target = artLabelRect();
        if (target.width() <= 0 || target.height() <= 0) {
            m_artLabel->hide();
            return;
        }

        const QPixmap source = QPixmap::fromImage(m_art);
        const QPixmap scaled = source.scaled(target.size(),
                                             Qt::KeepAspectRatioByExpanding,
                                             Qt::SmoothTransformation);
        const QRect sourceRect(qMax(0, (scaled.width() - target.width()) / 2),
                               qMax(0, (scaled.height() - target.height()) / 2),
                               target.width(),
                               target.height());

        QPixmap cropped(target.size());
        cropped.fill(Qt::transparent);
        QPainter cropPainter(&cropped);
        cropPainter.setRenderHint(QPainter::SmoothPixmapTransform);
        cropPainter.drawPixmap(cropped.rect(), scaled, sourceRect);
        cropPainter.end();

        m_artLabel->setGeometry(target);
        m_artLabel->setPixmap(cropped);
        m_artLabel->show();
        m_artLabel->raise();
    }

    Card m_card;
    QString m_artPath;
    QString m_resolvedArtPath;
    qreal m_angle;
    QImage m_art;
    mutable bool m_reportedMissingArt;
    QLabel *m_artLabel;
};

class PlayerPortraitWidget : public QWidget
{
public:
    explicit PlayerPortraitWidget(QWidget *parent = nullptr)
        : QWidget(parent)
    {
        setMinimumSize(210, 250);
        setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Fixed);
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        painter.setRenderHint(QPainter::SmoothPixmapTransform);

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(3, 5, 10, 92));
        painter.drawEllipse(QRectF(width() * 0.2, height() * 0.72, width() * 0.6, height() * 0.13));

        const QPointF c(width() * 0.5, height() * 0.43);
        painter.setPen(QPen(QColor(255, 229, 145, 90), 2));
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(c, width() * 0.29, width() * 0.29);
        painter.drawEllipse(c, width() * 0.22, width() * 0.22);

        painter.setPen(QPen(QColor(255, 226, 134, 120), 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawArc(QRectF(c.x() - width() * 0.34, c.y() - width() * 0.23,
                               width() * 0.68, width() * 0.46), 22 * 16, 138 * 16);
        painter.drawArc(QRectF(c.x() - width() * 0.33, c.y() - width() * 0.25,
                               width() * 0.66, width() * 0.5), 202 * 16, 118 * 16);

        QPainterPath shield;
        shield.moveTo(width() * 0.5, height() * 0.22);
        shield.cubicTo(width() * 0.67, height() * 0.27, width() * 0.72, height() * 0.39, width() * 0.69, height() * 0.56);
        shield.cubicTo(width() * 0.64, height() * 0.69, width() * 0.55, height() * 0.75, width() * 0.5, height() * 0.78);
        shield.cubicTo(width() * 0.45, height() * 0.75, width() * 0.36, height() * 0.69, width() * 0.31, height() * 0.56);
        shield.cubicTo(width() * 0.28, height() * 0.39, width() * 0.33, height() * 0.27, width() * 0.5, height() * 0.22);

        QLinearGradient shieldFill(QPointF(0, height() * 0.22), QPointF(0, height() * 0.78));
        shieldFill.setColorAt(0.0, QColor(122, 37, 45));
        shieldFill.setColorAt(0.55, QColor(32, 50, 83));
        shieldFill.setColorAt(1.0, QColor(17, 23, 39));
        painter.setPen(QPen(QColor(255, 230, 150), 4));
        painter.setBrush(shieldFill);
        painter.drawPath(shield);

        QPainterPath leftPage;
        leftPage.moveTo(width() * 0.35, height() * 0.49);
        leftPage.quadTo(width() * 0.43, height() * 0.44, width() * 0.5, height() * 0.51);
        leftPage.lineTo(width() * 0.5, height() * 0.67);
        leftPage.quadTo(width() * 0.42, height() * 0.61, width() * 0.35, height() * 0.66);
        leftPage.closeSubpath();
        QPainterPath rightPage;
        rightPage.moveTo(width() * 0.5, height() * 0.51);
        rightPage.quadTo(width() * 0.57, height() * 0.44, width() * 0.65, height() * 0.49);
        rightPage.lineTo(width() * 0.65, height() * 0.66);
        rightPage.quadTo(width() * 0.58, height() * 0.61, width() * 0.5, height() * 0.67);
        rightPage.closeSubpath();
        painter.setPen(QPen(QColor(89, 58, 42), 2));
        painter.setBrush(QColor(255, 239, 194));
        painter.drawPath(leftPage);
        painter.drawPath(rightPage);

        painter.setPen(QPen(QColor(255, 248, 215), 7, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPointF(width() * 0.58, height() * 0.25), QPointF(width() * 0.42, height() * 0.66));
        painter.setPen(QPen(QColor(255, 210, 83), 3, Qt::SolidLine, Qt::RoundCap));
        painter.drawLine(QPointF(width() * 0.59, height() * 0.24), QPointF(width() * 0.43, height() * 0.67));
        painter.setBrush(QColor(255, 245, 196));
        painter.setPen(QPen(QColor(255, 225, 129), 2));
        painter.drawPolygon(QPolygonF()
                            << QPointF(width() * 0.6, height() * 0.22)
                            << QPointF(width() * 0.64, height() * 0.27)
                            << QPointF(width() * 0.58, height() * 0.29));

        painter.setPen(Qt::NoPen);
        painter.setBrush(QColor(255, 228, 123));
        painter.drawEllipse(QPointF(width() * 0.5, height() * 0.18), 5, 5);
        painter.drawEllipse(QPointF(width() * 0.25, height() * 0.4), 3, 3);
        painter.drawEllipse(QPointF(width() * 0.76, height() * 0.52), 3, 3);

        painter.setPen(QPen(QColor(255, 247, 220, 210), 2));
        painter.drawText(QRectF(width() * 0.16, height() * 0.84, width() * 0.68, height() * 0.08),
                         Qt::AlignCenter,
                         GameText::Battle::playerName());
    }
};

class EnergyCrystalWidget : public QWidget
{
public:
    explicit EnergyCrystalWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_current(0),
          m_maximum(0)
    {
        setFixedHeight(38);
        setMinimumWidth(128);
    }

    void setEnergy(int current, int maximum)
    {
        m_current = current;
        m_maximum = maximum;
        update();
    }

protected:
    void paintEvent(QPaintEvent *event) override
    {
        Q_UNUSED(event);

        QPainter painter(this);
        painter.setRenderHint(QPainter::Antialiasing);
        const int count = qMax(1, m_maximum);
        const int crystalSize = 28;
        const int spacing = 7;
        const int totalWidth = count * crystalSize + (count - 1) * spacing;
        int x = (width() - totalWidth) / 2;
        const int y = (height() - crystalSize) / 2;

        for (int i = 0; i < count; ++i) {
            const bool full = i < m_current;
            QPolygonF crystal;
            crystal << QPointF(x + crystalSize / 2.0, y)
                    << QPointF(x + crystalSize, y + crystalSize / 2.0)
                    << QPointF(x + crystalSize / 2.0, y + crystalSize)
                    << QPointF(x, y + crystalSize / 2.0);

            QLinearGradient fill(QPointF(x, y), QPointF(x + crystalSize, y + crystalSize));
            if (full) {
                fill.setColorAt(0.0, QColor(147, 227, 255));
                fill.setColorAt(0.55, QColor(43, 121, 221));
                fill.setColorAt(1.0, QColor(18, 43, 128));
            } else {
                fill.setColorAt(0.0, QColor(83, 83, 92));
                fill.setColorAt(1.0, QColor(36, 36, 43));
            }
            painter.setBrush(fill);
            painter.setPen(QPen(full ? QColor(206, 246, 255) : QColor(113, 113, 124), 2));
            painter.drawPolygon(crystal);

            if (full) {
                painter.setPen(QPen(QColor(255, 255, 255, 180), 1));
                painter.drawLine(QPointF(x + 9, y + 8), QPointF(x + 14, y + 4));
            }
            x += crystalSize + spacing;
        }
    }

private:
    int m_current;
    int m_maximum;
};

class EnemyUnitWidget : public QWidget
{
public:
    explicit EnemyUnitWidget(QWidget *parent = nullptr)
        : QWidget(parent),
          m_portraitLabel(new QLabel(this)),
          m_blockBadge(new QLabel(this)),
          m_healthBar(new QProgressBar(this)),
          m_statusPanel(new QWidget(this)),
          m_statusLayout(new QHBoxLayout(m_statusPanel)),
          m_scoreLabel(new QLabel(this)),
          m_intentLabel(new QLabel(this))
    {
        setFixedSize(240, 320);
        setAttribute(Qt::WA_StyledBackground, true);
        setStyleSheet("EnemyUnitWidget { background: transparent; }");

        m_portraitLabel->setAlignment(Qt::AlignCenter);
        m_portraitLabel->setFixedSize(210, 160);
        m_portraitLabel->setStyleSheet(
            "QLabel {"
            "  background: transparent;"
            "  border: 0;"
            "  color: #fff3d0;"
            "  font-size: 22px;"
            "  font-weight: 900;"
            "  padding: 8px;"
            "}");

        m_blockBadge->setFixedSize(38, 38);
        m_blockBadge->setAlignment(Qt::AlignCenter);
        m_blockBadge->setStyleSheet(
            "QLabel { background: #2f7cc5; border: 3px solid #bfe5ff; border-radius: 19px;"
            "color: white; font-size: 15px; font-weight: 900; }");
        m_blockBadge->hide();

        m_healthBar->setFixedSize(194, 24);
        m_healthBar->setTextVisible(true);
        m_healthBar->setStyleSheet(
            "QProgressBar { background: rgba(34, 18, 16, 190); border: 2px solid rgba(255, 226, 168, 150);"
            "border-radius: 8px; color: #fff8de; font-size: 13px; font-weight: 900; text-align: center; }"
            "QProgressBar::chunk { background: #b62e2b; border-radius: 6px; }");

        m_statusLayout->setContentsMargins(0, 0, 0, 0);
        m_statusLayout->setSpacing(5);
        m_statusPanel->setStyleSheet("background: transparent;");

        m_scoreLabel->setAlignment(Qt::AlignCenter);
        m_scoreLabel->setFixedSize(200, 28);
        m_scoreLabel->setStyleSheet(
            "QLabel { background: rgba(74, 48, 18, 168);"
            "border: 1px solid rgba(255, 226, 168, 155); border-radius: 10px;"
            "color: #ffe3a0; font-size: 13px; font-weight: 900; padding: 4px; }");

        m_intentLabel->setAlignment(Qt::AlignCenter);
        m_intentLabel->setWordWrap(true);
        m_intentLabel->setFixedSize(200, 60);
        m_intentLabel->setStyleSheet(
            "QLabel { background: rgba(8, 12, 18, 132);"
            "border: 1px solid rgba(255, 226, 168, 110); border-radius: 12px; color: #fff3d0;"
            "font-size: 14px; font-weight: 900; padding: 7px; }");

        QVBoxLayout *root = new QVBoxLayout(this);
        root->setContentsMargins(0, 0, 0, 0);
        root->setSpacing(6);
        root->addWidget(m_portraitLabel, 0, Qt::AlignCenter);
        QHBoxLayout *hpLayout = new QHBoxLayout;
        hpLayout->setContentsMargins(0, 0, 0, 0);
        hpLayout->setSpacing(0);
        hpLayout->addWidget(m_blockBadge);
        hpLayout->addWidget(m_healthBar);
        root->addLayout(hpLayout);
        root->addWidget(m_statusPanel, 0, Qt::AlignCenter);
        root->addWidget(m_scoreLabel, 0, Qt::AlignCenter);
        root->addWidget(m_intentLabel, 0, Qt::AlignCenter);
    }

    QWidget *portraitAnchor() const { return m_portraitLabel; }

    void setEnemy(const Enemy &enemy, int scoreReward, int turnNumber, bool finalExam)
    {
        if (!enemy.imagePath().isEmpty()) {
            const QPixmap enemyPixmap(findProjectAssetPath(enemy.imagePath()));
            if (!enemyPixmap.isNull()) {
                m_portraitLabel->setText(QString());
                m_portraitLabel->setPixmap(enemyPixmap.scaled(m_portraitLabel->size(),
                                                              Qt::KeepAspectRatioByExpanding,
                                                              Qt::SmoothTransformation));
            } else {
                m_portraitLabel->setText(enemy.name());
            }
        } else {
            m_portraitLabel->setPixmap(QPixmap());
            m_portraitLabel->setText(enemy.name());
        }

        m_healthBar->setRange(0, enemy.maxHp());
        m_healthBar->setValue(qBound(0, enemy.hp(), enemy.maxHp()));
        m_healthBar->setFormat(GameText::Battle::enemyHpFormat().arg(qMax(0, enemy.hp())).arg(enemy.maxHp()));
        m_blockBadge->setText(QString::number(enemy.block()));
        m_blockBadge->setVisible(enemy.block() > 0);
        m_blockBadge->setToolTip(enemy.block() > 0
                                     ? battleStatusToolTip(GameText::EnemyText::blockStatusName(), enemy.block())
                                     : QString());
        m_intentLabel->setText(QStringLiteral("%1\n%2").arg(enemy.name(), enemy.intentText()));
        m_intentLabel->setToolTip(enemy.intentTooltip());
        m_scoreLabel->setText(finalExam
                                  ? GameText::Battle::finalExamScoreFormat().arg(scoreReward)
                                  : GameText::Battle::usualScoreRewardFormat().arg(scoreReward));
        m_scoreLabel->setToolTip(GameText::Battle::scoreRewardTooltipFormat().arg(qMax(1, turnNumber)));

        rebuildStatuses(enemy);
        setVisible(!enemy.isDead());
    }

private:
    void rebuildStatuses(const Enemy &enemy)
    {
        while (QLayoutItem *item = m_statusLayout->takeAt(0)) {
            delete item->widget();
            delete item;
        }

        QList<QPair<QString, int>> statuses;
        if (enemy.weakStacks() > 0) {
            statuses << qMakePair(GameText::EnemyText::weakStatusName(), enemy.weakStacks());
        }
        if (enemy.vulnerableStacks() > 0) {
            statuses << qMakePair(GameText::EnemyText::vulnerableStatusName(), enemy.vulnerableStacks());
        }
        if (enemy.strength() > 0) {
            statuses << qMakePair(GameText::EnemyText::strengthStatusName(), enemy.strength());
        }

        for (const QPair<QString, int> &status : statuses) {
            QLabel *icon = new QLabel(QStringLiteral("%1\n%2")
                                          .arg(battleStatusShortText(status.first))
                                          .arg(status.second),
                                      m_statusPanel);
            icon->setFixedSize(34, 34);
            icon->setAlignment(Qt::AlignCenter);
            icon->setToolTip(battleStatusToolTip(status.first, status.second));
            icon->setStyleSheet(
                "QLabel { background: #8f5bd1; border: 2px solid rgba(255,255,255,175);"
                "border-radius: 17px; color: white; font-size: 11px; font-weight: 900; padding: 0px; }");
            m_statusLayout->addWidget(icon);
        }
    }

    QLabel *m_portraitLabel;
    QLabel *m_blockBadge;
    QProgressBar *m_healthBar;
    QWidget *m_statusPanel;
    QHBoxLayout *m_statusLayout;
    QLabel *m_scoreLabel;
    QLabel *m_intentLabel;
};

BattleWidget::BattleWidget(QWidget *parent)
    : QWidget(parent),
      m_isBossBattle(false),
      m_titleLabel(new QLabel(this)),
      m_playerPortraitWidget(new PlayerPortraitWidget(this)),
      m_enemyBoard(new QWidget(this)),
      m_playerInfoLabel(new QLabel(this)),
      m_tipLabel(new QLabel(this)),
      m_playerBlockBadge(new QLabel(this)),
      m_playerHealthBar(new QProgressBar(this)),
      m_playerStatusPanel(new QWidget(this)),
      m_playerStatusLayout(new QHBoxLayout(m_playerStatusPanel)),
      m_energyCrystalWidget(new EnergyCrystalWidget(this)),
      m_handPanel(new QWidget(this)),
      m_endTurnButton(new QPushButton(GameText::Battle::endTurnButton(), this)),
      m_drawPileButton(new QPushButton(this)),
      m_discardPileButton(new QPushButton(this)),
      m_exhaustPileButton(new QPushButton(this)),
      m_playerHp(GameBalance::Player::startMaxHp()),
      m_playerMaxHp(GameBalance::Player::startMaxHp()),
      m_playerBlock(0),
      m_playerStrength(0),
      m_turnStartBlockBonus(0),
      m_energy(GameBalance::Battle::energyPerTurn()),
      m_maxEnergy(GameBalance::Battle::energyPerTurn()),
      m_turnNumber(1),
      m_battleNumber(1),
      m_enemiesDefeated(0),
      m_pendingUsualScoreReward(0),
      m_hoveredCardIndex(-1),
      m_dragCardIndex(-1),
      m_dragPotionIndex(-1),
      m_enemyTurnIndex(0),
      m_draggingCardEntity(false),
      m_draggingPotionEntity(false),
      m_dragPotionGhost(nullptr),
      m_battleEnded(false),
      m_playerTurn(true),
      m_runFinished(false),
      m_mapCallbackScheduled(false),
      m_backgroundPixmap(resolveAssetPath(GameText::Assets::battleBackground()))
{
    setObjectName("BattleWidget");
    setAttribute(Qt::WA_StyledBackground, true);
    setStyleSheet(
        "#BattleWidget {"
        "  background: #10141d;"
        "}"
        "QLabel { color: #f5ead2; }"
        "QPushButton {"
        "  color: #fff7de;"
        "  background: #8c4b22;"
        "  border: 2px solid #f4c86b;"
        "  border-radius: 10px;"
        "  padding: 8px 12px;"
        "  font-size: 14px;"
        "  font-weight: 800;"
        "}"
        "QPushButton:hover { background: #ad642b; border-color: #ffe0a1; }"
        "QPushButton:disabled { background: #3b201c; border-color: #7f5f47; color: #9d8b7a; }");

    QVBoxLayout *rootLayout = new QVBoxLayout(this);
    rootLayout->setContentsMargins(14, 10, 14, 8);
    rootLayout->setSpacing(6);
    rootLayout->addWidget(createControlStrip());
    rootLayout->addWidget(createArenaPanel(), 1);

    m_handPanel->setMinimumHeight(290);
    m_handPanel->setAttribute(Qt::WA_StyledBackground, true);
    m_handPanel->setStyleSheet(
        "QWidget {"
        "  background: transparent;"
        "  border: 0;"
        "}");
    rootLayout->addWidget(m_handPanel);

    connect(m_endTurnButton, &QPushButton::clicked, this, &BattleWidget::endTurn);
    connect(m_drawPileButton, &QPushButton::clicked, this, [this]() {
        showPileDialog(PileKind::Draw);
    });
    connect(m_discardPileButton, &QPushButton::clicked, this, [this]() {
        showPileDialog(PileKind::Discard);
    });
    connect(m_exhaustPileButton, &QPushButton::clicked, this, [this]() {
        showPileDialog(PileKind::Exhaust);
    });

    setupInitialDemoText();
}

QWidget *BattleWidget::createArenaPanel()
{
    QFrame *panel = new QFrame(this);
    panel->setStyleSheet(
        "QFrame { background: transparent; border: 0; }");

    m_titleLabel->setAlignment(Qt::AlignCenter);
    m_titleLabel->setStyleSheet(
        "background: rgba(7, 11, 17, 96);"
        "border: 0;"
        "border-radius: 10px;"
        "font-size: 22px;"
        "font-weight: 900;"
        "padding: 5px 16px;");

    const QString healthStyle =
        "QProgressBar {"
        "  background: rgba(34, 18, 16, 190);"
        "  border: 2px solid rgba(255, 226, 168, 150);"
        "  border-radius: 8px;"
        "  color: #fff8de;"
        "  font-size: 13px;"
        "  font-weight: 900;"
        "  text-align: center;"
        "}"
        "QProgressBar::chunk { background: #b62e2b; border-radius: 6px; }";
    m_playerHealthBar->setFixedSize(214, 24);
    m_playerHealthBar->setTextVisible(true);
    m_playerHealthBar->setStyleSheet(healthStyle);

    auto setupBlockBadge = [](QLabel *badge) {
        badge->setFixedSize(42, 42);
        badge->setAlignment(Qt::AlignCenter);
        badge->setStyleSheet(
            "QLabel {"
            "  background: #2f7cc5;"
            "  border: 3px solid #bfe5ff;"
            "  border-radius: 21px;"
            "  color: white;"
            "  font-size: 16px;"
            "  font-weight: 900;"
            "}");
        badge->hide();
    };
    setupBlockBadge(m_playerBlockBadge);

    m_playerStatusLayout->setContentsMargins(0, 0, 0, 0);
    m_playerStatusLayout->setSpacing(5);
    m_playerStatusPanel->setStyleSheet("background: transparent;");

    m_tipLabel->setAlignment(Qt::AlignCenter);
    m_tipLabel->setWordWrap(true);
    m_tipLabel->setMinimumSize(360, 126);
    m_tipLabel->setStyleSheet(
        "QLabel {"
        "  background: transparent;"
        "  border: 0;"
        "  padding: 8px 12px;"
        "  color: rgba(255, 244, 215, 180);"
        "  font-size: 16px;"
        "  font-weight: 800;"
        "}");

    m_enemyBoard->setMinimumSize(560, 330);
    m_enemyBoard->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    m_enemyBoard->setStyleSheet("background: transparent;");

    QWidget *playerPanel = new QWidget(panel);
    QVBoxLayout *playerLayout = new QVBoxLayout(playerPanel);
    playerLayout->setContentsMargins(0, 0, 0, 0);
    playerLayout->setSpacing(8);
    playerLayout->addStretch();
    playerLayout->addWidget(m_playerPortraitWidget, 0, Qt::AlignCenter);
    QHBoxLayout *playerHpLayout = new QHBoxLayout;
    playerHpLayout->setContentsMargins(0, 0, 0, 0);
    playerHpLayout->setSpacing(0);
    playerHpLayout->addWidget(m_playerBlockBadge);
    playerHpLayout->addWidget(m_playerHealthBar);
    playerLayout->addLayout(playerHpLayout);
    playerLayout->addWidget(m_playerStatusPanel, 0, Qt::AlignCenter);
    playerLayout->addStretch();

    QWidget *middlePanel = new QWidget(panel);
    QVBoxLayout *middleLayout = new QVBoxLayout(middlePanel);
    middleLayout->setContentsMargins(0, 0, 0, 0);
    middleLayout->setSpacing(10);
    middleLayout->addStretch();
    middleLayout->addWidget(m_tipLabel, 0, Qt::AlignCenter);
    middleLayout->addStretch();

    QGridLayout *arenaLayout = new QGridLayout(panel);
    arenaLayout->setContentsMargins(24, 0, 24, 2);
    arenaLayout->setHorizontalSpacing(22);
    arenaLayout->setVerticalSpacing(0);
    arenaLayout->setColumnStretch(0, 2);
    arenaLayout->setColumnStretch(1, 2);
    arenaLayout->setColumnStretch(2, 3);
    arenaLayout->addWidget(playerPanel, 0, 0);
    arenaLayout->addWidget(middlePanel, 0, 1);
    arenaLayout->addWidget(m_enemyBoard, 0, 2, Qt::AlignCenter);
    return panel;
}

QWidget *BattleWidget::createControlStrip()
{
    QFrame *strip = new QFrame(this);
    strip->setFixedHeight(64);
    strip->setStyleSheet(
        "QFrame { background: rgba(5, 10, 16, 150); border: 1px solid rgba(255, 226, 168, 70); border-radius: 10px; }"
        "QPushButton { border-radius: 8px; padding: 6px 10px; font-size: 13px; }");

    m_playerInfoLabel->setAlignment(Qt::AlignCenter);
    m_playerInfoLabel->setStyleSheet(
        "QLabel {"
        "  background: rgba(26, 21, 18, 120);"
        "  border: 1px solid rgba(255, 226, 168, 95);"
        "  border-radius: 8px;"
        "  color: #fff3d4;"
        "  padding: 7px 12px;"
        "  font-size: 14px;"
        "  font-weight: 900;"
        "}");

    QHBoxLayout *layout = new QHBoxLayout(strip);
    layout->setContentsMargins(12, 7, 12, 7);
    layout->setSpacing(8);
    m_titleLabel->setMinimumWidth(130);
    m_titleLabel->setMaximumWidth(180);
    layout->addWidget(m_titleLabel);
    layout->addWidget(m_playerInfoLabel, 1);
    layout->addWidget(m_energyCrystalWidget);

    for (int i = 0; i < 5; ++i) {
        QLabel *slot = new QLabel(strip);
        slot->setFixedSize(58, 32);
        slot->setAlignment(Qt::AlignCenter);
        slot->setWordWrap(true);
        slot->setStyleSheet(
            "background: rgba(245, 218, 164, 42);"
            "border: 1px solid rgba(255, 226, 168, 115);"
            "border-radius: 7px;"
            "color: #fff3d4;"
            "font-size: 10px;"
            "font-weight: 900;"
            "padding: 2px;");
        m_relicLabels.append(slot);
        layout->addWidget(slot);
    }

    QLabel *potionTitle = new QLabel(GameText::Battle::potionTitle(), strip);
    potionTitle->setStyleSheet("font-size: 13px; font-weight: 900; color: rgba(255, 243, 212, 180);");
    layout->addWidget(potionTitle);
    for (int i = 0; i < GameState::instance().maxPotions(); ++i) {
        QPushButton *potionButton = new QPushButton(strip);
        potionButton->setCursor(Qt::PointingHandCursor);
        potionButton->setFixedSize(82, 32);
        potionButton->setProperty("potionIndex", i);
        potionButton->installEventFilter(this);
        m_potionButtons.append(potionButton);
        layout->addWidget(potionButton);
    }

    m_endTurnButton->setFixedSize(90, 34);
    m_drawPileButton->setFixedHeight(30);
    m_discardPileButton->setFixedHeight(30);
    m_exhaustPileButton->setFixedHeight(30);
    layout->addSpacing(8);
    layout->addWidget(m_endTurnButton);
    layout->addSpacing(8);
    layout->addWidget(m_drawPileButton);
    layout->addWidget(m_discardPileButton);
    layout->addWidget(m_exhaustPileButton);
    return strip;
}

void BattleWidget::setupInitialDemoText()
{
    applyNormalMessageStyle();
    m_titleLabel->setText(GameText::App::title());
    m_tipLabel->setText(GameText::Battle::handTipText());
    refreshUi();
}

void BattleWidget::applyNormalMessageStyle()
{
    m_titleLabel->setStyleSheet(
        "background: rgba(7, 11, 17, 96);"
        "border: 0;"
        "border-radius: 10px;"
        "color: #f5ead2;"
        "font-size: 22px;"
        "font-weight: 900;"
        "padding: 5px 16px;");

    m_tipLabel->setStyleSheet(
        "QLabel {"
        "  background: transparent;"
        "  border: 0;"
        "  padding: 8px 12px;"
        "  color: rgba(255, 244, 215, 180);"
        "  font-size: 16px;"
        "  font-weight: 800;"
        "}");
}

void BattleWidget::applyBattleEndMessageStyle()
{
    m_titleLabel->setStyleSheet(
        "background: rgba(255, 241, 204, 232);"
        "border: 2px solid rgba(83, 50, 27, 150);"
        "border-radius: 10px;"
        "color: #24160d;"
        "font-size: 22px;"
        "font-weight: 900;"
        "padding: 5px 16px;");

    m_tipLabel->setStyleSheet(
        "QLabel {"
        "  background: rgba(255, 241, 204, 226);"
        "  border: 2px solid rgba(83, 50, 27, 150);"
        "  border-radius: 12px;"
        "  padding: 10px 14px;"
        "  color: #24160d;"
        "  font-size: 16px;"
        "  font-weight: 900;"
        "}");
}

void BattleWidget::setBossBattle(bool isBossBattle)
{
    m_isBossBattle = isBossBattle;
}

void BattleWidget::startDebugBattle(bool bossBattle)
{
    m_finishCallback = nullptr;
    GameState::instance().resetForNewRun();
    m_cardLibrary = createDefaultCards();
    GameState::instance().setOwnedCards(m_cardLibrary);
    m_playerMaxHp = GameBalance::Player::startMaxHp();
    m_playerHp = m_playerMaxHp;
    m_maxEnergy = GameBalance::Battle::energyPerTurn();
    m_battleNumber = bossBattle ? GameBalance::Battle::bossBattleNumber() : 1;
    m_enemiesDefeated = bossBattle ? GameBalance::Battle::bossBattleNumber() - 1 : 0;
    m_runFinished = false;
    startBattle();
}

void BattleWidget::startMapBattle(bool bossBattle, const std::function<void(bool)> &finishCallback)
{
    m_finishCallback = finishCallback;
    m_cardLibrary = GameState::instance().ownedCards();
    if (m_cardLibrary.isEmpty()) {
        m_cardLibrary = createDefaultCards();
        GameState::instance().setOwnedCards(m_cardLibrary);
    }
    m_playerMaxHp = GameState::instance().maxHp();
    m_playerHp = GameState::instance().hp();
    m_maxEnergy = GameBalance::Battle::energyPerTurn();
    m_battleNumber = bossBattle ? GameBalance::Battle::bossBattleNumber() : 1;
    m_enemiesDefeated = GameState::instance().defeatedEnemies();
    m_runFinished = false;
    startBattle();
}

void BattleWidget::startRun()
{
    GameState::instance().resetForNewRun();
    m_cardLibrary = createDefaultCards();
    GameState::instance().setOwnedCards(m_cardLibrary);
    m_playerMaxHp = GameState::instance().maxHp();
    m_playerHp = GameState::instance().hp();
    m_maxEnergy = GameBalance::Battle::energyPerTurn();
    m_battleNumber = 1;
    m_enemiesDefeated = 0;
    m_runFinished = false;
    startBattle();
}

void BattleWidget::startBattle()
{
    applyNormalMessageStyle();
    m_hand.setDeck(m_cardLibrary);
    m_exhaustPile.clear();
    m_enemies = createEnemiesForBattle();
    m_enemyScoreCollected.clear();
    for (int i = 0; i < m_enemies.size(); ++i) {
        m_enemyScoreCollected.append(false);
    }
    m_pendingUsualScoreReward = 0;
    rebuildEnemyWidgets();
    m_playerBlock = 0;
    m_playerStrength = 0;
    m_turnStartBlockBonus = GameState::instance().consumeNextBattleTurnBlock();
    m_energy = m_maxEnergy;
    m_turnNumber = 0;
    m_hoveredCardIndex = -1;
    m_dragCardIndex = -1;
    clearPotionDrag();
    m_enemyTurnIndex = 0;
    m_draggingCardEntity = false;
    m_battleEnded = false;
    m_playerTurn = true;
    m_mapCallbackScheduled = false;

    setBossBattle(m_battleNumber >= GameBalance::Battle::bossBattleNumber());
    m_titleLabel->setText(GameText::Battle::battleTitleFormat().arg(m_battleNumber));
    beginPlayerTurn();
    applyRelicsAtBattleStart();
    m_playerStrength += GameState::instance().consumeNextBattleStartStrength();
    refreshUi();
}

void BattleWidget::beginPlayerTurn()
{
    ++m_turnNumber;
    m_playerBlock = 0;
    if (m_turnStartBlockBonus > 0) {
        m_playerBlock += m_turnStartBlockBonus;
    }
    applyRelicsAtTurnStart();
    m_energy = m_maxEnergy;
    m_playerTurn = true;
    drawCards(GameBalance::Battle::cardsDrawnEachTurn());
    refreshUi();
}

void BattleWidget::drawCards(int count)
{
    m_hand.drawCards(count);
    rebuildHandButtons();
}

void BattleWidget::rebuildHandButtons()
{
    qDeleteAll(m_handButtons);
    m_handButtons.clear();

    for (int i = 0; i < m_hand.size(); ++i) {
        const Card card = m_hand.cardAt(i);
        BattleCardButton *button = new BattleCardButton(card, m_handPanel);
        button->setProperty("cardIndex", i);
        button->setCursor(Qt::PointingHandCursor);
        button->setToolTip(cardLine(card));
        button->setEnabled(m_playerTurn && !m_battleEnded && m_energy >= card.cost());
        button->installEventFilter(this);
        button->show();
        button->raise();
        m_handButtons.append(button);
    }

    layoutHandButtons(true);
    refreshActionButtons();
}

void BattleWidget::layoutHandButtons(bool animate)
{
    const int count = m_handButtons.size();
    if (count == 0 || !m_handPanel) {
        return;
    }

    const int layoutCount = m_draggingCardEntity ? qMax(0, count - 1) : count;
    if (layoutCount == 0) {
        return;
    }

    const int panelWidth = qMax(720, m_handPanel->width());
    const int panelHeight = qMax(270, m_handPanel->height());
    const int cardWidth = 160;
    const int cardHeight = 210;
    const int spread = qMin(116, qMax(72, panelWidth / qMax(6, layoutCount + 3)));
    const int centerX = panelWidth / 2;
    const int baseY = panelHeight - cardHeight - 18;

    int visibleIndex = 0;
    for (int i = 0; i < count; ++i) {
        QWidget *button = m_handButtons.at(i);
        if (!button) {
            continue;
        }
        if (m_draggingCardEntity && i == m_dragCardIndex) {
            continue;
        }
        if (button->parentWidget() != m_handPanel) {
            button->setParent(m_handPanel);
            button->show();
        }

        const double offset = visibleIndex - (layoutCount - 1) / 2.0;
        ++visibleIndex;
        const bool hovered = (i == m_hoveredCardIndex && !m_draggingCardEntity);
        const int curveLift = static_cast<int>(offset * offset * 6.0);
        const QSize size = hovered
                               ? QSize(static_cast<int>(cardWidth * 1.16), static_cast<int>(cardHeight * 1.16))
                               : QSize(cardWidth, cardHeight);
        const int x = centerX + static_cast<int>(offset * spread) - size.width() / 2;
        const int y = baseY + curveLift - (hovered ? 42 : 0);
        const QRect target(QPoint(x, y), size);

        if (BattleCardButton *cardButton = dynamic_cast<BattleCardButton *>(button)) {
            const qreal angle = hovered ? 0.0 : qBound(-8.0, offset * 3.0, 8.0);
            cardButton->setPaintAngle(angle);
        }

        if (animate) {
            QPropertyAnimation *move = new QPropertyAnimation(button, "geometry", button);
            move->setDuration(130);
            move->setStartValue(button->geometry().isValid() ? button->geometry() : target);
            move->setEndValue(target);
            move->start(QAbstractAnimation::DeleteWhenStopped);
        } else {
            button->setGeometry(target);
        }

        if (!hovered) {
            button->raise();
        }
    }

    if (m_hoveredCardIndex >= 0 && m_hoveredCardIndex < m_handButtons.size()) {
        m_handButtons.at(m_hoveredCardIndex)->raise();
    }
}

bool BattleWidget::eventFilter(QObject *watched, QEvent *event)
{
    QWidget *watchedWidget = qobject_cast<QWidget *>(watched);
    if (!watchedWidget) {
        return QWidget::eventFilter(watched, event);
    }

    if (watchedWidget->property("potionIndex").isValid()) {
        return handlePotionEvent(watchedWidget, event);
    }

    QWidget *cardWidget = watchedWidget;
    if (!cardWidget->property("cardIndex").isValid()) {
        return QWidget::eventFilter(watched, event);
    }

    const int handIndex = cardWidget->property("cardIndex").toInt();
    if (handIndex < 0 || handIndex >= m_hand.size()) {
        return QWidget::eventFilter(watched, event);
    }

    if (event->type() == QEvent::Enter && !m_draggingCardEntity && m_dragCardIndex < 0) {
        m_hoveredCardIndex = handIndex;
        layoutHandButtons(true);
        return false;
    }

    if (event->type() == QEvent::Leave && m_hoveredCardIndex == handIndex && m_dragCardIndex < 0) {
        m_hoveredCardIndex = -1;
        layoutHandButtons(true);
        return false;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && cardWidget->isEnabled()) {
            m_dragCardIndex = handIndex;
            m_hoveredCardIndex = -1;
            m_dragStartPos = mouseEvent->globalPosition().toPoint();
            m_draggingCardEntity = false;
            cardWidget->grabMouse();
            return true;
        }
    }

    if (event->type() == QEvent::MouseMove && m_dragCardIndex == handIndex) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if ((mouseEvent->globalPosition().toPoint() - m_dragStartPos).manhattanLength() >= QApplication::startDragDistance()) {
            if (!m_draggingCardEntity) {
                const QRect globalRect(cardWidget->mapToGlobal(QPoint(0, 0)), cardWidget->size());
                cardWidget->setParent(this);
                cardWidget->setGeometry(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
                if (BattleCardButton *battleCard = dynamic_cast<BattleCardButton *>(cardWidget)) {
                    battleCard->setPaintAngle(0);
                }
                cardWidget->show();
                cardWidget->raise();
                m_draggingCardEntity = true;
                layoutHandButtons(true);
            }
            cardWidget->move(mapFromGlobal(mouseEvent->globalPosition().toPoint())
                             - QPoint(cardWidget->width() / 2, cardWidget->height() / 2));
            cardWidget->raise();
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && m_dragCardIndex == handIndex) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        cardWidget->releaseMouse();

        if (m_draggingCardEntity) {
            const Card card = m_hand.cardAt(handIndex);
            const bool targetsEnemy = card.damage() > 0 || card.weak() > 0 || card.vulnerable() > 0;
            const QPoint globalPoint = mouseEvent->globalPosition().toPoint();
            const int targetEnemyIndex = enemyIndexAtGlobalPoint(globalPoint);

            if (!targetsEnemy || targetEnemyIndex >= 0) {
                playCard(handIndex, targetEnemyIndex);
            } else {
                cardWidget->setParent(m_handPanel);
                cardWidget->show();
                m_draggingCardEntity = false;
                m_dragCardIndex = -1;
                layoutHandButtons(true);
            }
        } else {
            m_dragCardIndex = -1;
        }
        return true;
    }

    return QWidget::eventFilter(watched, event);
}

void BattleWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);

    QPainter painter(this);
    painter.setRenderHint(QPainter::SmoothPixmapTransform, true);

    if (m_backgroundPixmap.isNull()) {
        QLinearGradient fallback(rect().topLeft(), rect().bottomRight());
        fallback.setColorAt(0.0, QColor(11, 18, 28));
        fallback.setColorAt(0.52, QColor(40, 28, 38));
        fallback.setColorAt(1.0, QColor(8, 9, 14));
        painter.fillRect(rect(), fallback);
    } else {
        const QSize sourceSize = m_backgroundPixmap.size();
        const double scale = qMax(width() / double(sourceSize.width()),
                                  height() / double(sourceSize.height()));
        const QSize targetSize(sourceSize.width() * scale, sourceSize.height() * scale);
        const QPoint topLeft((width() - targetSize.width()) / 2,
                             (height() - targetSize.height()) / 2);
        painter.drawPixmap(QRect(topLeft, targetSize), m_backgroundPixmap);
    }

    QLinearGradient shade(rect().topLeft(), rect().bottomLeft());
    shade.setColorAt(0.0, QColor(2, 6, 12, 100));
    shade.setColorAt(0.45, QColor(2, 6, 12, 12));
    shade.setColorAt(0.78, QColor(2, 5, 10, 40));
    shade.setColorAt(1.0, QColor(2, 5, 10, 150));
    painter.fillRect(rect(), shade);
}

bool BattleWidget::handlePotionEvent(QWidget *potionWidget, QEvent *event)
{
    if (!potionWidget || !potionWidget->property("potionIndex").isValid()) {
        return false;
    }

    const int potionIndex = potionWidget->property("potionIndex").toInt();
    const QList<PotionData> potions = GameState::instance().potions();
    if (potionIndex < 0 || potionIndex >= potions.size()) {
        return false;
    }

    if (event->type() == QEvent::MouseButtonPress) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if (mouseEvent->button() == Qt::LeftButton && potionWidget->isEnabled()) {
            m_dragPotionIndex = potionIndex;
            m_potionDragStartPos = mouseEvent->globalPosition().toPoint();
            m_draggingPotionEntity = false;
            potionWidget->grabMouse();
            return true;
        }
    }

    if (event->type() == QEvent::MouseMove && m_dragPotionIndex == potionIndex) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        if ((mouseEvent->globalPosition().toPoint() - m_potionDragStartPos).manhattanLength()
            >= QApplication::startDragDistance()) {
            if (!m_dragPotionGhost) {
                const QRect globalRect(potionWidget->mapToGlobal(QPoint(0, 0)), potionWidget->size());
                QPushButton *potionButton = qobject_cast<QPushButton *>(potionWidget);
                const QString ghostText = potionButton ? potionButton->text() : potions.at(potionIndex).name;
                m_dragPotionGhost = new QLabel(ghostText, this);
                m_dragPotionGhost->setAlignment(Qt::AlignCenter);
                m_dragPotionGhost->setAttribute(Qt::WA_TransparentForMouseEvents);
                m_dragPotionGhost->setStyleSheet(
                    "QLabel { background: rgba(61, 105, 126, 235);"
                    "border: 2px solid rgba(194, 237, 255, 220); border-radius: 10px;"
                    "color: #effcff; font-size: 12px; font-weight: 900; padding: 6px; }");
                m_dragPotionGhost->setGeometry(QRect(mapFromGlobal(globalRect.topLeft()), globalRect.size()));
                m_dragPotionGhost->show();
                m_dragPotionGhost->raise();
            }

            m_draggingPotionEntity = true;
            if (m_dragPotionGhost) {
                m_dragPotionGhost->move(mapFromGlobal(mouseEvent->globalPosition().toPoint())
                                        - QPoint(m_dragPotionGhost->width() / 2,
                                                 m_dragPotionGhost->height() / 2));
                m_dragPotionGhost->raise();
            }
            return true;
        }
    }

    if (event->type() == QEvent::MouseButtonRelease && m_dragPotionIndex == potionIndex) {
        QMouseEvent *mouseEvent = static_cast<QMouseEvent *>(event);
        potionWidget->releaseMouse();

        if (m_draggingPotionEntity) {
            const PotionData potion = potions.at(potionIndex);
            const int targetEnemyIndex = enemyIndexAtGlobalPoint(mouseEvent->globalPosition().toPoint());
            if (!potionTargetsEnemy(potion) || targetEnemyIndex >= 0) {
                usePotionAt(potionIndex, targetEnemyIndex);
            }
        }

        clearPotionDrag();
        return true;
    }

    return false;
}

void BattleWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    layoutHandButtons(false);
    layoutEnemyWidgets();
}

void BattleWidget::refreshUi()
{
    m_playerHealthBar->setRange(0, m_playerMaxHp);
    m_playerHealthBar->setValue(qBound(0, m_playerHp, m_playerMaxHp));
    m_playerHealthBar->setFormat(GameText::Battle::playerHpFormat().arg(qMax(0, m_playerHp)).arg(m_playerMaxHp));

    m_playerBlockBadge->setText(QString::number(m_playerBlock));
    m_playerBlockBadge->setVisible(m_playerBlock > 0);
    m_playerBlockBadge->setToolTip(m_playerBlock > 0
                                       ? battleStatusToolTip(GameText::Battle::playerBlockStatusName(), m_playerBlock)
                                       : QString());
    m_energyCrystalWidget->setEnergy(m_energy, m_maxEnergy);

    QList<QPair<QString, int>> playerStatuses;
    if (m_playerStrength > 0) {
        playerStatuses << qMakePair(GameText::Battle::playerStrengthStatusName(), m_playerStrength);
    }
    rebuildStatusIcons(m_playerStatusLayout, playerStatuses);

    for (int i = 0; i < m_enemyWidgets.size() && i < m_enemies.size(); ++i) {
        const bool finalExam = m_battleNumber >= GameBalance::Battle::bossBattleNumber();
        const int scoreReward = finalExam ? currentFinalExamScore()
                                          : currentUsualScoreRewardForEnemy(m_enemies.at(i));
        m_enemyWidgets.at(i)->setEnemy(m_enemies.at(i), scoreReward, m_turnNumber, finalExam);
    }
    layoutEnemyWidgets();

    m_playerInfoLabel->setText(GameText::Battle::playerInfoFormat()
                                   .arg(m_turnNumber)
                                   .arg(m_energy)
                                   .arg(m_maxEnergy)
                                   .arg(m_playerStrength)
                                   .arg(GameState::instance().usualScore()));

    if (!m_battleEnded) {
        m_tipLabel->setText(GameText::Battle::handTipText());
    }

    refreshRelicSlots();
    refreshPotionSlots();
    refreshPileButtons();
    refreshActionButtons();
}

void BattleWidget::rebuildEnemyWidgets()
{
    qDeleteAll(m_enemyWidgets);
    m_enemyWidgets.clear();

    for (int i = 0; i < m_enemies.size(); ++i) {
        EnemyUnitWidget *enemyWidget = new EnemyUnitWidget(m_enemyBoard);
        enemyWidget->setProperty("enemyIndex", i);
        const bool finalExam = m_battleNumber >= GameBalance::Battle::bossBattleNumber();
        const int scoreReward = finalExam ? currentFinalExamScore()
                                          : currentUsualScoreRewardForEnemy(m_enemies.at(i));
        enemyWidget->setEnemy(m_enemies.at(i), scoreReward, m_turnNumber, finalExam);
        enemyWidget->show();
        m_enemyWidgets.append(enemyWidget);
    }

    layoutEnemyWidgets();
}

void BattleWidget::layoutEnemyWidgets()
{
    if (!m_enemyBoard || m_enemyWidgets.isEmpty()) {
        return;
    }

    int aliveCount = 0;
    for (const Enemy &enemy : m_enemies) {
        if (!enemy.isDead()) {
            ++aliveCount;
        }
    }
    if (aliveCount <= 0) {
        for (EnemyUnitWidget *enemyWidget : m_enemyWidgets) {
            enemyWidget->hide();
        }
        return;
    }

    const QSize unitSize(240, 320);
    const int centerX = m_enemyBoard->width() / 2;
    const int centerY = m_enemyBoard->height() / 2;
    const int horizontalStep = aliveCount <= 2 ? 222 : 184;
    const int depthStep = 20;
    int aliveIndex = 0;

    for (int i = 0; i < m_enemyWidgets.size() && i < m_enemies.size(); ++i) {
        EnemyUnitWidget *enemyWidget = m_enemyWidgets.at(i);
        if (!enemyWidget || m_enemies.at(i).isDead()) {
            if (enemyWidget) {
                enemyWidget->hide();
            }
            continue;
        }

        const double offset = aliveIndex - (aliveCount - 1) / 2.0;
        const int x = centerX + static_cast<int>(offset * horizontalStep) - unitSize.width() / 2;
        const int y = centerY - unitSize.height() / 2 + static_cast<int>(offset * depthStep);
        enemyWidget->setGeometry(QRect(QPoint(x, y), unitSize));
        enemyWidget->show();
        enemyWidget->raise();
        ++aliveIndex;
    }
}

int BattleWidget::firstAliveEnemyIndex() const
{
    for (int i = 0; i < m_enemies.size(); ++i) {
        if (!m_enemies.at(i).isDead()) {
            return i;
        }
    }
    return -1;
}

int BattleWidget::enemyIndexAtGlobalPoint(const QPoint &globalPoint) const
{
    for (int i = m_enemyWidgets.size() - 1; i >= 0; --i) {
        EnemyUnitWidget *enemyWidget = m_enemyWidgets.at(i);
        if (!enemyWidget || i >= m_enemies.size() || m_enemies.at(i).isDead()) {
            continue;
        }
        if (enemyWidget->rect().contains(enemyWidget->mapFromGlobal(globalPoint))) {
            return i;
        }
    }
    return -1;
}

bool BattleWidget::allEnemiesDead() const
{
    if (m_enemies.isEmpty()) {
        return false;
    }
    for (const Enemy &enemy : m_enemies) {
        if (!enemy.isDead()) {
            return false;
        }
    }
    return true;
}

void BattleWidget::refreshActionButtons()
{
    const bool canAct = m_playerTurn && !m_battleEnded;

    if (m_endTurnButton) {
        if (m_battleEnded) {
            if (m_finishCallback) {
                m_endTurnButton->setText(GameText::Battle::returnToMapButton());
                m_endTurnButton->setEnabled(!m_mapCallbackScheduled);
            } else if (m_playerHp <= 0 || m_runFinished) {
                m_endTurnButton->setText(GameText::Battle::restartButton());
                m_endTurnButton->setEnabled(true);
            } else {
                m_endTurnButton->setText(GameText::Battle::nextBattleButton());
                m_endTurnButton->setEnabled(true);
            }
        } else {
            m_endTurnButton->setText(GameText::Battle::endTurnButton());
            m_endTurnButton->setEnabled(canAct);
        }
    }

}

void BattleWidget::refreshRelicSlots()
{
    const QList<RelicData> relics = GameState::instance().relics();
    for (int i = 0; i < m_relicLabels.size(); ++i) {
        QLabel *slot = m_relicLabels.at(i);
        slot->clear();
        slot->setToolTip(QString());
        if (i >= relics.size()) {
            continue;
        }

        const RelicData relic = relics.at(i);
        slot->setToolTip(relic.name + QStringLiteral("\n") + relic.description);
        if (!relic.imagePath.isEmpty()) {
            const QPixmap relicImage(relic.imagePath);
            if (!relicImage.isNull()) {
                slot->setPixmap(relicImage.scaled(slot->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
                continue;
            }
        }
        slot->setText(relic.name);
    }
}

void BattleWidget::refreshPotionSlots()
{
    const QList<PotionData> potions = GameState::instance().potions();
    for (int i = 0; i < m_potionButtons.size(); ++i) {
        QPushButton *button = m_potionButtons.at(i);
        if (i >= potions.size()) {
            button->setText(GameText::Battle::potionEmptyText());
            button->setToolTip(QString());
            button->setEnabled(false);
            button->setStyleSheet("background: rgba(35, 35, 38, 120); border: 1px solid rgba(180, 180, 190, 80); border-radius: 8px; color: rgba(240, 230, 210, 110); font-size: 12px; font-weight: 800;");
            continue;
        }

        const PotionData potion = potions.at(i);
        button->setText(potion.name);
        button->setToolTip(GameText::Battle::potionUseTooltip(potion.description));
        button->setEnabled(!m_battleEnded && m_playerTurn);
        button->setStyleSheet("background: rgba(45, 76, 92, 205); border: 1px solid rgba(178, 225, 240, 150); border-radius: 8px; color: #effcff; font-size: 12px; font-weight: 800;");
    }
}

void BattleWidget::refreshPileButtons()
{
    m_drawPileButton->setText(GameText::Battle::drawPileButtonFormat().arg(m_hand.drawPileSize()));
    m_discardPileButton->setText(GameText::Battle::discardPileButtonFormat().arg(m_hand.discardPileSize()));
    m_exhaustPileButton->setText(GameText::Battle::exhaustPileButtonFormat().arg(m_exhaustPile.size()));
}

void BattleWidget::rebuildStatusIcons(QHBoxLayout *layout, const QList<QPair<QString, int>> &statuses)
{
    clearStatusLayout(layout);
    for (const QPair<QString, int> &status : statuses) {
        QLabel *icon = new QLabel(QStringLiteral("%1\n%2")
                                      .arg(battleStatusShortText(status.first))
                                      .arg(status.second),
                                  layout->parentWidget());
        icon->setFixedSize(34, 34);
        icon->setAlignment(Qt::AlignCenter);
        icon->setToolTip(battleStatusToolTip(status.first, status.second));
        icon->setStyleSheet(
            "QLabel {"
            "  background: #8f5bd1;"
            "  border: 2px solid rgba(255,255,255,175);"
            "  border-radius: 17px;"
            "  color: white;"
            "  font-size: 11px;"
            "  font-weight: 900;"
            "  padding: 0px;"
            "}");
        layout->addWidget(icon);
    }
}

void BattleWidget::clearStatusLayout(QHBoxLayout *layout)
{
    while (QLayoutItem *item = layout->takeAt(0)) {
        delete item->widget();
        delete item;
    }
}

void BattleWidget::collectEnemyScoreIfDefeated(int enemyIndex)
{
    if (m_battleNumber >= GameBalance::Battle::bossBattleNumber()
        || enemyIndex < 0
        || enemyIndex >= m_enemies.size()) {
        return;
    }

    while (m_enemyScoreCollected.size() < m_enemies.size()) {
        m_enemyScoreCollected.append(false);
    }

    if (!m_enemies.at(enemyIndex).isDead() || m_enemyScoreCollected.at(enemyIndex)) {
        return;
    }

    m_enemyScoreCollected[enemyIndex] = true;
    m_pendingUsualScoreReward += currentUsualScoreRewardForEnemy(m_enemies.at(enemyIndex));
}

void BattleWidget::applyRelicsAtBattleStart()
{
    const QList<RelicData> relics = GameState::instance().relics();
    for (const RelicData &relic : relics) {
        if (relic.effectType == RelicEffectType::StartCombatBlock) {
            m_playerBlock += relic.amount;
        } else if (relic.effectType == RelicEffectType::StartCombatStrength) {
            m_playerStrength += relic.amount;
        } else if (relic.effectType == RelicEffectType::StartCombatDraw) {
            drawCards(relic.amount);
        }
    }
}

void BattleWidget::applyRelicsAtTurnStart()
{
    const QList<RelicData> relics = GameState::instance().relics();
    for (const RelicData &relic : relics) {
        if (relic.effectType == RelicEffectType::TurnStartBlock) {
            m_playerBlock += relic.amount;
        }
    }
}

void BattleWidget::applyRelicsAfterBattleWin()
{
    const QList<RelicData> relics = GameState::instance().relics();
    for (const RelicData &relic : relics) {
        if (relic.effectType == RelicEffectType::EndCombatHeal) {
            m_playerHp = qMin(m_playerMaxHp, m_playerHp + relic.amount);
        }
    }
    GameState::instance().setHp(m_playerHp);
}

bool BattleWidget::potionTargetsEnemy(const PotionData &potion) const
{
    return potion.effectType == PotionEffectType::DamageEnemy;
}

void BattleWidget::clearPotionDrag()
{
    if (m_dragPotionGhost) {
        m_dragPotionGhost->deleteLater();
        m_dragPotionGhost = nullptr;
    }
    m_dragPotionIndex = -1;
    m_draggingPotionEntity = false;
}

void BattleWidget::usePotionAt(int potionIndex, int targetEnemyIndex)
{
    if (m_battleEnded || !m_playerTurn) {
        return;
    }

    const QList<PotionData> potions = GameState::instance().potions();
    if (potionIndex < 0 || potionIndex >= potions.size()) {
        return;
    }

    const PotionData potion = potions.at(potionIndex);
    if (potion.effectType == PotionEffectType::DamageEnemy) {
        if (targetEnemyIndex < 0) {
            targetEnemyIndex = firstAliveEnemyIndex();
        }
        if (targetEnemyIndex < 0 || targetEnemyIndex >= m_enemies.size()) {
            return;
        }
        Enemy &enemy = m_enemies[targetEnemyIndex];
        const int shownDamage = enemy.previewIncomingDamage(potion.amount);
        enemy.takeDamage(potion.amount);
        collectEnemyScoreIfDefeated(targetEnemyIndex);
        if (targetEnemyIndex < m_enemyWidgets.size()) {
            showSlashEffect(m_enemyWidgets.at(targetEnemyIndex)->portraitAnchor());
            showDamagePopup(m_enemyWidgets.at(targetEnemyIndex)->portraitAnchor(), shownDamage);
        }
    } else if (potion.effectType == PotionEffectType::GainBlock) {
        m_playerBlock += potion.amount;
    } else if (potion.effectType == PotionEffectType::Heal) {
        m_playerHp = qMin(m_playerMaxHp, m_playerHp + potion.amount);
        GameState::instance().setHp(m_playerHp);
    }

    GameState::instance().removePotionAt(potionIndex);
    if (!checkBattleEnd()) {
        refreshUi();
    }
}

void BattleWidget::playCard(int handIndex, int targetEnemyIndex)
{
    if (m_battleEnded || !m_playerTurn || handIndex < 0 || handIndex >= m_hand.size()) {
        return;
    }

    const Card card = m_hand.cardAt(handIndex);
    const bool targetsEnemy = card.damage() > 0 || card.weak() > 0 || card.vulnerable() > 0;
    if (targetsEnemy) {
        if (targetEnemyIndex < 0) {
            targetEnemyIndex = firstAliveEnemyIndex();
        }
        if (targetEnemyIndex < 0 || targetEnemyIndex >= m_enemies.size() || m_enemies.at(targetEnemyIndex).isDead()) {
            m_dragCardIndex = -1;
            m_draggingCardEntity = false;
            rebuildHandButtons();
            refreshUi();
            return;
        }
    }

    if (m_energy < card.cost()) {
        m_dragCardIndex = -1;
        m_draggingCardEntity = false;
        refreshUi();
        rebuildHandButtons();
        return;
    }

    m_energy -= card.cost();

    if (card.damage() > 0) {
        const int damage = playerAttackDamage(card);
        Enemy &enemy = m_enemies[targetEnemyIndex];
        const int shownDamage = enemy.previewIncomingDamage(damage);
        enemy.takeDamage(damage);
        collectEnemyScoreIfDefeated(targetEnemyIndex);
        if (targetEnemyIndex < m_enemyWidgets.size()) {
            showSlashEffect(m_enemyWidgets.at(targetEnemyIndex)->portraitAnchor());
            showDamagePopup(m_enemyWidgets.at(targetEnemyIndex)->portraitAnchor(), shownDamage);
        }
    }
    if (card.block() > 0) {
        m_playerBlock += card.block();
    }
    if (card.heal() > 0) {
        m_playerHp = qMin(m_playerMaxHp, m_playerHp + card.heal());
    }
    if (card.selfDamage() > 0) {
        m_playerHp -= card.selfDamage();
        showImpactFlash(m_playerPortraitWidget);
        showDamagePopup(m_playerPortraitWidget, card.selfDamage());
    }
    if (card.strengthGain() > 0) {
        m_playerStrength += card.strengthGain();
    }
    if (card.weak() > 0) {
        m_enemies[targetEnemyIndex].applyWeak(card.weak());
    }
    if (card.vulnerable() > 0) {
        m_enemies[targetEnemyIndex].applyVulnerable(card.vulnerable());
    }

    animateCardToPile(handIndex, card.exhaust() ? PileKind::Exhaust : PileKind::Discard);
    const Card playedCard = m_hand.removeCardFromHand(handIndex);
    if (card.exhaust()) {
        m_exhaustPile.append(playedCard);
    } else {
        m_hand.addToDiscard(playedCard);
    }

    if (card.draw() > 0) {
        m_hand.drawCards(card.draw());
    }

    m_hoveredCardIndex = -1;
    m_dragCardIndex = -1;
    clearPotionDrag();
    m_draggingCardEntity = false;
    rebuildHandButtons();

    if (!checkBattleEnd()) {
        refreshUi();
    }
}

void BattleWidget::animateCardToPile(int handIndex, PileKind pileKind)
{
    if (handIndex < 0 || handIndex >= m_handButtons.size()) {
        return;
    }

    QWidget *sourceWidget = m_handButtons.at(handIndex);
    if (!sourceWidget) {
        return;
    }

    QPushButton *targetButton = m_discardPileButton;
    if (pileKind == PileKind::Draw) {
        targetButton = m_drawPileButton;
    } else if (pileKind == PileKind::Exhaust) {
        targetButton = m_exhaustPileButton;
    }

    const QRect sourceRect(mapFromGlobal(sourceWidget->mapToGlobal(QPoint(0, 0))), sourceWidget->size());
    const QPoint targetCenter = mapFromGlobal(targetButton->mapToGlobal(QPoint(targetButton->width() / 2,
                                                                               targetButton->height() / 2)));
    const QSize flySize(84, 110);
    const QRect targetRect(targetCenter - QPoint(flySize.width() / 2, flySize.height() / 2), flySize);

    QLabel *flyingCard = new QLabel(m_hand.cardAt(handIndex).buttonText(), this);
    flyingCard->setAlignment(Qt::AlignCenter);
    flyingCard->setWordWrap(true);
    flyingCard->setStyleSheet(
        "QLabel {"
        "  background: rgba(255, 247, 223, 230);"
        "  border: 2px solid #ffd27a;"
        "  border-radius: 10px;"
        "  color: #2b2118;"
        "  font-size: 10px;"
        "  font-weight: 900;"
        "  padding: 6px;"
        "}");
    flyingCard->setGeometry(sourceRect);
    flyingCard->show();
    flyingCard->raise();

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(flyingCard);
    flyingCard->setGraphicsEffect(effect);

    QPropertyAnimation *move = new QPropertyAnimation(flyingCard, "geometry", flyingCard);
    move->setDuration(260);
    move->setStartValue(sourceRect);
    move->setEndValue(targetRect);
    move->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *fade = new QPropertyAnimation(effect, "opacity", flyingCard);
    fade->setDuration(260);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, flyingCard, &QLabel::deleteLater);
    fade->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattleWidget::endTurn()
{
    if (m_battleEnded) {
        if (m_finishCallback) {
            scheduleMapCallback(m_playerHp > 0);
            return;
        }
        if (m_playerHp <= 0 || m_runFinished) {
            startRun();
        } else {
            startBattle();
        }
        return;
    }

    if (!m_playerTurn) {
        return;
    }

    m_playerTurn = false;
    m_hoveredCardIndex = -1;
    m_dragCardIndex = -1;
    m_draggingCardEntity = false;
    m_enemyTurnIndex = 0;
    m_hand.discardHand();
    rebuildHandButtons();
    refreshUi();
    finishEnemyTurn();
}

void BattleWidget::finishEnemyTurn()
{
    if (m_battleEnded) {
        return;
    }

    runNextEnemyAction();
}

void BattleWidget::runNextEnemyAction()
{
    if (m_battleEnded) {
        return;
    }

    while (m_enemyTurnIndex < m_enemies.size() && m_enemies.at(m_enemyTurnIndex).isDead()) {
        ++m_enemyTurnIndex;
    }

    if (m_enemyTurnIndex >= m_enemies.size()) {
        if (!checkBattleEnd()) {
            beginPlayerTurn();
        }
        return;
    }

    const int actingEnemyIndex = m_enemyTurnIndex;
    ++m_enemyTurnIndex;
    playEnemyAttackAnimation(actingEnemyIndex);
}

void BattleWidget::playEnemyAttackAnimation(int enemyIndex)
{
    if (enemyIndex < 0 || enemyIndex >= m_enemyWidgets.size() || enemyIndex >= m_enemies.size()) {
        runNextEnemyAction();
        return;
    }

    if (!m_enemies.at(enemyIndex).currentActionDealsDamage()) {
        QTimer::singleShot(GameBalance::Battle::enemyActionPauseMs(), this, [this, enemyIndex]() {
            applyEnemyAction(enemyIndex);
            if (!checkBattleEnd()) {
                QTimer::singleShot(GameBalance::Battle::enemyActionPauseMs(),
                                   this,
                                   &BattleWidget::runNextEnemyAction);
            }
        });
        return;
    }

    QWidget *enemyAnchor = m_enemyWidgets.at(enemyIndex)->portraitAnchor();
    if (!enemyAnchor || !m_playerPortraitWidget) {
        applyEnemyAction(enemyIndex);
        QTimer::singleShot(GameBalance::Battle::enemyActionPauseMs(), this, &BattleWidget::runNextEnemyAction);
        return;
    }

    const QPoint start = mapFromGlobal(enemyAnchor->mapToGlobal(QPoint(enemyAnchor->width() / 2,
                                                                       enemyAnchor->height() / 2)));
    const QPoint end = mapFromGlobal(m_playerPortraitWidget->mapToGlobal(QPoint(m_playerPortraitWidget->width() / 2,
                                                                                m_playerPortraitWidget->height() / 2)));

    QLabel *orb = new QLabel(this);
    orb->setFixedSize(34, 34);
    orb->setAttribute(Qt::WA_TransparentForMouseEvents);
    orb->setStyleSheet(
        "QLabel {"
        "  background: qradialgradient(cx:0.42, cy:0.38, radius:0.68,"
        "                              stop:0 #fffbd2, stop:0.35 #ffd36a, stop:1 #ff6f37);"
        "  border: 2px solid rgba(255, 245, 190, 210);"
        "  border-radius: 17px;"
        "}");
    orb->move(start - QPoint(17, 17));
    orb->show();
    orb->raise();

    QPropertyAnimation *move = new QPropertyAnimation(orb, "pos", orb);
    move->setDuration(GameBalance::Battle::enemyProjectileMs());
    move->setStartValue(start - QPoint(17, 17));
    move->setEndValue(end - QPoint(17, 17));
    connect(move, &QPropertyAnimation::finished, this, [this, orb, enemyIndex]() {
        orb->deleteLater();
        applyEnemyAction(enemyIndex);
        if (!checkBattleEnd()) {
            QTimer::singleShot(GameBalance::Battle::enemyActionPauseMs(), this, &BattleWidget::runNextEnemyAction);
        }
    });
    move->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattleWidget::applyEnemyAction(int enemyIndex)
{
    if (enemyIndex < 0 || enemyIndex >= m_enemies.size()) {
        return;
    }

    const int enemyDamage = m_enemies[enemyIndex].attackPlayer();
    const int blockedDamage = qMin(m_playerBlock, enemyDamage);
    const int realDamage = enemyDamage - blockedDamage;
    m_playerBlock -= blockedDamage;
    m_playerHp -= realDamage;
    GameState::instance().setHp(m_playerHp);
    if (realDamage > 0) {
        showImpactFlash(m_playerPortraitWidget);
        showDamagePopup(m_playerPortraitWidget, realDamage);
    }

    refreshUi();
}

void BattleWidget::showPileDialog(PileKind pileKind)
{
    QList<Card> cards;
    QString title;
    if (pileKind == PileKind::Draw) {
        cards = m_hand.drawPileCards();
        title = GameText::Battle::drawPileButtonFormat().arg(m_hand.drawPileSize());
    } else if (pileKind == PileKind::Discard) {
        cards = m_hand.discardPileCards();
        title = GameText::Battle::discardPileButtonFormat().arg(m_hand.discardPileSize());
    } else {
        cards = m_exhaustPile;
        title = GameText::Battle::exhaustPileButtonFormat().arg(m_exhaustPile.size());
    }

    QDialog *dialog = new QDialog(this);
    dialog->setAttribute(Qt::WA_DeleteOnClose);
    dialog->setWindowTitle(title);
    dialog->resize(460, 380);
    dialog->setStyleSheet(
        "QDialog { background: #171114; }"
        "QTextEdit { background: #241b1d; color: #f7ead0; border: 1px solid rgba(255,226,168,100); font-size: 14px; }");

    QTextEdit *textEdit = new QTextEdit(dialog);
    textEdit->setReadOnly(true);
    textEdit->setText(cardListText(cards));

    QVBoxLayout *layout = new QVBoxLayout(dialog);
    layout->setContentsMargins(12, 12, 12, 12);
    layout->addWidget(textEdit);
    dialog->show();
}

QString BattleWidget::cardListText(const QList<Card> &cards) const
{
    if (cards.isEmpty()) {
        return GameText::Battle::emptyPileText();
    }

    QStringList lines;
    for (const Card &card : cards) {
        lines << cardLine(card);
    }
    return lines.join(QStringLiteral("\n\n"));
}

QString BattleWidget::cardLine(const Card &card) const
{
    const QString exhaustText = card.exhaust()
                                    ? QStringLiteral(" [%1]").arg(GameText::Battle::exhaustTag())
                                    : QString();
    return QStringLiteral("%1%2\n费用 %3\n%4")
        .arg(card.displayName(), exhaustText)
        .arg(card.cost())
        .arg(card.description());
}

QString BattleWidget::resolveAssetPath(const QString &relativePath) const
{
    if (relativePath.isEmpty() || QFileInfo(relativePath).isAbsolute()) {
        return relativePath;
    }

    QStringList candidates;
    auto addCandidatesFrom = [&candidates, &relativePath](const QDir &startDir) {
        QDir dir(startDir);
        for (int i = 0; i < 8; ++i) {
            candidates << dir.filePath(relativePath);
            candidates << dir.filePath(QStringLiteral("Spire/%1").arg(relativePath));
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
    return relativePath;
}

bool BattleWidget::checkBattleEnd()
{
    if (m_battleEnded) {
        return true;
    }

    if (allEnemiesDead()) {
        for (int i = 0; i < m_enemies.size(); ++i) {
            collectEnemyScoreIfDefeated(i);
        }
        const int defeatedEnemyCount = qMax(1, m_enemies.size());
        const int usualScoreReward = m_pendingUsualScoreReward > 0
                                         ? m_pendingUsualScoreReward
                                         : currentBattleUsualScoreReward();
        const int finalExamScore = currentFinalExamScore();
        m_battleEnded = true;
        m_playerTurn = false;
        m_enemiesDefeated += defeatedEnemyCount;
        applyRelicsAfterBattleWin();
        m_hand.discardHand();
        rebuildHandButtons();
        applyBattleEndMessageStyle();

        if (m_battleNumber >= GameBalance::Battle::bossBattleNumber()) {
            m_runFinished = true;
            GameState::instance().recordBossDefeated(finalExamScore);
            m_titleLabel->setText(GameText::Battle::runClearTitleFormat()
                                      .arg(GameState::instance().totalScore()));
            m_tipLabel->setText(GameText::Battle::finalVictoryTipFormat()
                                    .arg(GameState::instance().finalExamScore())
                                    .arg(GameState::instance().usualScore())
                                    .arg(GameState::instance().totalScore()));
        } else {
            ++m_battleNumber;
            GameState::instance().recordEnemyDefeated(usualScoreReward,
                                                      defeatedEnemyCount);
            m_titleLabel->setText(GameText::Battle::battleWinFormat().arg(m_enemiesDefeated));
            m_tipLabel->setText(GameText::Battle::battleVictoryTipFormat()
                                    .arg(usualScoreReward)
                                    .arg(GameState::instance().usualScore()));
        }
        refreshUi();
        scheduleMapCallback(true);
        return true;
    }

    if (m_playerHp <= 0) {
        m_playerHp = 0;
        GameState::instance().setHp(0);
        m_battleEnded = true;
        m_playerTurn = false;
        m_runFinished = true;
        m_hand.discardHand();
        rebuildHandButtons();
        applyBattleEndMessageStyle();
        m_titleLabel->setText(GameText::Battle::battleFailTitle());
        m_tipLabel->setText(GameText::Battle::failTip());
        refreshUi();
        scheduleMapCallback(false);
        return true;
    }

    return false;
}

void BattleWidget::scheduleMapCallback(bool victory)
{
    if (!m_finishCallback || m_mapCallbackScheduled) {
        return;
    }

    m_mapCallbackScheduled = true;
    refreshActionButtons();
    QTimer::singleShot(victory ? GameBalance::Battle::victoryReturnDelayMs()
                               : GameBalance::Battle::failReturnDelayMs(),
                       this,
                       [this, victory]() {
        if (!m_finishCallback) {
            return;
        }
        const std::function<void(bool)> callback = m_finishCallback;
        m_finishCallback = nullptr;
        callback(victory);
    });
}

QList<Card> BattleWidget::createDefaultCards() const
{
    return CardLibrary::starterDeck();
}

QList<Enemy> BattleWidget::createEnemiesForBattle() const
{
    QList<Enemy> enemies;
    if (m_battleNumber >= GameBalance::Battle::bossBattleNumber()) {
        enemies << Enemy::createFinalExam();
        return enemies;
    }

    const int floor = GameState::instance().currentFloor();
    if (floor < GameBalance::Enemies::earlyFloorLimit()) {
        const int roll = GameRandom::instance().bounded(GameBalance::Enemies::earlyEncounterTypes());
        if (roll == 0) {
            enemies << Enemy::createCampusCultist();
        } else if (roll == 1) {
            enemies << Enemy::createDdlSlime();
        } else if (roll == 2) {
            enemies << Enemy::createMorningAlarm()
                    << Enemy::createMorningAlarm();
        } else {
            enemies << Enemy::createClubFlyer();
        }
    } else if (floor < GameBalance::Enemies::middleFloorLimit()) {
        const int roll = GameRandom::instance().bounded(GameBalance::Enemies::middleEncounterTypes());
        if (roll == 0) {
            enemies << Enemy::createHomeworkWorm();
        } else if (roll == 1) {
            enemies << Enemy::createDdlSlime()
                    << Enemy::createDdlSlime();
        } else if (roll == 2) {
            enemies << Enemy::createClubFlyer()
                    << Enemy::createDdlSlime();
        } else if (roll == 3) {
            enemies << Enemy::createTeachingSentry();
        } else {
            enemies << Enemy::createCampusCultist();
        }
    } else {
        const int roll = GameRandom::instance().bounded(GameBalance::Enemies::lateEncounterTypes());
        if (roll == 0) {
            enemies << Enemy::createProjectNob();
        } else if (roll == 1) {
            enemies << Enemy::createDdlSlime()
                    << Enemy::createDdlSlime();
        } else if (roll == 2) {
            enemies << Enemy::createClubFlyer()
                    << Enemy::createDdlSlime();
        } else if (roll == 3) {
            enemies << Enemy::createTeachingSentry();
        } else {
            enemies << Enemy::createDdlSlime()
                    << Enemy::createProjectNob();
        }
    }
    return enemies;
}

int BattleWidget::playerAttackDamage(const Card &card) const
{
    if (card.damage() <= 0) {
        return 0;
    }
    return qMax(0, card.damage() + m_playerStrength * qMax(1, card.strengthMultiplier()));
}

int BattleWidget::currentUsualScoreRewardForEnemy(const Enemy &enemy) const
{
    const int baseScore = enemy.usualScoreReward();
    if (baseScore <= 0) {
        return 0;
    }

    const int decay = qMax(0, m_turnNumber - 1) * GameBalance::CourseGrade::enemyScoreDecayPerTurn();
    return qMax(GameBalance::CourseGrade::enemyMinimumScore(), baseScore - decay);
}

int BattleWidget::currentBattleUsualScoreReward() const
{
    int reward = 0;
    for (const Enemy &enemy : m_enemies) {
        reward += currentUsualScoreRewardForEnemy(enemy);
    }
    return reward;
}

int BattleWidget::currentFinalExamScore() const
{
    const int decay = qMax(0, m_turnNumber - 1) * GameBalance::CourseGrade::finalExamDecayPerTurn();
    return qMax(GameBalance::CourseGrade::finalExamMinimumScore(),
                GameBalance::CourseGrade::finalExamStartScore() - decay);
}

void BattleWidget::showSlashEffect(QWidget *anchor)
{
    if (!anchor) {
        return;
    }

    const QSize effectSize(190, 130);
    const QPoint center = mapFromGlobal(anchor->mapToGlobal(QPoint(anchor->width() / 2,
                                                                    anchor->height() / 2)));

    QPixmap slashPixmap(effectSize);
    slashPixmap.fill(Qt::transparent);
    QPainter painter(&slashPixmap);
    painter.setRenderHint(QPainter::Antialiasing);
    painter.setPen(QPen(QColor(255, 255, 255, 230), 10, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(30, 100), QPointF(154, 26));
    painter.setPen(QPen(QColor(255, 209, 92, 215), 5, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(43, 105), QPointF(168, 30));
    painter.setPen(QPen(QColor(255, 78, 57, 190), 3, Qt::SolidLine, Qt::RoundCap));
    painter.drawLine(QPointF(22, 84), QPointF(128, 20));

    QLabel *slash = new QLabel(this);
    slash->setAttribute(Qt::WA_TransparentForMouseEvents);
    slash->setPixmap(slashPixmap);
    slash->setGeometry(QRect(center - QPoint(effectSize.width() / 2, effectSize.height() / 2), effectSize));
    slash->show();
    slash->raise();

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect(slash);
    slash->setGraphicsEffect(opacity);

    QPropertyAnimation *fade = new QPropertyAnimation(opacity, "opacity", slash);
    fade->setDuration(GameBalance::Battle::slashEffectMs());
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, slash, &QLabel::deleteLater);
    fade->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *move = new QPropertyAnimation(slash, "pos", slash);
    move->setDuration(GameBalance::Battle::slashEffectMs());
    move->setStartValue(slash->pos() + QPoint(-10, 6));
    move->setEndValue(slash->pos() + QPoint(18, -10));
    move->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattleWidget::showImpactFlash(QWidget *anchor)
{
    if (!anchor) {
        return;
    }

    const QRect flashRect(mapFromGlobal(anchor->mapToGlobal(QPoint(0, 0))), anchor->size());
    QLabel *flash = new QLabel(this);
    flash->setAttribute(Qt::WA_TransparentForMouseEvents);
    flash->setGeometry(flashRect.adjusted(-8, -8, 8, 8));
    flash->setStyleSheet(
        "QLabel {"
        "  background: rgba(255, 58, 46, 95);"
        "  border: 3px solid rgba(255, 238, 178, 175);"
        "  border-radius: 18px;"
        "}");
    flash->show();
    flash->raise();

    QGraphicsOpacityEffect *opacity = new QGraphicsOpacityEffect(flash);
    flash->setGraphicsEffect(opacity);

    QPropertyAnimation *fade = new QPropertyAnimation(opacity, "opacity", flash);
    fade->setDuration(GameBalance::Battle::hitFlashMs());
    fade->setStartValue(0.95);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, flash, &QLabel::deleteLater);
    fade->start(QAbstractAnimation::DeleteWhenStopped);
}

void BattleWidget::showDamagePopup(QWidget *anchor, int damage)
{
    if (!anchor || damage <= 0) {
        return;
    }

    QLabel *popup = new QLabel(QStringLiteral("-%1").arg(damage), this);
    popup->setAlignment(Qt::AlignCenter);
    popup->setStyleSheet(
        "QLabel {"
        "  color: #ff3f33;"
        "  background: rgba(25, 5, 5, 95);"
        "  border-radius: 12px;"
        "  font-size: 30px;"
        "  font-weight: 900;"
        "  padding: 3px 10px;"
        "}");
    popup->adjustSize();
    const QPoint start = mapFromGlobal(anchor->mapToGlobal(QPoint(anchor->width() / 2,
                                                                   anchor->height() / 2)));
    popup->move(start + QPoint(24, -12));
    popup->show();
    popup->raise();

    QGraphicsOpacityEffect *effect = new QGraphicsOpacityEffect(popup);
    popup->setGraphicsEffect(effect);

    QPropertyAnimation *fade = new QPropertyAnimation(effect, "opacity", popup);
    fade->setDuration(850);
    fade->setStartValue(1.0);
    fade->setEndValue(0.0);
    connect(fade, &QPropertyAnimation::finished, popup, &QLabel::deleteLater);
    fade->start(QAbstractAnimation::DeleteWhenStopped);

    QPropertyAnimation *move = new QPropertyAnimation(popup, "pos", popup);
    move->setDuration(850);
    move->setStartValue(popup->pos());
    move->setEndValue(popup->pos() + QPoint(0, -54));
    move->start(QAbstractAnimation::DeleteWhenStopped);
}
