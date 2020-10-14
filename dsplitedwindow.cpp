#include "dsplitedwindow.h"
#include "dsplitedbar.h"

#include <QResizeEvent>

LDA_BEGIN_NAMESPACE

DSplitedWindow::DSplitedWindow(QWidget *parent, bool blur, DBlurEffectWidget::BlendMode blendMode) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_leftw = new DBlurEffectWidget(this);
    m_rightw = new QWidget(this);
    m_rightw->setStyleSheet("background-color: white;");

    m_leftw->setFixedSize(m_leftwidth, this->height());
    m_rightw->setFixedSize(this->width() - m_leftwidth, this->height());
    m_rightw->move(m_leftwidth, 0);
    m_leftw->move(0,0);

    m_leftw->setBlendMode(blendMode);
    m_leftw->setBlurEnabled(blur);

    m_bar = new DSplitedBar(this);
    m_bar->setBlurBackground(true);
}

QWidget *DSplitedWindow::leftWidget() const {return m_leftw;}

QWidget *DSplitedWindow::rightWidget() const {return m_rightw;}

void DSplitedWindow::resizeEvent(QResizeEvent *e) {
    int tmp_w = e->size().width() - m_leftwidth;
    if (m_rightw != nullptr) {
        m_rightw->setFixedSize(tmp_w, e->size().height());
    }

    if (m_bottomw != nullptr) {
        m_bottomw->setFixedWidth(tmp_w);
        m_bottomw->move(m_leftwidth, e->size().height() - m_bottomw->height());
    }

    m_leftw->setFixedHeight(e->size().height());
    m_bar->setFixedWidth(e->size().width());

    QWidget::resizeEvent(e);
}

int DSplitedWindow::leftAreaWidth() {return m_leftwidth;}

void DSplitedWindow::setLeftAreaWidth(int width)
{
    m_leftw->setFixedWidth(width);
    m_leftwidth = width;
    m_rightw->move(width, 0);
}

void DSplitedWindow::setRightWidget(QWidget *w)
{
    w->setParent(this);
    if (isOriginal == true) {
        m_rightw->~QWidget();
        isOriginal = false;
    }
    m_rightw = w;
    m_rightw->move(m_leftwidth, 0);
    m_bar->raise();
    if (m_bottomw != nullptr) {
        m_bottomw->raise();
    }
}

DSplitedBar *DSplitedWindow::splitedbar() const
{
    return m_bar;
}

QWidget *DSplitedWindow::bottomWidget() const
{
    return m_bottomw;
}

void DSplitedWindow::setBottomWidget(QWidget *w)
{
    w->setParent(this);
    w->raise();
    m_bottomw = w;
    m_bar->raise();
    update();
}

void DSplitedWindow::switchFullScreen()
{
    if (forceFullScreen || !window()->windowState().testFlag(Qt::WindowFullScreen)) {
        window()->setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        window()->setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

void DSplitedWindow::enableForceFullScreen(bool force)
{
    forceFullScreen = force;
    switchFullScreen();
}

void DSplitedWindow::setFullScreen(bool full)
{
    if (full || !window()->windowState().testFlag(Qt::WindowFullScreen)) {
        window()->setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        window()->setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

LDA_END_NAMESPACE

