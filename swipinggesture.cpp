#include "swipinggesture.h"

LDA_BEGIN_NAMESPACE

SwipingGesture::SwipingGesture(QSwipeGesture *event) : QSwipeGesture(event) {}
SwipingGesture::SwipingGesture(QObject *parent) : QSwipeGesture(parent) {}

Qt::GestureState SwipingGesture::state() const
{
    if (usingInState) {
        return m_state;
    }
    return QSwipeGesture::state();
}

QSwipeGesture::SwipeDirection SwipingGesture::verticalDirection() const
{
    if (usingInState) {
        return m_vert;
    }
    return QSwipeGesture::verticalDirection();
}

QSwipeGesture::SwipeDirection SwipingGesture::horizontalDirection() const
{
    if (usingInState) {
        return m_horiz;
    }
    return QSwipeGesture::horizontalDirection();
}

void SwipingGesture::setGestureState(Qt::GestureState state)
{
    if (!usingInState) {
        m_horiz = QSwipeGesture::horizontalDirection();
        m_vert = QSwipeGesture::verticalDirection();
        usingInState = true;
    }
    m_state = state;
}

void SwipingGesture::setVerticalDirection(QSwipeGesture::SwipeDirection direction)
{
    if (!usingInState) {
        m_state = QSwipeGesture::state();
        m_horiz = QSwipeGesture::horizontalDirection();
        usingInState = true;
    }
    m_vert = direction;
}

void SwipingGesture::setHorizontalDirection(QSwipeGesture::SwipeDirection direction)
{
    if (!usingInState) {
        m_state = QSwipeGesture::state();
        m_vert = QSwipeGesture::verticalDirection();
        usingInState = true;
    }
    m_horiz = direction;
}

LDA_END_NAMESPACE
