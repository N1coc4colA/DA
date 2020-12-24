/*
 * Copyright (C) 2017 ~ 2017 Deepin Technology Co., Ltd.
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
 */

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

#include "daddonapplication.h"
#include "daddonsplittedbar.h"
#include "daddonsplittedwindow.h"

/*
 * Copyright (C) 2015 ~ 2017 Deepin Technology Co., Ltd.
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
 */

#ifndef DAPPLICATION_P_H
#define DAPPLICATION_P_H

#include <DObjectPrivate>
#include <DApplication>
#include <iostream>

#include "dpathbuf.h"

#include <QIcon>

class QLocalServer;
class QTranslator;

DCORE_USE_NAMESPACE
DWIDGET_BEGIN_NAMESPACE

class DAboutDialog;

class DApplicationPrivate : public DObjectPrivate
{
    D_DECLARE_PUBLIC(DApplication)

public:
    explicit DApplicationPrivate(DApplication *q);
    ~DApplicationPrivate();

    D_DECL_DEPRECATED QString theme() const;
    D_DECL_DEPRECATED void setTheme(const QString &theme);

    bool setSingleInstanceBySemaphore(const QString &key);
#ifdef Q_OS_UNIX
    bool setSingleInstanceByDbus(const QString &key);
#endif

    bool loadDtkTranslator(QList<QLocale> localeFallback);
    bool loadTranslator(QList<DPathBuf> translateDirs, const QString &name, QList<QLocale> localeFallback);
    void _q_onNewInstanceStarted();

    static bool isUserManualExists();
public:
// int m_pidLockFD = 0;
    QLocalServer *m_localServer = nullptr;
#ifdef Q_OS_UNIX
    QStringList m_monitoredStartupApps;
#endif

    QString productName;
    QIcon   productIcon;
    QString appLicense;
    QString appDescription;
    QString homePage;
    QString acknowledgementPage;
    bool acknowledgementPageVisible = true;

    bool visibleMenuShortcutText   = false;
    bool visibleMenuCheckboxWidget = false;
    bool visibleMenuIcon           = false;
    bool autoActivateWindows       = false;

    DAboutDialog *aboutDialog = Q_NULLPTR;

    friend class DCommonAccess;
};

DWIDGET_END_NAMESPACE

#endif // DAPPLICATION_P_H

#include <QDebug>
#include <QMenu>
#include <QHBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QProcess>
#include <QTimer>
#include <QPropertyAnimation>

#include <DWindowManagerHelper>

LDA_BEGIN_NAMESPACE

static const int DefaultTitlebarHeight = 50;
static const int DefaultIconHeight = 32;
static const int DefaultIconWidth = 32;
static const int hiddenVisibleArea = 1;
static const int hiddenAnimationSpeed = 5;

DAddonSplittedBarPrivate::DAddonSplittedBarPrivate(DAddonSplittedBar *qq): DObjectPrivate(qq)
{
}

void DAddonSplittedBarPrivate::init()
{
    D_Q(DAddonSplittedBar);

    mainLayout      = new QHBoxLayout;
    leftArea        = new QWidget;
    leftLayout      = new QHBoxLayout(leftArea);
    rightArea       = new QWidget;
    rightLayout     = new QHBoxLayout;
    centerArea      = new DLabel(q);
    centerLayout    = new QHBoxLayout(centerArea);
    iconLabel       = new DIconButton(q);
    buttonArea      = new QWidget;
    minButton       = new DWindowMinButton;
    maxButton       = new DWindowMaxButton;
    closeButton     = new DWindowCloseButton;
    optionButton    = new DWindowOptionButton;
    quitFullButton  = new DWindowQuitFullButton;
    separatorTop    = new DHorizontalLine(q);
    separator       = new DHorizontalLine(q);
    titleLabel      = centerArea;
    titleLabel->setElideMode(Qt::ElideMiddle);

    minButton->installEventFilter(q);
    maxButton->installEventFilter(q);
    closeButton->installEventFilter(q);
    optionButton->installEventFilter(q);
    quitFullButton->installEventFilter(q);

    buttonArea->setObjectName("DAddonSplittedBarButtonArea");
    centerArea->setObjectName("DAddonSplittedBarCenterArea");
    optionButton->setObjectName("DAddonSplittedBarDWindowOptionButton");
    optionButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    minButton->setObjectName("DAddonSplittedBarDWindowMinButton");
    minButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    maxButton->setObjectName("DAddonSplittedBarDWindowMaxButton");
    maxButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    closeButton->setObjectName("DAddonSplittedBarDWindowCloseButton");
    closeButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    quitFullButton->setObjectName("DTitlebarDWindowQuitFullscreenButton");
    quitFullButton->setAccessibleName("DTitlebarDWindowQuitFullscreenButton");
    quitFullButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    quitFullButton->hide();

    iconLabel->setIconSize(QSize(DefaultIconWidth, DefaultIconHeight));
    iconLabel->setWindowFlags(Qt::WindowTransparentForInput);
    iconLabel->setFocusPolicy(Qt::NoFocus);
    iconLabel->setFlat(true);
    iconLabel->hide();

    leftArea->setWindowFlag(Qt::WindowTransparentForInput);
    leftArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    leftLayout->setContentsMargins(0, 0, 0, 0);
    leftArea->setFixedWidth(q->left_margin);

    centerLayout->setContentsMargins(0, 0, 0, 0);
    centerArea->setText(qApp->applicationDisplayName());
    centerArea->setWindowFlags(Qt::WindowTransparentForInput);
    centerArea->setFrameShape(QFrame::NoFrame);
    centerArea->setAutoFillBackground(false);
    centerArea->setBackgroundRole(QPalette::NoRole);
    centerArea->setAlignment(Qt::AlignCenter);
    centerArea->setWordWrap(true);
    //centerArea->setMaximumWidth(q->left_margin);

    buttonArea->setWindowFlag(Qt::WindowTransparentForInput);
    buttonArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::MinimumExpanding);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonArea);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->addWidget(optionButton);
    buttonLayout->addWidget(minButton);
    buttonLayout->addWidget(maxButton);
    buttonLayout->addWidget(quitFullButton);
    buttonLayout->addWidget(closeButton);

    rightArea->setWindowFlag(Qt::WindowTransparentForInput);
    rightArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    QHBoxLayout *rightAreaLayout = new QHBoxLayout(rightArea);
    rightAreaLayout->setContentsMargins(0, 0, 0, 0);
    rightAreaLayout->setMargin(0);
    rightAreaLayout->setSpacing(0);
    rightAreaLayout->addLayout(rightLayout);
    rightAreaLayout->addWidget(buttonArea, 0, Qt::AlignmentFlag::AlignLeft);

    separatorTop->setFixedHeight(1);
    separatorTop->hide();
    separatorTop->setWindowFlags(Qt::WindowTransparentForInput);

    separator->setFixedHeight(1);
    separator->hide();
    separator->setWindowFlags(Qt::WindowTransparentForInput);

    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->addWidget(leftArea, 0, Qt::AlignLeft);
    mainLayout->addWidget(rightArea, 0, Qt::AlignRight);

    titleLabel->setMargin(5);

    q->setLayout(mainLayout);
    q->setFixedHeight(DefaultTitlebarHeight);
    q->setMinimumHeight(DefaultTitlebarHeight);

    q->connect(quitFullButton, &DWindowQuitFullButton::clicked, q, [ = ]() {
        bool isFullscreen = targetWindow()->windowState().testFlag(Qt::WindowFullScreen);
        if (isFullscreen) {
            targetWindow()->showNormal();
        } else {
            targetWindow()->showFullScreen();
        }
    });
    q->connect(optionButton, &DWindowOptionButton::clicked, q, &DAddonSplittedBar::optionClicked);
    q->connect(DWindowManagerHelper::instance(), SIGNAL(windowMotifWMHintsChanged(quint32)),
               q, SLOT(_q_onTopWindowMotifHintsChanged(quint32)));

    q->setFrameShape(QFrame::NoFrame);
    q->setBackgroundRole(QPalette::Base);
    q->setAutoFillBackground(true);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    q->setFocusPolicy(Qt::StrongFocus);

