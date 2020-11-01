#include "indraggablewidget.h"

#include <QPalette>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>

LDA_BEGIN_NAMESPACE

InDraggableWidget::InDraggableWidget(QWidget *parent) : QWidget(parent)
{
    setAttribute(Qt::WA_Hover, true);
    setMouseTracking(true);
}

void InDraggableWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::MouseButton::LeftButton) {
        holding = true;
        m_x = parentWidget()->mapFromGlobal(parentWidget()->mapToGlobal(this->pos())).x() - parentWidget()->mapFromGlobal(event->globalPos()).x();
        m_y = parentWidget()->mapFromGlobal(parentWidget()->mapToGlobal(this->pos())).y() - parentWidget()->mapFromGlobal(event->globalPos()).y();
        if (autoRaise) {
            this->raise();
        }
        Q_EMIT dragStarted();
    } else {
        holding = false;
    }
    QWidget::mousePressEvent(event);
}

void InDraggableWidget::mouseReleaseEvent(QMouseEvent *event)
{
    if (holding) {
        holding = false;
        Q_EMIT dragEnded();
    }
    QWidget::mouseReleaseEvent(event);
}

void InDraggableWidget::mouseMoveEvent(QMouseEvent *event)
{
    if (holding) {
        QPoint pos = parentWidget()->mapFromGlobal(event->globalPos());
        //Check that WH is in the range to don't make it partially/totally hiden (by nothing)
        //when it goes out of parent's area, so its stays completely visible!

        if (pos.x()+m_x < 0) {
            pos.setX(-m_x);
        } else if (pos.x()+(this->width()+m_x) > parentWidget()->width()) {
            pos.setX(parentWidget()->width()-this->width()-m_x);
        }

        if (pos.y()+m_y < 0) {
            pos.setY(-m_y);
        } else if (pos.y()+(this->height()+m_y) > parentWidget()->height()) {
            pos.setY(parentWidget()->height()-this->height()-m_y);
        }

        this->move(QPoint(pos.x() +m_x, pos.y() +m_y));
    } else {
        QWidget::mouseMoveEvent(event);
    }
}

void InDraggableWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    QPainterPath path;
    path.addRoundedRect(this->rect(), 5, 5);
    painter.fillPath(path, qApp->palette().color(QPalette::ColorGroup::Current, QPalette::ColorRole::Window));
}

bool InDraggableWidget::isRaisingEnabled()
{
    return autoRaise;
}

void InDraggableWidget::enableRaising(bool enable)
{
    autoRaise = enable;
}

void InDraggableWidget::hoverEnterEvent(QHoverEvent *) {}
void InDraggableWidget::hoverLeaveEvent(QHoverEvent *) {}
void InDraggableWidget::hoverMoveEvent(QHoverEvent *) {}

bool InDraggableWidget::event(QEvent * e)
{
    switch(e->type())
    {
    case QEvent::HoverEnter:
        hoverEnterEvent(static_cast<QHoverEvent*>(e));
        return true;
    case QEvent::HoverLeave:
        hoverLeaveEvent(static_cast<QHoverEvent*>(e));
        return true;
    case QEvent::HoverMove:
        hoverMoveEvent(static_cast<QHoverEvent*>(e));
        return true;
    default:
        break;
    }
    return QWidget::event(e);
}

LDA_END_NAMESPACE
