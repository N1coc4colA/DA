#include "dotsslidetabs.h"

#include <QPalette>
#include <QPainter>
#include <QPaintEvent>
#include <QPainterPath>
#include <QApplication>
#include <QStackedWidget>

LDA_BEGIN_NAMESPACE

static const int dotSpacingMargin = 3;
//static const int dotRmSizeBordered = 3;

DotsSlideTabs::DotsSlideTabs(QStackedWidget *source, int dots, QWidget *parent) : QWidget(parent)
{
    max_dots = dots;
    setSource(source);
}

DotsSlideTabs::DotsSlideTabs(QStackedWidget *source, QWidget *p) : QWidget(p)
{
    setMouseTracking(true);
    max_dots = 5;
    setSource(source);
}

void DotsSlideTabs::setSource(QStackedWidget *source)
{
    if (source) {
        closeConnections();
        m_source = source;
        raiseConnections();
    }
    updateStates();
}

void DotsSlideTabs::setMaximumVisibleDots(int max)
{
    max_dots = max;
    update();
}

void DotsSlideTabs::raiseConnections()
{
    connect(m_source, &QStackedWidget::currentChanged, this, &DotsSlideTabs::updateStates);
}

void DotsSlideTabs::closeConnections()
{
    if (m_source) {
        disconnect(m_source, &QStackedWidget::currentChanged, this, &DotsSlideTabs::updateStates);
    }
}

void DotsSlideTabs::updateStates()
{
    //Check if the first and last have to be reduced
    //Visible dots checkup
    if (m_source->count() < max_dots) {
        m_visibleDots = m_source->count();
    } else {
        m_visibleDots = max_dots;
    }
    m_visibleDots = m_source->count();
    m_currentDot = m_source->currentIndex();

    update();
}

void DotsSlideTabs::updateDotsSize()
{
    m_dotSize = (this->width()-dotSpacingMargin)/(m_visibleDots+dotSpacingMargin);
    m_dotSize = this->width()/m_dotSize;
}

void DotsSlideTabs::enableAutoDotsSizeUpdate(bool enable)
{
    m_enableAutoUpdates = enable;
}

void DotsSlideTabs::setDotsSize(int size)
{
    m_dotSize = size;
    update();
}

bool DotsSlideTabs::event(QEvent *e)
{
    if (e->type() == QEvent::Type::MouseMove) {
        this->mouseMoveEvent(static_cast<QMouseEvent *>(e));
        e->accept();
        return true;
    }
    return QWidget::event(e);
}

void DotsSlideTabs::enterEvent(QEvent *e)
{
    QWidget::enterEvent(e);
    hovered = true;
    setMouseTracking(true);
    update();
}

void DotsSlideTabs::leaveEvent(QEvent *e)
{
    QWidget::leaveEvent(e);
    hovered = false;
    setMouseTracking(false);
    update();
}

void DotsSlideTabs::mouseMoveEvent(QMouseEvent *e)
{
    bool inUse = false;
    if (hovered) {
        int leftPadding = (this->width() - (m_visibleDots*(m_dotSize + dotSpacingMargin) - dotSpacingMargin))/2;
        if (leftPadding < e->x() && e->x() < this->width() - leftPadding) {
            int topPadding = (this->height() - m_dotSize)/2;
            if (topPadding < e->y() && e->y() < this->height() - topPadding) {
                //Check which dot is focused
                int before = (e->x() - dotSpacingMargin - leftPadding) / (dotSpacingMargin + m_dotSize);
                double after = e->x() - dotSpacingMargin - (before *(dotSpacingMargin + m_dotSize));
                if (((before+1 * (dotSpacingMargin + m_dotSize)) - dotSpacingMargin + e->x()) >= e->x()) {
                    int tmp = 0;
                    if (after >= m_dotSize) {
                        tmp = before -0;
                    } else {
                        tmp = before -1;
                    }
                    if (tmp != hoveredDot) {
                        hoveredDot = tmp;
                        inUse = true;
                    }
                }
            } else {
                hoveredDot = -1;
            }
        } else {
            hoveredDot = -1;
        }
    } else {
        hoveredDot = -1;
    }
    update();
    QWidget::mouseMoveEvent(e);
}

void DotsSlideTabs::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if (m_enableAutoUpdates) {
        updateDotsSize();
    }
}

void DotsSlideTabs::mousePressEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    if (e->button() == Qt::MouseButton::LeftButton && hoveredDot != -1 && !clicked) {
        clicked = true;
        update();
    }
}

void DotsSlideTabs::mouseReleaseEvent(QMouseEvent *e)
{
    QWidget::mousePressEvent(e);
    if (e->button() == Qt::MouseButton::LeftButton && hoveredDot != -1 && clicked) {
        clicked = false;
        m_source->setCurrentIndex(hoveredDot);
        update();
    }
}

void DotsSlideTabs::paintEvent(QPaintEvent *e)
{
    QPainter painter(this);
    if (this->testAttribute(Qt::WidgetAttribute::WA_TranslucentBackground)) {
        painter.fillRect(e->rect(), this->palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Base));
    }

    QPainterPath path;
    int leftPadding = (this->width() - (m_visibleDots*(m_dotSize + dotSpacingMargin) - dotSpacingMargin))/2;

    int i = 0;
    while (i<m_visibleDots) {
        //Paint the dot
        path.addEllipse(QRect((leftPadding + i * (m_dotSize+dotSpacingMargin)), (this->height() - m_dotSize)/2, m_dotSize, m_dotSize));
        i++;
    }
    painter.fillPath(path, this->palette().brush(QPalette::ColorGroup::Current, QPalette::ColorRole::Button));

    //Paint the hovered dot
    if (hovered && hoveredDot != -1) {
        QPainterPath path3;
        path3.addEllipse(QRect((hoveredDot) * (m_dotSize+dotSpacingMargin) + leftPadding, (this->height() - m_dotSize)/2, m_dotSize, m_dotSize));
        painter.fillPath(path3, this->palette().brush(QPalette::ColorGroup::Current, QPalette::ColorRole::ButtonText));
    }

    //Paint the current dot
    QPainterPath path2;
    path2.addEllipse(QRect((m_currentDot) * (m_dotSize+dotSpacingMargin) + leftPadding, (this->height() - m_dotSize)/2, m_dotSize, m_dotSize));
    painter.fillPath(path2, this->palette().brush(QPalette::ColorGroup::Current, QPalette::ColorRole::Highlight));

    if (clicked == true && hoveredDot != -1) {
        QPainterPath path4;
        path4.addEllipse(QRect((hoveredDot) * (m_dotSize+dotSpacingMargin) + leftPadding, (this->height() - m_dotSize)/2, m_dotSize, m_dotSize));
        painter.fillPath(path4, this->palette().brush(QPalette::ColorGroup::Current, QPalette::ColorRole::HighlightedText));
    }
}

LDA_END_NAMESPACE