#ifndef QT_NO_MENU
    q->setIconMenu(this->automateIconMenu());
    QObject::connect(iconLabel, &DIconButton::clicked, q, &DAddonSplittedBar::showIconMenu);
#endif

    q->connect(q, &DAddonSplittedBar::setupMaximizedSettings, q, [this]() {this->handleMaximizingPreparation();});
}

QWidget *DAddonSplittedBarPrivate::targetWindow()
{
    D_Q(DAddonSplittedBar);
    return q->topLevelWidget()->window();
}

bool DAddonSplittedBarPrivate::isVisableOnFullscreen()
{
    D_Q(DAddonSplittedBar);
    return !q->property("_restore_height").isValid();
}

void DAddonSplittedBarPrivate::hideOnFullscreen()
{
    D_Q(DAddonSplittedBar);
    if (q->height() > 0) {
        q->setProperty("_restore_height", q->height());
    }
    //q->setFixedHeight(0);
}

void DAddonSplittedBarPrivate::showOnFullscreen()
{
    D_Q(DAddonSplittedBar);
    if (q->property("_restore_height").isValid()) {
        q->setFixedHeight(q->property("_restore_height").toInt());
        q->setProperty("_restore_height", QVariant());
    }
}

void DAddonSplittedBarPrivate::updateFullscreen()
{
    D_Q(DAddonSplittedBar);

    if (!autoHideOnFullscreen) {
        return;
    }

    bool isFullscreen = targetWindow()->windowState().testFlag(Qt::WindowFullScreen);
    bool isMaximized = targetWindow()->windowState().testFlag(Qt::WindowMaximized);
    auto mainWindow = qobject_cast<DAddonSplittedWindow *>(targetWindow());
    bool value = true;

    if (!isFullscreen && !isMaximized) {
        quitFullButton->hide();
        showOnFullscreen();
        q->isHidden = !value;
        q->isMaximized = !value;
        q->move(q->x(), 0);
    } else {
        quitFullButton->show();

        q->setParent(mainWindow);
        q->show();

        q->isHidden = value;
        if (isMaximized) {
            q->isMaximized = value;
        }

        if (isFullscreen) {
            hideOnFullscreen();
        }
    }
}

void DAddonSplittedBarPrivate::handleMaximizingPreparation()
{
    D_Q(DAddonSplittedBar);
    q->move(q->x(), 5 - q->height());
}

