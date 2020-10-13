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

#include <QResizeEvent>

LDA_BEGIN_NAMESPACE

DAddonSplittedWindow::DAddonSplittedWindow(QWidget *parent, bool blur, DBlurEffectWidget::BlendMode blendMode) : QWidget(parent)
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

    m_bar = new DAddonSplittedBar(this);
    m_bar->setBlurBackground(true);
}

QWidget *DAddonSplittedWindow::leftWidget() const {return m_leftw;}

QWidget *DAddonSplittedWindow::rightWidget() const {return m_rightw;}

void DAddonSplittedWindow::resizeEvent(QResizeEvent *e) {
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

int DAddonSplittedWindow::leftAreaWidth() {return m_leftwidth;}

void DAddonSplittedWindow::setLeftAreaWidth(int width)
{
    m_leftw->setFixedWidth(width);
    m_leftwidth = width;
    m_rightw->move(width, 0);
}

void DAddonSplittedWindow::setRightWidget(QWidget *w)
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

DAddonSplittedBar *DAddonSplittedWindow::splitedbar() const
{
    return m_bar;
}

QWidget *DAddonSplittedWindow::bottomWidget() const
{
    return m_bottomw;
}

void DAddonSplittedWindow::setBottomWidget(QWidget *w)
{
    w->setParent(this);
    w->raise();
    m_bottomw = w;
    m_bar->raise();
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

LDA_END_NAMESPACE
