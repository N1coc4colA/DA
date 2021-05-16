#include "element.h"
#include "proxypainter.h"

#include <QPainterPath>
#include <QResizeEvent>
#include <QMimeData>
#include <QPainter>
#include <QLabel>
#include <QDebug>
#include <QTimer>
#include <QTime>
#include <QDrag>

LDA_BEGIN_NAMESPACE

Element::Element(int h, QWidget *parent, Qt::WindowFlags f) : QFrame(parent, f)
{
    m_height = h;
    setFixedHeight(m_height);
    setMinimumHeight(m_height);
    setMaximumHeight(m_height);
}

Element::~Element() {}

void Element::paintEvent(QPaintEvent *e)
{
    QPainter *p = new QPainter(this);
    QPainterPath path;
    path.addRoundedRect(this->rect(), *rad, *rad);
    p->setClipPath(path);
    paint(new ProxyPainter(p, nullptr, true), e);
    p->~QPainter();
}

void Element::resizeEvent(QResizeEvent *e)
{
    QWidget::resizeEvent(e);
    if (e->isAccepted() == true) {
        int wi = this->width();
        int he = this->height();
        bool mustUpdate = false;
        if (this->size().height() != m_height) {
            he = m_height;
            mustUpdate = true;
        }
        if (e->oldSize().width() -11 != e->size().width()) {
            wi = e->size().width() -11;
            mustUpdate = true;
        }
        if (mustUpdate == true) {
            this->resize(wi, he);
        }
    }
}

void Element::dropEvent(QDropEvent *event)
{
   QByteArray t = event->mimeData()->data("application/x-item");
   event->accept();
}

void Element::dragEnterEvent(QDragEnterEvent *event)
{
   event->accept();
}

void Element::dragLeaveEvent(QDragLeaveEvent *event)
{
   event->accept();
}

void Element::paint(ProxyPainter *painter, const QPaintEvent *)
{
    painter->fillRect(rect(), Qt::blue);
}

QSize Element::sizeHint() const
{
    return QSize(40, m_height);
}

int Element::radius()
{
    return *rad;
}

DerivedElement::DerivedElement(int s) : Element(s) {
    QTimer *timer = new QTimer(this);
    connect(timer, SIGNAL(timeout()), this, SLOT(update()));
    timer->start(1000);
}

DerivedElement::~DerivedElement()
{
    this->Element::~Element();
}

void DerivedElement::paint(ProxyPainter *p, const QPaintEvent *event)
{
    p->setRenderHints(QPainter::Antialiasing);
    QLinearGradient gradient(0, 0, 0, this->height());
    QPainterPath path;
    path.addRoundedRect(rect(), radius(), radius());
    gradient.setColorAt(0, QColor("#895bdc"));
    gradient.setColorAt(1, QColor("#3F51B5"));
    p->setBrush(gradient);
    p->fillPath(path, gradient);

    p->setPen(Qt::white);
    QFont f = p->font();
    f.setBold(true);
    f.setPointSize(40);
    p->setFont(f);
    p->drawText(10, 60, QTime::currentTime().toString("hh:mm:ss"));
}

LDA_END_NAMESPACE
