#include "touchsystem.h"
#include "touchinterfacing.h"

#include "iod.h"

#include <QMetaEnum>

LDA_BEGIN_NAMESPACE

#ifndef op
bool operator == (const SkipEvents& y, const SkipEvents& x) { return ( (((x & y) == x)) ? true : false); }
bool operator != (const SkipEvents& y, const SkipEvents& x) { return !( (((x & y) == x)) ? true : false); }
#endif

TouchSystem::TouchSystem() : QObject(nullptr) {}

void TouchSystem::listenTo(QWidget *object, TouchInterfacing *face)
{
    m_interfaces << face;
    m_sources[object] = face;
    object->installEventFilter(this);
}

void TouchSystem::destroyInterface(TouchInterfacing *face)
{
    m_interfaces.removeOne(face);
}

void TouchSystem::check()
{
}

void TouchSystem::enableDebug(bool enable)
{
    debug = enable;
}

bool TouchSystem::eventFilter(QObject *object, QEvent *event)
{
    check();
    bool out = true;
    if (QWidget *current_source = qobject_cast<QWidget *>(object)) {

        if (m_sources.contains(current_source)) {
            //source = current_source;
            interface = m_sources[current_source];

            if (!interface->reject) {
                if (interface->skipEvents != SkipEvents::Any) {

                    if (event->type() == QEvent::Type::Gesture) {
                        if (interface->skipEvents != SkipEvents::GestureAll) {
                            if (processGestures(static_cast<QGestureEvent *>(event))) {
                                out = true;
                            }
                        }

                    } else {

                        if (interface->skipEvents != SkipEvents::MouseAll) {
                            if (event->type() == QEvent::Type::MouseMove) {
                                if (processMouseMove(static_cast<QMouseEvent *>(event))) {
                                    out = true;
                                }
                            } else if (event->type() == QEvent::Type::MouseButtonPress) {
                                if (processMousePress(static_cast<QMouseEvent *>(event))) {
                                    out = true;
                                }
                            } else if (event->type() == QEvent::Type::MouseButtonRelease) {
                                if (processMouseRelease(static_cast<QMouseEvent *>(event))) {
                                    out = true;
                                }
                            } else {
                                if (interface->skipEvents != SkipEvents::Other) {
                                    return interface->handleOtherEvents(event);
                                }
                            }
                        }

                    }
                    if (debug) {
                        iod::IOD::output("TouchSystem (");
                        iod::IOD::output(interface->source->metaObject()->className());
                        iod::IOD::output("): | UID: ");
                        iod::IOD::output(QString::number(interface->interface_uid).toUtf8().data());
                        iod::IOD::output(" | Event Type: ");
                        iod::IOD::output(QString(QMetaEnum::fromType<QEvent::Type>().key(event->type())).toUtf8().data());
                        iod::IOD::output(" | Watched object: ");
                        iod::IOD::output(QString(object->metaObject()->className()).toUtf8().data());
                        iod::IOD::output(" - ");
                        iod::IOD::output(object);
                        iod::IOD::output("\n");
                    }
                }
            }
            //interface = nullptr;
        }
    }
    event->setAccepted(out);
    return out;
}

bool TouchSystem::processGestures(QGestureEvent *event)
{
    bool out = false;
    if (QGesture *swipe = event->gesture(Qt::SwipeGesture)) {
        if (interface->skipEvents != SkipEvents::SwipeGesture) {
            if (interface->handleSwipeGesture(new SwipingGesture(static_cast<QSwipeGesture *>(swipe)))) {
                out = true;
            }
        }
    } else if (QGesture *pan = event->gesture(Qt::PanGesture)) {
        if (interface->skipEvents != SkipEvents::PanGesture) {
            if (interface->handlePanGesture(static_cast<QPanGesture *>(pan))) {
                out = true;
            }
        }
    }

    if (QGesture *pinch = event->gesture(Qt::PinchGesture)) {
        if (interface->skipEvents != SkipEvents::PinchGesture) {
            if (interface->handlePinchGesture(static_cast<QPinchGesture *>(pinch))) {
                out = true;
            }
        }
    }
    if (QGesture *tap = event->gesture(Qt::TapGesture)) {
        if (interface->skipEvents != SkipEvents::TapGesture) {
            if (interface->handleTapGesture(static_cast<QTapGesture *>(tap))) {
                out = true;
            }
        }
    }

    return out;
}


