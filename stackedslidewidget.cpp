#include "stackedslidewidget.h"

#include "swipinggesture.h"

LDA_BEGIN_NAMESPACE

StackedSlideWidget::StackedSlideWidget(QWidget *parent) : QStackedWidget(parent), TouchInterfacing(this, ((TouchInterfacing *)this)) {}

StackedSlideWidget::~StackedSlideWidget()
{
    this->TouchInterfacing::~TouchInterfacing();
    this->QStackedWidget::~QStackedWidget();
}

bool StackedSlideWidget::handleOtherEvents(QEvent *e)
{
    return this->event(e);
}

void StackedSlideWidget::paintEvent(QPaintEvent *) {}

bool StackedSlideWidget::handleSwipeGesture(SwipingGesture *gesture)
{

    int index = this->currentIndex();
    if (gesture->state() == Qt::GestureState::GestureFinished) {
        if (gesture->horizontalDirection() == QSwipeGesture::SwipeDirection::Right) {
            if ((index-1) > -1) {
                index--;
            } else {
                index = count()-1;
            }
        } else if (gesture->horizontalDirection() == QSwipeGesture::SwipeDirection::Left) {
            if ((index+1) < this->count()) {
                index++;
            } else {
                index = 0;
            }
        }
        this->setCurrentIndex(index);
        return true;
    } else {
        return false;
    }
}

LDA_END_NAMESPACE
