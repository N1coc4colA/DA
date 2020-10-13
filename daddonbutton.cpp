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

#include "daddonbutton.h"

LDA_BEGIN_NAMESPACE

DAddonIconButton::DAddonIconButton(QWidget *parent, Qt::AnchorPoint xa, Qt::AnchorPoint ya) : Dtk::Widget::DIconButton(parent)
{
    connect(this, &Dtk::Widget::DIconButton::clicked, this, &DAddonIconButton::requestMenu);
    anchor_x = xa;
    anchor_y = ya;
}

DAddonIconButton::~DAddonIconButton() {}

int DAddonIconButton::processAnchor(Qt::AnchorPoint a)
{
    int v = 0;
    switch (a) {
    case Qt::AnchorTop: v = 0; break;
    case Qt::AnchorBottom: v = this->rect().height(); break;
    case Qt::AnchorLeft: v = 0; break;
    case Qt::AnchorRight: v = this->rect().width(); break;
    case Qt::AnchorVerticalCenter: v = this->rect().height()/2; break;
    case Qt::AnchorHorizontalCenter: v = this->rect().width()/2; break;
    default: break; //In case that you pass an int as the value and it is out of range!
    }
    return v;
}

void DAddonIconButton::handleMenuRequest(QMenu *menu)
{
    if (menu != nullptr) {
        QPoint center = mapToGlobal(QPoint(processAnchor(anchor_x), processAnchor(anchor_y)));
        int menuHeight = menu->sizeHint().height();
        int menuWidth = menu->sizeHint().width();
        center.setY(center.y() - menuHeight -10);
        center.setX(center.x() - menuWidth / 2);
        menu->exec(center);
    }
}

Qt::AnchorPoint DAddonIconButton::anchorX() const
{
    return anchor_x;
}

Qt::AnchorPoint DAddonIconButton::anchorY() const
{
    return anchor_y;
}

void DAddonIconButton::setMenuXAnchor(Qt::AnchorPoint p)
{
    anchor_x = p;
}

void DAddonIconButton::setMenuYAnchor(Qt::AnchorPoint p)
{
    anchor_y = p;
}

void DAddonIconButton::setMenuAnchors(Qt::AnchorPoint x, Qt::AnchorPoint y)
{
    anchor_x = x;
    anchor_y = y;
}

LDA_END_NAMESPACE