void DAddonSplittedBarPrivate::updateButtonsState(Qt::WindowFlags type)
{
    D_Q(DAddonSplittedBar);
    bool useDXcb = DPlatformWindowHandle::isEnabledDXcb(targetWindow());
    bool isFullscreen = targetWindow()->windowState().testFlag(Qt::WindowFullScreen);

    bool forceShow = false;
#ifndef Q_OS_LINUX
    forceShow = false;
#endif

    bool showTitle = (type.testFlag(Qt::WindowTitleHint) || forceShow) && !embedMode;
    if (titleLabel) {
        titleLabel->setVisible(showTitle);
    }

    bool forceHide = (!useDXcb) || embedMode || isFullscreen;

    bool showMin = (type.testFlag(Qt::WindowMinimizeButtonHint) || forceShow) && !forceHide;
    minButton->setVisible(showMin);


    bool allowResize = true;

    if (q->window() && q->window()->windowHandle()) {
        auto functions_hints = DWindowManagerHelper::getMotifFunctions(q->window()->windowHandle());
        allowResize = functions_hints.testFlag(DWindowManagerHelper::FUNC_RESIZE);
    }

    bool showMax = (type.testFlag(Qt::WindowMaximizeButtonHint) || forceShow) && !forceHide && allowResize;
    maxButton->setVisible(showMax);

    bool showClose = (type.testFlag(Qt::WindowCloseButtonHint) || forceShow) && !forceHide;
    closeButton->setVisible(showClose);

    if (q->isHidden) {
        if (!q->isMaximized) {
            //quitFullButton->setVisible(true);
        }
    }
}

void DAddonSplittedBarPrivate::updateButtonsFunc()
{
    if (!targetWindowHandle || !qgetenv("WAYLAND_DISPLAY").isEmpty()) {
        return;
    }

    DWindowManagerHelper::setMotifFunctions(
        targetWindowHandle,
        DWindowManagerHelper::FUNC_MAXIMIZE,
        !disableFlags.testFlag(Qt::WindowMaximizeButtonHint));
    DWindowManagerHelper::setMotifFunctions(
        targetWindowHandle,
        DWindowManagerHelper::FUNC_MINIMIZE,
        !disableFlags.testFlag(Qt::WindowMinimizeButtonHint));
    DWindowManagerHelper::setMotifFunctions(
        targetWindowHandle,
        DWindowManagerHelper::FUNC_CLOSE,
        !disableFlags.testFlag(Qt::WindowCloseButtonHint));
}

void DAddonSplittedBarPrivate::updateCenterArea()
{
    D_QC(DAddonSplittedBar);

    if (centerArea->isHidden()) {
        return;
    }

    int padding = qMax(leftArea->width(), rightArea->width());
    QRect rect(0, 0, q->width() - 2 * padding, q->height());
    rect.moveCenter(q->rect().center());
    centerArea->setGeometry(rect);
}

void DAddonSplittedBarPrivate::handleParentWindowStateChange()
{
    maxButton->setMaximized(targetWindow()->windowState() == Qt::WindowMaximized);
    updateFullscreen();
    updateButtonsState(targetWindow()->windowFlags());
}

void DAddonSplittedBarPrivate::handleParentWindowIdChange()
{
    if (!targetWindowHandle) {
        targetWindowHandle = targetWindow()->windowHandle();

        updateButtonsFunc();
    } else if (targetWindow()->windowHandle() != targetWindowHandle) {
        qWarning() << "targetWindowHandle change" << targetWindowHandle << targetWindow()->windowHandle();
    }
}

void DAddonSplittedBarPrivate::_q_toggleWindowState()
{
    QWidget *parentWindow = targetWindow();

    if (!parentWindow || disableFlags.testFlag(Qt::WindowMaximizeButtonHint)) {
        return;
    }

    if (parentWindow->isMaximized()) {
        parentWindow->showNormal();
    } else if (!parentWindow->isFullScreen() && (maxButton->isVisible())) {
        parentWindow->showMaximized();
    }
}

void DAddonSplittedBarPrivate::_q_showMinimized()
{
    targetWindow()->showMinimized();
}

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
static Qt::WindowFlags &setWindowFlag(Qt::WindowFlags &flags, Qt::WindowType type, bool on)
{
    return on ? (flags |= type) : (flags &= ~int(type));
}
#endif

void DAddonSplittedBarPrivate::_q_onTopWindowMotifHintsChanged(quint32 winId)
{
    D_QC(DAddonSplittedBar);

    if (!DPlatformWindowHandle::isEnabledDXcb(targetWindow())) {
        q->disconnect(DWindowManagerHelper::instance(), SIGNAL(windowMotifWMHintsChanged(quint32)),
                      q, SLOT(_q_onTopWindowMotifHintsChanged(quint32)));
        return;
    }

    if (winId != q->window()->internalWinId()) {
        return;
    }

    DWindowManagerHelper::MotifDecorations decorations_hints = DWindowManagerHelper::getMotifDecorations(q->window()->windowHandle());
    DWindowManagerHelper::MotifFunctions functions_hints = DWindowManagerHelper::getMotifFunctions(q->window()->windowHandle());

    if (titleLabel) {
        titleLabel->setVisible(decorations_hints.testFlag(DWindowManagerHelper::DECOR_TITLE));
    }

    updateButtonsState(targetWindow()->windowFlags());

    minButton->setEnabled(functions_hints.testFlag(DWindowManagerHelper::FUNC_MINIMIZE));
    maxButton->setEnabled(functions_hints.testFlag(DWindowManagerHelper::FUNC_MAXIMIZE)
                          && functions_hints.testFlag(DWindowManagerHelper::FUNC_RESIZE));
    closeButton->setEnabled(functions_hints.testFlag(DWindowManagerHelper::FUNC_CLOSE));
    // sync button state
#if QT_VERSION >= QT_VERSION_CHECK(5, 7, 0)
    disableFlags.setFlag(Qt::WindowMinimizeButtonHint, !minButton->isEnabled());
    disableFlags.setFlag(Qt::WindowMaximizeButtonHint, !maxButton->isEnabled());
    disableFlags.setFlag(Qt::WindowCloseButtonHint, !closeButton->isEnabled());
#else
    setWindowFlag(disableFlags, Qt::WindowMinimizeButtonHint, !minButton->isEnabled());
    setWindowFlag(disableFlags, Qt::WindowMaximizeButtonHint, !maxButton->isEnabled());
    setWindowFlag(disableFlags, Qt::WindowCloseButtonHint, !closeButton->isEnabled());
#endif
}

