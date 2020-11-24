/*
 * This file is part LDA, Library for Deepin Toolkit Addons.
 * Some components from this project can include sources.
 * If they claim a license, then it is specified as the sources
 * can have been modified. They are shown at beginnig of files
 * or inclusions (when include is replaced by its content).
 *
 * Copyright (C) 2020 Nicolas B. @n1coc4cola or N1coc4colA on Github.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 */

#include "daddonsplittedwindow.h"
#include "daddonsplittedbar.h"
#include "multicolors.h"

#include <QResizeEvent>
#include <QDebug>
#include <QAction>
#include <iostream>

LDA_BEGIN_NAMESPACE

DAddonSplittedWindow::DAddonSplittedWindow(QWidget *parent, bool blur, DBlurEffectWidget::BlendMode blendMode) : QWidget(parent)
{
    this->setAttribute(Qt::WA_TranslucentBackground);
    m_leftw = new DBlurEffectWidget(this);
    m_rightw = new MultiColors(this);

    m_leftw->setFixedSize(m_leftwidth, this->height());
    m_rightw->move(m_leftwidth, 0);
    m_leftw->move(0,0);

    m_leftw->setBlendMode(blendMode);
    m_leftw->setBlurEnabled(blur);

    m_bar = new DAddonSplittedBar(this);
    m_bar->setBlurBackground(true);

    this->resize(this->size());
    this->setObjectName("DAddonSplittedWindow");

    QAction *act = new QAction;
    act->setShortcut(QKeySequence(Qt::Key_F11));
    this->addAction(act);

    connect(act, &QAction::triggered, this, &DAddonSplittedWindow::switchFullScreen);
}

bool DAddonSplittedWindow::eventFilter(QObject *obj, QEvent *ev)
{
    if (obj && m_bottomw && obj == m_bottomw) {
        if (ev->type() == QEvent::Type::Resize) {
            updatePositions();
        }
    }
    return false;
}

void DAddonSplittedWindow::resizeEvent(QResizeEvent *e)
{
    updatePositions();
    QWidget::resizeEvent(e);
}

void DAddonSplittedWindow::updatePositions()
{
    int tmp_w = this->width() - m_leftwidth;

    if (m_rightw != nullptr) {
        m_rightw->setFixedWidth(tmp_w);
        int margins = 0;

        if (!fillTop) {
            margins += splitedbar()->height();
            m_rightw->move(m_leftwidth,splitedbar()->height());
        } else {
            if (m_bar) {
                if (m_bar->isAutoHidden()) {
                    if (fillTop) {
                        if (m_bar->y() == (5 - m_bar->height())) {
                            m_rightw->move(m_leftwidth, 5);
                            margins -= 5;
                        } else {
                            m_rightw->move(m_leftwidth, 0);
                        }
                    } else {
                        m_rightw->move(m_leftwidth, m_bar->y() + m_bar->height());
                    }
                } else {
                    m_rightw->move(m_leftwidth,0);
                }
            } else {
                m_rightw->move(m_leftwidth,0);
            }
        }

        if (!fillBottom) {
            if (bottomWidget()) {
                margins += bottomWidget()->height();
            }
        }

        m_rightw->setFixedHeight(this->height() -margins);
        m_rightw->raise();
    }

    if (m_bottomw != nullptr) {
        m_bottomw->setFixedWidth(tmp_w);
        m_bottomw->move(m_leftwidth, this->height() - m_bottomw->height());
        m_bottomw->raise();
    }

    m_leftw->setFixedHeight(this->height());
    m_bar->setFixedWidth(this->width());
    m_bar->raise();
}

void DAddonSplittedWindow::setLeftAreaWidth(int width)
{
    m_leftw->setFixedWidth(width);
    m_leftwidth = width;
    updatePositions();
}

void DAddonSplittedWindow::setRightWidget(QWidget *w)
{
    if (isOriginal == true) {
        isOriginal = false;
        m_rightw->~QWidget();
    }
    m_rightw = w;
    m_rightw->move(m_leftwidth, 0);
    m_rightw->raise();
    m_bar->raise();
    if (m_bottomw != nullptr) {
        m_bottomw->raise();
    }
    updatePositions();
    update();
}


void DAddonSplittedWindow::setBottomWidget(QWidget *w)
{
    w->setParent(this);
    w->raise();

    if (m_bottomw) {
        removeEventFilter(m_bottomw);
    }
    m_bottomw = w;
    this->installEventFilter(m_bottomw);
    m_bar->raise();
    updatePositions();
    update();
}

void DAddonSplittedWindow::switchFullScreen()
{
    if (forceFullScreen || !window()->windowState().testFlag(Qt::WindowFullScreen)) {
        window()->setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        window()->setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

void DAddonSplittedWindow::enableForceFullScreen(bool force)
{
    forceFullScreen = force;
    switchFullScreen();
}

void DAddonSplittedWindow::setFullScreen(bool full)
{
    if (full || !window()->windowState().testFlag(Qt::WindowFullScreen)) {
        window()->setWindowState(windowState() | Qt::WindowFullScreen);
    } else {
        window()->setWindowState(windowState() & ~Qt::WindowFullScreen);
    }
}

void DAddonSplittedWindow::setFillTop(bool enable)
{
    fillTop = enable;
}

void DAddonSplittedWindow::setFillBottom(bool enable)
{
    fillBottom = enable;
}

void DAddonSplittedWindow::setLeftBlur(bool enable)
{
    if (qobject_cast<DBlurEffectWidget *>(this->leftWidget())) {
        qobject_cast<DBlurEffectWidget*>(m_leftw)->setBlurEnabled(enable);
    }
}

void DAddonSplittedWindow::setLeftBlurMode(Dtk::Widget::DBlurEffectWidget::BlurMode mode)
{
    if (qobject_cast<DBlurEffectWidget *>(this->leftWidget())) {
        qobject_cast<DBlurEffectWidget*>(m_leftw)->setMode(mode);
    }
}

void DAddonSplittedWindow::setLeftBlendMode(DBlurEffectWidget::BlendMode mode)
{
    if (qobject_cast<DBlurEffectWidget *>(this->leftWidget())) {
        qobject_cast<DBlurEffectWidget*>(m_leftw)->setBlendMode(mode);
    }
}

void DAddonSplittedWindow::setLeftBlurColor(QColor color)
{
    if (qobject_cast<DBlurEffectWidget *>(this->leftWidget())) {
        qobject_cast<DBlurEffectWidget*>(m_leftw)->setMaskColor(color);
    }
}

void DAddonSplittedWindow::setTitleBar(DAddonSplittedBar *bar)
{
    this->m_bar->~DAddonSplittedBar();
    this->m_bar = bar;
}

int DAddonSplittedWindow::leftAreaWidth() {return m_leftwidth;}
bool DAddonSplittedWindow::isEnabledTopFill() {return fillTop;}
QWidget *DAddonSplittedWindow::leftWidget() const {return m_leftw;}
bool DAddonSplittedWindow::isEnabledBottomFill() {return fillBottom;}
QWidget *DAddonSplittedWindow::rightWidget() const {return m_rightw;}
QWidget *DAddonSplittedWindow::bottomWidget() const {return m_bottomw;}
DAddonSplittedBar *DAddonSplittedWindow::splitedbar() const {return m_bar;}

LDA_END_NAMESPACE
