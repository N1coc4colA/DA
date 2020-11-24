#include "touchinterfacing.h"

#include "touchsystem.h"

LDA_BEGIN_NAMESPACE

TouchInterfacing::TouchInterfacing(QWidget *source, TouchInterfacing *face)
{
    this->source = source;
    interface_uid = getInternalID();

    source->grabGesture(Qt::GestureType::SwipeGesture);
    source->grabGesture(Qt::GestureType::PanGesture);
    source->grabGesture(Qt::GestureType::PinchGesture);
    source->grabGesture(Qt::GestureType::TapGesture);

    m_system = TouchSystem::instance();
    m_system->listenTo(source, face);
}

TouchInterfacing::~TouchInterfacing()
{
    m_system->destroyInterface(this);
}

void TouchInterfacing::enableEventRedirection(bool enable)
{

    split = enable;
}

void TouchInterfacing::enableEventTranslation(bool enable)
{
    merge = enable;
}

void TouchInterfacing::disableHandling(bool disable)
{
    handling = !disable;
}

void TouchInterfacing::enableInternalTasks(bool enable)
{
    reject = enable;
}

void TouchInterfacing::enableDebug(bool debug)
{
    m_system->enableDebug(debug);
}

void TouchInterfacing::setBlockedEvents(int events)
{
    skipEvents = events;
}

int TouchInterfacing::getInterfaceUID()
{
    return interface_uid;
}

bool TouchInterfacing::handleOtherEvents(QEvent *) { return false; }
bool TouchInterfacing::handleSwipeGesture(SwipingGesture *) { return false; }
bool TouchInterfacing::handleTapGesture(QTapGesture *) { return false; }
bool TouchInterfacing::handlePinchGesture(QPinchGesture *) { return false; }
bool TouchInterfacing::handlePanGesture(QPanGesture *) { return false; }
bool TouchInterfacing::handleMousePress(QMouseEvent *) { return false; }
bool TouchInterfacing::handleMouseRelease(QMouseEvent *) { return false; }
bool TouchInterfacing::handleMouseMove(QMouseEvent *) { return false; }

LDA_END_NAMESPACE