#ifndef QT_NO_MENU

void DAddonSplittedBarPrivate::_q_addDefaultMenuItems()
{
    D_Q(DAddonSplittedBar);

    if (!menu) {
        q->setMenu(new QMenu(q));
    }
    if (!switchThemeMenu) {
        bool disableDtkSwitchThemeMenu = qEnvironmentVariableIsSet("KLU_DISABLE_MENU_THEME");
        if (!disableDtkSwitchThemeMenu) {
            switchThemeMenu = new QMenu(qApp->translate("TitleBarMenu", "Theme"), menu);
            lightThemeAction = switchThemeMenu->addAction(qApp->translate("TitleBarMenu", "Light Theme"));
            darkThemeAction = switchThemeMenu->addAction(qApp->translate("TitleBarMenu", "Dark Theme"));
            autoThemeAction = switchThemeMenu->addAction(qApp->translate("TitleBarMenu", "System Theme"));

            autoThemeAction->setCheckable(true);
            lightThemeAction->setCheckable(true);
            darkThemeAction->setCheckable(true);

            QActionGroup *group = new QActionGroup(switchThemeMenu);
            group->addAction(autoThemeAction);
            group->addAction(lightThemeAction);
            group->addAction(darkThemeAction);

            QObject::connect(group, SIGNAL(triggered(QAction*)),
                             q, SLOT(_q_switchThemeActionTriggered(QAction*)));

            menu->addMenu(switchThemeMenu);
            themeSeparator = menu->addSeparator();

            switchThemeMenu->menuAction()->setVisible(canSwitchTheme);
            themeSeparator->setVisible(canSwitchTheme);
        }
    }
    if (!helpAction && DApplicationPrivate::isUserManualExists()) {
        helpAction = new QAction(qApp->translate("TitleBarMenu", "Help"), menu);
        QObject::connect(helpAction, SIGNAL(triggered(bool)), q, SLOT(_q_helpActionTriggered()));
        menu->addAction(helpAction);
    }
    if (!aboutAction) {
        aboutAction = new QAction(qApp->translate("TitleBarMenu", "About"), menu);
        QObject::connect(aboutAction, SIGNAL(triggered(bool)), q, SLOT(_q_aboutActionTriggered()));
        menu->addAction(aboutAction);
    }
    if (!quitAction) {
        quitAction = new QAction(qApp->translate("TitleBarMenu", "Exit"), menu);
        QObject::connect(quitAction, SIGNAL(triggered(bool)), q, SLOT(_q_quitActionTriggered()));
        menu->addAction(quitAction);
    }
}

void DAddonSplittedBarPrivate::_q_helpActionTriggered()
{
    DAddonApplication *dapp = qobject_cast<DAddonApplication *>(qApp);
    if (dapp) {
        dapp->handleHelpAction();
    }
}

void DAddonSplittedBarPrivate::_q_aboutActionTriggered()
{
    DAddonApplication *dapp = qobject_cast<DAddonApplication *>(qApp);
    if (dapp) {
        dapp->handleAboutAction();
    }
}

void DAddonSplittedBarPrivate::_q_quitActionTriggered()
{
    DAddonApplication *dapp = qobject_cast<DAddonApplication *>(qApp);
    if (dapp) {
        dapp->handleQuitAction();
    }
}

void DAddonSplittedBarPrivate::_q_switchThemeActionTriggered(QAction *action)
{
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::UnknownType;

    if (action == lightThemeAction) {
        type = DGuiApplicationHelper::LightType;
    } else if (action == darkThemeAction) {
        type = DGuiApplicationHelper::DarkType;
    }

    DGuiApplicationHelper::instance()->setPaletteType(type);
}

void DAddonSplittedBarPrivate::setIconVisible(bool visible)
{
    if (iconLabel->isVisible() == visible)
        return;

    if (visible) {
        leftLayout->insertSpacing(0, 10);
        leftLayout->insertWidget(1, iconLabel, 0, Qt::AlignLeading | Qt::AlignVCenter);
        iconLabel->show();
    } else {
        iconLabel->hide();
        delete leftLayout->takeAt(0);
        delete leftLayout->takeAt(1);
    }
}

void DAddonSplittedBarPrivate::updateTabOrder()
{
    D_Q(DAddonSplittedBar);

    QList<QWidget *> orderWidget;
    QList<QHBoxLayout *> orderLayout;
    orderLayout << leftLayout << centerLayout << rightLayout;

    for (QHBoxLayout * lyt : orderLayout) {
        if (!lyt) {
            continue;
        }

        for (int i = 0; i < lyt->count(); ++i) {
            QWidget *wdg = lyt->itemAt(i)->widget();
            if (wdg && (wdg->focusPolicy() & Qt::FocusPolicy::TabFocus)) {
                orderWidget.append(wdg);
            }
        }
    }

    if (orderWidget.isEmpty()) {
        return;
    }

    QWidget::setTabOrder(q, orderWidget.first());
    for (int i = 0; i < orderWidget.count() - 1; ++i) {
        QWidget::setTabOrder(orderWidget.at(i), orderWidget.at(i + 1));
    }
}

#endif

