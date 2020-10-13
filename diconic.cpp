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

#include "diconic.h"

#include <QDir>
#include <QDirIterator>
#include <DThemeManager>
#include <QDebug>
#include <QIcon>

const QString p_assets =    ":/assets/images/";
const QString p_bit =       ":/icons/deepin/builtin/texts/";
const QString p_bii =       ":/icons/deepin/builtin/icons/";
const QString p_lightI =    ":/icons/deepin/builtin/light/icons/";
const QString p_darkI =     ":/icons/deepin/builtin/dark/icons/";
const QString p_chameleon = ":/chameleon/";

LDA_BEGIN_NAMESPACE

DIconic::DIconic(QObject *parent) : QObject(parent) {}

DIconic::~DIconic()
{
    //QObject::~QObject();
}

QIcon *DIconic::fromString(const QString icon_name, IconScopes scope)
{
    QStringList dirs;

    switch (scope) {
    case (int)IconScopes::Assets: dirs << p_assets; break;
    case (int)IconScopes::Chameleon: dirs << p_chameleon; break;
    case (int)IconScopes::BuiltInIcon: dirs << p_bii; break;
    case (int)IconScopes::BuiltInText: dirs << p_bit; break;
    case (int)IconScopes::BuiltIn: dirs << p_bii << p_bit; break;
    case (int)IconScopes::Dark: dirs << p_darkI; break;
    case (int)IconScopes::Light: dirs << p_lightI; break;
    case (int)IconScopes::All: dirs << p_assets << p_lightI << p_darkI << p_bit << p_bii << p_chameleon; break;
    }

    QIcon missing = QIcon::fromTheme(icon_name, QIcon(":/missing.svg"));

    int i = 0;
    while (i<dirs.length()) {
        QDirIterator *it = new QDirIterator(dirs.at(i), QDirIterator::Subdirectories);
        while (it->hasNext()) {
            QString path = it->next().toUtf8().data();
            if (path == QString(dirs.at(i) + icon_name)) {
                return new QIcon(path);
            }
            //path.~QString();
        }
        //it->~QDirIterator();
        i++;
    }
    //dirs.~QStringList();
    qDebug() << "icon " << icon_name << " notfound.";
    return new QIcon(missing);
}

QStringList DIconic::listPaths()
{
    QStringList dirs;
    dirs << p_assets << p_lightI << p_darkI << p_bit << p_bii << p_chameleon;
    return dirs;
}

QStringList DIconic::list()
{
    QStringList data;

    int i = 0;
    QStringList dirs = listPaths();
    while (i<dirs.length()) {
        QDirIterator *it = new QDirIterator(dirs.at(i), QDirIterator::Subdirectories);
        while (it->hasNext()) {
            data << it->next();
        }
        //it->~QDirIterator();
        i++;
    }
    //dirs.~QStringList();
    return data;
}

LDA_END_NAMESPACE
