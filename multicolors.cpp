#include <cmath>

#include "multicolors.h"

#include <QVBoxLayout>
#include <DTitlebar>
#include <QDebug>
#include <QObject>

#define CYELLOW Qt::yellow
#define CORANGE QColor::fromRgb(244, 67, 54)
#define CRED Qt::red
#define CPINK QColor::fromRgb(255, 80, 140)
#define CPURPLE QColor::fromRgb(156, 39, 176)
#define CCYAN QColor::fromRgb(0, 204, 175)
#define CAZURE QColor::fromRgb(0, 188, 212)
#define CBLUE Qt::blue
#define CBLACK Qt::black
#define CPEAR QColor::fromRgb(139, 195, 74)
#define CGREEN Qt::green
#define CGRAY Qt::gray
#define CWHITE Qt::white
#define CMARINBLUE QColor::fromRgb(63, 81, 181)

LDA_USE_NAMESPACE

MultiColors::MultiColors(QWidget *parent) : QWidget(parent) {
}

void MultiColors::paintEvent(QPaintEvent *e)
{
   // QWidget::paintEvent(e);

    int timesY = (int)ceil((double)this->width() / rectangleSize);
    int timesX = (int)ceil((double)this->height() / rectangleSize);

    QPainter paint(this);
    paint.setRenderHint(QPainter::RenderHint::Antialiasing);
    QPen p;

    QColor list[] = {Qt::blue, Qt::red, Qt::white, CYELLOW, CPINK, Qt::green, Qt::black, Qt::gray, Qt::yellow, Qt::cyan,
                    CPURPLE, CAZURE, CPEAR, CMARINBLUE, CORANGE};

    int i = 0, colorC = 0;

    while (i<timesX) {
        int j = 0;
        while (j<timesY) {
            paint.fillRect(QRect(j*rectangleSize, i*rectangleSize, rectangleSize, rectangleSize), list[colorC]);
            colorC++;
            if (colorC>14) {
                colorC = 0;
            }
            j++;
        }
        i++;
    }
}