DAddonSplittedBar::DAddonSplittedBar(QWidget *parent) :
    QFrame(parent),
    DObject(*new DAddonSplittedBarPrivate(this))
{
    if (DApplication::buildDtkVersion() < DTK_VERSION_CHECK(2, 0, 6, 1)) {
        setBackgroundTransparent(true);
    }

    D_D(DAddonSplittedBar);
    d->init();

    if (parent && parent->window()->windowType() != Qt::Window) {
        d->optionButton->hide();
    }
    this->setObjectName("DAddonSplittedBar");
    this->setIcon(qApp->windowIcon());
}

DAddonSplittedBar::~DAddonSplittedBar()
{
    if (logic != nullptr) {
        logic->~QTimer();
    }
}

void DAddonSplittedBar::slideDown()
{
    if (runingAnim == false) {
        runingAnim = true;
    logic = new QTimer;
    logic->setInterval(1);
    logic->start();
    connect(logic, &QTimer::timeout, this, [=]() {
        if (this->y() < 0) {
            this->move(this->x(), this->y() + hiddenAnimationSpeed);
        } else {
            if (this->y() > 0) {
                this->move(this->x(), 0);
            }
            D_D(DAddonSplittedBar);
            if (DAddonSplittedWindow *source = qobject_cast<DAddonSplittedWindow *>(d->targetWindow())) {
                source->updatePositions();
            }
            logic->~QTimer();
            runingAnim = false;
        }
    });}
}

void DAddonSplittedBar::slideUp()
{
    if (runingAnim == false) {
        runingAnim = true;
    logic = new QTimer;
    logic->setInterval(1);
    logic->start();
    connect(logic, &QTimer::timeout, this, [=]() {
        if (this->y() < hiddenVisibleArea - this->height()) {
            this->move(this->x(), this->y() - hiddenAnimationSpeed);
        } else {
            if (this->y() > hiddenVisibleArea - this->height()) {
                this->move(this->x(), hiddenVisibleArea - this->height());
            }
            D_D(DAddonSplittedBar);
            if (DAddonSplittedWindow *source = qobject_cast<DAddonSplittedWindow *>(d->targetWindow())) {
                source->updatePositions();
            }
            logic->~QTimer();
            runingAnim = false;
        }
    });}
}

QList<QWidget *> getWidgetTree(QWidget *source, QList<QWidget *> *list = new QList<QWidget *>())
{
    if (source) {
        list->append(source);
        if (source->parentWidget()) {
            getWidgetTree(source->parentWidget(), list);
        }
    }
    return *list;
}

QPoint mapToTop(QWidget *source, int skip = 0)
{
    QPoint point = source->pos();
    int i = 0;
    QList<QWidget *> list = getWidgetTree(source);
    while (i<list.length() - skip) {
        qDebug() << i+1 << "/" << list.length() - skip;
        point = list.at(i)->mapToParent(point);
        i++;
    }
    return point;
}

void DAddonSplittedBar::enterEvent(QEvent *e)
{
    QFrame::enterEvent(e);
    if (this->isHidden) {
        this->slideDown();
    }
}

void DAddonSplittedBar::leaveEvent(QEvent *e)
{
    QFrame::leaveEvent(e);
    if (this->isHidden) {
        this->slideUp();
    }
}

#ifndef QT_NO_MENU

QMenu *DAddonSplittedBar::menu() const
{
    D_DC(DAddonSplittedBar);
    return d->menu;
}

QMenu *DAddonSplittedBar::leftMenu() const
{
    D_DC(DAddonSplittedBar);
    return d->m_iconmenu;
}

void DAddonSplittedBar::setMenu(QMenu *menu)
{
    D_D(DAddonSplittedBar);

    d->menu = menu;
    if (d->menu) {
        disconnect(this, &DAddonSplittedBar::optionClicked, nullptr, nullptr);
        connect(this, &DAddonSplittedBar::optionClicked, this, &DAddonSplittedBar::showMenu);
    }
}

void DAddonSplittedBar::setLeftMenu(QMenu *menu)
{
    D_D(DAddonSplittedBar);

    d->m_iconmenu = menu;
    if (d->m_iconmenu) {
        disconnect(this, &DAddonSplittedBar::optionClicked, nullptr, nullptr);
        connect(this, &DAddonSplittedBar::optionClicked, this, &DAddonSplittedBar::showMenu);
    }
}

#endif

QWidget *DAddonSplittedBar::customWidget() const
{
    D_DC(DAddonSplittedBar);

    return d->customWidget;
}

#ifndef QT_NO_MENU

void DAddonSplittedBar::setCustomTitleAlign(Qt::Alignment a)
{
    D_D(DAddonSplittedBar);
    d->titleLabel->setAlignment(a);
}

void DAddonSplittedBar::showMenu()
{
    D_D(DAddonSplittedBar);

    if (d->menu) {
        if (d->switchThemeMenu) {
            QAction *action;

            switch (DGuiApplicationHelper::instance()->paletteType()) {
            case DGuiApplicationHelper::LightType:
                action = d->lightThemeAction;
                break;
            case DGuiApplicationHelper::DarkType:
                action = d->darkThemeAction;
                break;
            default:
                action = d->autoThemeAction;
                break;
            }

            action->setChecked(true);
        }

        d->menu->exec(d->optionButton->mapToGlobal(d->optionButton->rect().bottomLeft()));
        d->optionButton->update(); // FIX: bug-25253 sometimes optionButton not udpate after menu exec(but why?)
    }
}
#endif