bool TouchSystem::processMouseMove(QMouseEvent *e)
{
    if (interface->merge) {
        if (interface->press) {
            if (interface->handling) {
                bool out = false;
                bool val1 = false;
                if (interface->m_swipe) {
                    if (interface->skipEvents != SkipEvents::SwipeGesture) {
                        //Update the swipe
                        interface->m_swipe->setSwipeAngle(generateAngle(*interface->pointSource, e->pos()));
                        interface->m_swipe->setGestureState(Qt::GestureState::GestureUpdated);

                        //Calculate directions
                        QSwipeGesture::SwipeDirection dir = QSwipeGesture::SwipeDirection::Left;
                        if (interface->pointSource->x() < e->pos().x()) {
                            dir = QSwipeGesture::SwipeDirection::Right;
                        }
                        interface->m_swipe->setHorizontalDirection(dir);
                        if (interface->pointSource->y() < e->pos().y()) {
                            dir = QSwipeGesture::SwipeDirection::Up;
                        } else {
                            dir = QSwipeGesture::SwipeDirection::Down;
                        }
                        interface->m_swipe->setVerticalDirection(dir);

                        //Reupload
                        SwipingGesture *gest = new SwipingGesture;
                        memcpy((void *)gest, (void *)interface->m_swipe, sizeof (*interface->m_swipe));
                        val1 = interface->handleSwipeGesture(gest);
                        //gest->deleteLater();
                    }
                } else {
                    if (interface->skipEvents != SkipEvents::SwipeGesture) {
                        //Generate the swipe
                        interface->m_swipe = new SwipingGesture();
                        //Calculate angle
                        interface->m_swipe->setSwipeAngle(generateAngle(*interface->pointSource, e->pos()));
                        interface->m_swipe->setGestureState(Qt::GestureState::GestureStarted);
                        //Upload event
                        SwipingGesture *gest = new SwipingGesture;
                        memcpy((void *)gest, (void *)interface->m_swipe, sizeof (*interface->m_swipe));
                        val1 = interface->handleSwipeGesture(gest);
                        //gest->~SwipingGesture();
                    }
                }
                if (interface->split) {
                    bool val2 = false;
                    if (interface->skipEvents != SkipEvents::MouseMove) {
                        val2 = interface->handleMouseMove(e);
                    }
                    if (val1 || val2) {
                        out = true;
                    }
                } else {
                    if ((!val1) && (interface->skipEvents != SkipEvents::MouseMove)) {
                        out = interface->handleMouseMove(e);
                    }
                }
                return out;
            }
        }
    }

    if (interface->skipEvents != SkipEvents::MouseMove) {
        return interface->handleMouseMove(e);
    }
    return false;
}

bool TouchSystem::processMousePress(QMouseEvent *e)
{
    if (interface->merge) {
        if (e->button() == Qt::LeftButton) {
            interface->press = true;
            interface->pointSource = new QPoint(e->pos());
            if (interface->split) {
                if (interface->skipEvents != SkipEvents::MousePress) {
                    interface->handleMousePress(e);
                }
            }
            return true;
        }
    }
    if (interface->skipEvents != SkipEvents::MousePress) {
        interface->handleMousePress(e);
    }
    return false;
}

bool TouchSystem::processMouseRelease(QMouseEvent *e)
{
    if (interface->merge) {
        if (e->button() == Qt::LeftButton) {
            interface->press = false;
            if (e->pos() == *interface->pointSource) {
                bool val1 = false;

                if (interface->skipEvents != SkipEvents::TapGesture) {
                    //Means a click event, so generate a tap!
                    QTapGesture *gest = new QTapGesture;
                    gest->setPosition(e->pos());
                    gest->setHotSpot(e->globalPos());
                    interface->pointSource->~QPoint();
                    val1 = interface->handleTapGesture(gest);
                    //gest->~QTapGesture();

                    if (debug) {
                        iod::IOD::output("TouchSystem (");
                        iod::IOD::output(interface->source->metaObject()->className());
                        iod::IOD::output("): | UID: ");
                        iod::IOD::output(QString::number(interface->interface_uid).toUtf8().data());
                        iod::IOD::output(" | Event Type: ");
                        iod::IOD::output("SynthetizedTapEvent");
                        iod::IOD::output(" | Watched object: ");
                        iod::IOD::output(QString(interface->source->metaObject()->className()).toUtf8().data());
                        iod::IOD::output(" - ");
                        iod::IOD::output(interface->source);
                        iod::IOD::output("\n");
                    }
                }

                if (interface->split) {
                    if (val1 || interface->handleMouseRelease(e)) {
                        return true;
                    } else {
                        return false;
                    }
                } else {
                    return val1;
                }
            } else {
                if (interface->m_swipe) {
                    //Clear the swipe
                    interface->m_swipe->setGestureState(Qt::GestureState::GestureFinished);
                    SwipingGesture *tmp = new SwipingGesture;
                    memcpy((void *)tmp, (void *)interface->m_swipe, sizeof (*interface->m_swipe));
                    //m_swipe->~SwipingGesture();

                    bool val1 = false;
                    if (interface->skipEvents != SkipEvents::SwipeGesture) {
                        val1 = interface->handleSwipeGesture(tmp);
                    }

                    if (debug) {
                        iod::IOD::output("TouchSystem (");
                        iod::IOD::output(interface->source->metaObject()->className());
                        iod::IOD::output("): | UID: ");
                        iod::IOD::output(QString::number(interface->interface_uid).toUtf8().data());
                        iod::IOD::output(" | Event Type: ");
                        iod::IOD::output("SynthetizedSwipeEvent");
                        iod::IOD::output(" | Watched object: ");
                        iod::IOD::output(QString(interface->source->metaObject()->className()).toUtf8().data());
                        iod::IOD::output(" - ");
                        iod::IOD::output(interface->source);
                        iod::IOD::output("\n");
                    }

                    if (interface->split) {
                        if (val1) {
                            if ((interface->skipEvents != SkipEvents::MouseRelease) && interface->handleMouseRelease(e)) {
                                return true;
                            } else {
                                return false;
                            }
                        } else {
                            if (interface->skipEvents != SkipEvents::MouseRelease) {
                                return interface->handleMouseRelease(e);
                            }
                        }
                    }
                    return val1;
                }
            }
        }
    }
    if (interface->skipEvents != SkipEvents::MouseRelease) {
        return interface->handleMouseRelease(e);
    }
    return false;
}

LDA_END_NAMESPACE