void DAddonSplittedBar::showEvent(QShowEvent *event)
{
    D_D(DAddonSplittedBar);
    d->separatorTop->setFixedWidth(width());
    d->separatorTop->move(0, 0);
    d->separator->setFixedWidth(width());
    d->separator->move(0, height() - d->separator->height());

#ifndef QT_NO_MENU
    d->_q_addDefaultMenuItems();
#endif

    QWidget::showEvent(event);

    if (DPlatformWindowHandle::isEnabledDXcb(window())) {
        d->_q_onTopWindowMotifHintsChanged(
            static_cast<quint32>(window()->internalWinId()));
    }

    d->updateCenterArea();
}

void DAddonSplittedBar::mousePressEvent(QMouseEvent *event)
{
    D_D(DAddonSplittedBar);
    d->mousePressed = (event->button() == Qt::LeftButton);

    if (event->button() == Qt::RightButton) {
        DWindowManagerHelper::popupSystemWindowMenu(window()->windowHandle());
        return;
    }

#ifdef DTK_TITLE_DRAG_WINDOW
    Q_EMIT mousePosPressed(event->buttons(), event->globalPos());
#endif
    Q_EMIT mousePressed(event->buttons());
}

void DAddonSplittedBar::mouseReleaseEvent(QMouseEvent *event)
{
    D_D(DAddonSplittedBar);
    if (event->button() == Qt::LeftButton) {
        d->mousePressed = false;
    }
}

bool DAddonSplittedBar::eventFilter(QObject *obj, QEvent *event)
{
    D_D(DAddonSplittedBar);

    if (event->type() == QEvent::MouseButtonPress &&
            static_cast<QMouseEvent *>(event)->button() == Qt::RightButton &&
            (obj ==d->minButton ||
             obj == d->maxButton ||
             obj == d->closeButton ||
             obj == d->optionButton ||
             obj == d->quitFullButton))
    {
        event->accept(); // button on titlebar should not show kwin menu
        return true;
    }

    if (obj == d->targetWindow()) {
        switch (event->type()) {
        case QEvent::ShowToParent:
            d->handleParentWindowIdChange();
            d->updateButtonsState(d->targetWindow()->windowFlags());
            break;
        case QEvent::Resize:
            if (d->autoHideOnFullscreen) {
                setFixedWidth(d->targetWindow()->width());
            }
            break;
        case QEvent::HoverMove: {
            auto mouseEvent = reinterpret_cast<QMouseEvent *>(event);
            bool isFullscreen = d->targetWindow()->windowState().testFlag(Qt::WindowFullScreen);
            if (isFullscreen && d->autoHideOnFullscreen) {
                if (mouseEvent->pos().y() > height() && d->isVisableOnFullscreen()) {
                    d->hideOnFullscreen();
                }
                if (mouseEvent->pos().y() < 2) {
                    d->showOnFullscreen();
                }
            }
            break;
        }
        case QEvent::WindowStateChange: {
            d->handleParentWindowStateChange();
            break;
        }
        default:
            break;
        }
    }
    return QWidget::eventFilter(obj, event);
}

QColor DAddonSplittedBar::getColor()
{
    D_D(DAddonSplittedBar);

    if (!d->blurWidget->isVisible()) {
        return palette().color(backgroundRole());
    } else {
        return d->blurWidget->maskColor();
    }
}

bool DAddonSplittedBar::event(QEvent *e)
{
    D_D(DAddonSplittedBar);
    if (e->type() == QEvent::LayoutRequest) {
        D_D(DAddonSplittedBar);

        d->updateCenterArea();
    } elif (e->type() == QEvent::Paint) {
        if (d->blurWidget) {
            QPainter *p = new QPainter(this);
            QColor col = d->blurWidget->maskColor();
            p->setOpacity(1);
            p->fillRect(QRect(0,0, left_margin, this->height()), QBrush(QColor(col.red(), col.green(), col.blue(), 150)));
            p->~QPainter();
        }
    }
    return QFrame::event(e);
}

void DAddonSplittedBar::resizeEvent(QResizeEvent *event)
{
    D_D(DAddonSplittedBar);

    d->separatorTop->setFixedWidth(event->size().width());
    d->separator->setFixedWidth(event->size().width());
    d->updateCenterArea();

    if (d->blurWidget) {
        d->blurWidget->resize(event->size().width() - left_margin, event->size().height());
        if (isFirstMarginSet == true) {
            d->blurWidget->move(left_margin, 0);
        }
    }

    if (d->targetWindow()->windowState().testFlag(Qt::WindowFullScreen) || d->targetWindow()->window()->isMaximized()) {
        if (this->isHidden) {
            Q_EMIT setupMaximizedSettings();
        }
    }

    return QWidget::resizeEvent(event);
}

int DAddonSplittedBar::leftMargin()
{
    return left_margin;
}

void DAddonSplittedBar::setCustomWidget(QWidget *w, bool fixCenterPos)
{
    D_D(DAddonSplittedBar);

    if (w == d->customWidget) {
        return;
    }

    if (d->customWidget) {
        d->mainLayout->removeWidget(d->customWidget);
        d->customWidget->hide();
        d->customWidget->deleteLater();
    }

    d->customWidget = w;

    if (w) {
        w->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);
    } else {
        d->centerArea->show();
        d->titleLabel = d->centerArea;

        return;
    }

    if (fixCenterPos) {
        for (int i = 0; i < d->centerLayout->count(); ++i) {
            delete d->centerLayout->itemAt(i);
        }

        addWidget(w, Qt::Alignment());
        d->centerArea->show();
        d->titleLabel = d->centerArea;
    } else {
        d->mainLayout->insertWidget(1, w);
        d->titleLabel = nullptr;
        d->centerArea->hide();
    }
}

void DAddonSplittedBar::addWidget(QWidget *w, Qt::Alignment alignment)
{
    D_D(DAddonSplittedBar);

    if (alignment & Qt::AlignLeft) {
        d->leftLayout->addWidget(w, 0, alignment & ~Qt::AlignLeft);
    } else if (alignment & Qt::AlignRight) {
        d->rightLayout->addWidget(w, 0, alignment & ~Qt::AlignRight);
    } else {
        d->centerLayout->addWidget(w, 0, alignment);
        d->centerArea->clear();
        d->titleLabel = nullptr;
    }

    updateGeometry();
    d->updateTabOrder();
}

void DAddonSplittedBar::removeWidget(QWidget *w)
{
    D_D(DAddonSplittedBar);

    d->leftLayout->removeWidget(w);
    d->centerLayout->removeWidget(w);
    d->rightLayout->removeWidget(w);

    if (d->centerLayout->isEmpty()) {
        d->titleLabel = d->centerArea;
        d->titleLabel->setText(d->targetWindowHandle->title());
    }

    updateGeometry();
    d->updateTabOrder();
}

void DAddonSplittedBar::setFixedHeight(int h)
{
    QWidget::setFixedHeight(h);
}

void DAddonSplittedBar::setBackgroundTransparent(bool transparent)
{
    setAutoFillBackground(!transparent);

    if (transparent)
        setBackgroundRole(QPalette::NoRole);
    else
        setBackgroundRole(QPalette::Base);
}

void DAddonSplittedBar::setSeparatorVisible(bool visible)
{
    D_D(DAddonSplittedBar);
    if (visible) {
        d->separator->show();
        d->separator->raise();
    } else {
        d->separator->hide();
    }
}

void DAddonSplittedBar::setTitle(const QString &title)
{
    D_D(DAddonSplittedBar);
    if (d->titleLabel && !d->embedMode) {
        d->titleLabel->setText(title);
    } else if (parentWidget()) {
        parentWidget()->setWindowTitle(title);
    }
}

void DAddonSplittedBar::setIcon(const QIcon &icon)
{
    D_D(DAddonSplittedBar);
    if (!d->embedMode) {
        d->iconLabel->setIcon(icon);
        d->setIconVisible(!icon.isNull());
    } else if (parentWidget()) {
        d->setIconVisible(false);
        parentWidget()->setWindowIcon(icon);
    }
}

void DAddonSplittedBar::toggleWindowState()
{
    D_D(DAddonSplittedBar);

    d->_q_toggleWindowState();
}

void DAddonSplittedBar::setBlurBackground(bool blurBackground)
{
    D_D(DAddonSplittedBar);

    if (static_cast<bool>(d->blurWidget) == blurBackground)
        return;

    if (d->blurWidget) {
        d->blurWidget->hide();
        d->blurWidget->deleteLater();
        d->blurWidget = nullptr;
    } else {
        d->blurWidget = new DBlurEffectWidget(this);
        d->blurWidget->lower();
        d->blurWidget->resize(size());
        d->blurWidget->setMaskColor(DBlurEffectWidget::AutoColor);
        d->blurWidget->setRadius(20);
        d->blurWidget->show();
    }

    setAutoFillBackground(!blurBackground);
}

int DAddonSplittedBar::buttonAreaWidth() const
{
    D_DC(DAddonSplittedBar);
    return d->buttonArea->width();
}

bool DAddonSplittedBar::separatorVisible() const
{
    D_DC(DAddonSplittedBar);
    return d->separator->isVisible();
}

bool DAddonSplittedBar::autoHideOnFullscreen() const
{
    D_DC(DAddonSplittedBar);
    return d->autoHideOnFullscreen;
}

void DAddonSplittedBar::setAutoHideOnFullscreen(bool autohide)
{
    D_D(DAddonSplittedBar);
    d->autoHideOnFullscreen = autohide;
}

void DAddonSplittedBar::setVisible(bool visible)
{
    D_D(DAddonSplittedBar);

    if (visible == isVisible()) {
        return;
    }

    QWidget::setVisible(visible);

    if (visible) {
        if (!d->targetWindow()) {
            return;
        }
        d->targetWindow()->installEventFilter(this);

        connect(d->maxButton, SIGNAL(clicked()), this, SLOT(_q_toggleWindowState()));
        connect(this, SIGNAL(doubleClicked()), this, SLOT(_q_toggleWindowState()));
        connect(d->minButton, SIGNAL(clicked()), this, SLOT(_q_showMinimized()));
        connect(d->closeButton, &DWindowCloseButton::clicked, d->targetWindow(), &QWidget::close);

        d->updateButtonsState(d->targetWindow()->windowFlags());
    } else {
        if (!d->targetWindow()) {
            return;
        }

        d->targetWindow()->removeEventFilter(this);

        disconnect(d->maxButton, SIGNAL(clicked()), this, SLOT(_q_toggleWindowState()));
        disconnect(this, SIGNAL(doubleClicked()), this, SLOT(_q_toggleWindowState()));
        disconnect(d->minButton, SIGNAL(clicked()), this, SLOT(_q_showMinimized()));
        disconnect(d->closeButton, &DWindowCloseButton::clicked, d->targetWindow(), &QWidget::close);
    }
}

void DAddonSplittedBar::setEmbedMode(bool visible)
{
    D_D(DAddonSplittedBar);
    d->embedMode = visible;
    d->separatorTop->setVisible(visible);
    d->updateButtonsState(windowFlags());
}

bool DAddonSplittedBar::menuIsVisible() const
{
    D_DC(DAddonSplittedBar);
    return !d->optionButton->isVisible();
}

void DAddonSplittedBar::setMenuVisible(bool visible)
{
    D_D(DAddonSplittedBar);
    d->optionButton->setVisible(visible);
}

bool DAddonSplittedBar::menuIsDisabled() const
{
    D_DC(DAddonSplittedBar);
    return !d->optionButton->isEnabled();
}

void DAddonSplittedBar::setMenuDisabled(bool disabled)
{
    D_D(DAddonSplittedBar);
    d->optionButton->setDisabled(disabled);
}

bool DAddonSplittedBar::quitMenuIsDisabled() const
{
    D_DC(DAddonSplittedBar);

    return d->quitAction && !d->quitAction->isEnabled();
}

void DAddonSplittedBar::setQuitMenuDisabled(bool disabled)
{
    D_D(DAddonSplittedBar);

    if (!d->quitAction) {
        d->_q_addDefaultMenuItems();
    }

    d->quitAction->setEnabled(!disabled);
}

void DAddonSplittedBar::setQuitMenuVisible(bool visible)
{
    D_D(DAddonSplittedBar);

    if (!d->quitAction) {
        d->_q_addDefaultMenuItems();
    }

    d->quitAction->setVisible(visible);
}

bool DAddonSplittedBar::switchThemeMenuIsVisible() const
{
    D_DC(DAddonSplittedBar);

    return d->switchThemeMenu;
}

bool DAddonSplittedBar::isAutoHidden() const
{
    return isHidden;
}

void DAddonSplittedBar::setSwitchThemeMenuVisible(bool visible)
{
    D_D(DAddonSplittedBar);

    if (visible == d->canSwitchTheme) {
        return;
    }

    d->canSwitchTheme = visible;

    if (d->switchThemeMenu) {
        d->switchThemeMenu->menuAction()->setVisible(visible);
        d->themeSeparator->setVisible(visible);
    }
}

void DAddonSplittedBar::setDisableFlags(Qt::WindowFlags flags)
{
    D_D(DAddonSplittedBar);
    d->disableFlags = flags;
    d->updateButtonsFunc();
}

Qt::WindowFlags DAddonSplittedBar::disableFlags() const
{
    D_DC(DAddonSplittedBar);
    return d->disableFlags;
}

QSize DAddonSplittedBar::sizeHint() const
{
    D_DC(DAddonSplittedBar);

    if (d->centerArea->isHidden()) {
        return QFrame::sizeHint();
    }

    int padding = qMax(d->leftArea->sizeHint().width(), d->rightArea->sizeHint().width());
    int width = d->centerArea->sizeHint().width() + 2 * d->mainLayout->spacing() + 2 * padding;

    return QSize(width, DefaultTitlebarHeight);
}

QSize DAddonSplittedBar::minimumSizeHint() const
{
    return sizeHint();
}

bool DAddonSplittedBar::blurBackground() const
{
    D_DC(DAddonSplittedBar);
    return d->blurWidget;
}

void DAddonSplittedBar::mouseMoveEvent(QMouseEvent *event)
{
    D_D(DAddonSplittedBar);

    Qt::MouseButton button = event->buttons() & Qt::LeftButton ? Qt::LeftButton : Qt::NoButton;
    if (event->buttons() == Qt::LeftButton /*&& d->mousePressed*/) {
        if (!d->mousePressed) {
            return;
        }
        Q_EMIT mouseMoving(button);
    }

#ifdef DTK_TITLE_DRAG_WINDOW
    D_D(DAddonSplittedBar);
    if (d->mousePressed) {
        Q_EMIT mousePosMoving(button, event->globalPos());
    }
#endif
    QWidget::mouseMoveEvent(event);
}

void DAddonSplittedBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    D_D(DAddonSplittedBar);

    if (event->buttons() == Qt::LeftButton) {
        d->mousePressed = false;
        Q_EMIT doubleClicked();
    }
}

void DAddonSplittedBar::setLeftMargin(int margin)
{
    D_D(DAddonSplittedBar);
    left_margin = margin;
    d->leftArea->setFixedWidth(margin);
    this->update();
    if (d->blurWidget != nullptr) {
        d->blurWidget->move(left_margin, 0);
    }
}

QHBoxLayout *DAddonSplittedBar::leftLayout() const
{
    D_DC(DAddonSplittedBar);
    return d->leftLayout;
}


QHBoxLayout *DAddonSplittedBar::centerLayout() const
{
    D_DC(DAddonSplittedBar);
    return d->centerLayout;
}

QHBoxLayout *DAddonSplittedBar::rightLayout() const
{
    D_DC(DAddonSplittedBar);
    return d->rightLayout;
}

#ifndef QT_NO_MENU

void DAddonSplittedBar::setIconMenu(QMenu *menu)
{
    m_iconMenu = menu;
}

QMenu *DAddonSplittedBarPrivate::automateIconMenu()
{
    if (m_iconmenu == nullptr) {
        m_iconmenu = new QMenu;
        m_iconmenu->addAction("Hello!");
        m_iconmenu->addAction("Devs forgot");
        m_iconmenu->addAction("Me ;[");
        m_iconmenu->addAction("Am I an easter egg??");
    }
    return m_iconmenu;
}

void DAddonSplittedBar::showIconMenu()
{
    D_D(DAddonSplittedBar);
    m_iconMenu->exec(mapToGlobal(QPoint(d->iconLabel->rect().bottomLeft().x() +5, this->height())));
}

#endif

LDA_END_NAMESPACE
