
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

#include "dsplitedbar.h"
#include "dsplitedwindow.h"
#include "daddonapplication.h"
#include "private/dapplication_p.h"

#include <QDebug>
#include <QMenu>
#include <QHBoxLayout>
#include <QApplication>
#include <QMouseEvent>
#include <QProcess>
#include <QTimer>

#include <DWindowManagerHelper>

LDA_BEGIN_NAMESPACE

const int DefaultTitlebarHeight = 50;
const int DefaultIconHeight = 32;
const int DefaultIconWidth = 32;

DSplitedBarPrivate::DSplitedBarPrivate(DSplitedBar *qq): DObjectPrivate(qq)
{
}

void DSplitedBarPrivate::init()
{
    D_Q(DSplitedBar);

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
    quitFullButton  = new DImageButton;
    separatorTop    = new DHorizontalLine(q);
    separator       = new DHorizontalLine(q);
    titleLabel      = centerArea;
    titleLabel->setElideMode(Qt::ElideMiddle);

    minButton->installEventFilter(q);
    maxButton->installEventFilter(q);
    closeButton->installEventFilter(q);
    optionButton->installEventFilter(q);
    quitFullButton->installEventFilter(q);

    optionButton->setObjectName("DSplitedBarDWindowOptionButton");
    optionButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    minButton->setObjectName("DSplitedBarDWindowMinButton");
    minButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    maxButton->setObjectName("DSplitedBarDWindowMaxButton");
    maxButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    closeButton->setObjectName("DSplitedBarDWindowCloseButton");
    closeButton->setIconSize(QSize(DefaultTitlebarHeight, DefaultTitlebarHeight));
    quitFullButton->setObjectName("DSplitedBarDWindowQuitFullscreenButton");
    quitFullButton->hide();

    iconLabel->setIconSize(QSize(DefaultIconWidth, DefaultIconHeight));
    iconLabel->setWindowFlags(Qt::WindowTransparentForInput);
    iconLabel->setFocusPolicy(Qt::NoFocus);
    iconLabel->setFlat(true);
    // 默认无图标，所以隐藏
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
    centerArea->setMaximumWidth(q->left_margin);

    buttonArea->setWindowFlag(Qt::WindowTransparentForInput);
    buttonArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    QHBoxLayout *buttonLayout = new QHBoxLayout(buttonArea);
    buttonLayout->setContentsMargins(0, 0, 0, 0);
    buttonLayout->setSpacing(0);
    buttonLayout->addWidget(optionButton);
    buttonLayout->addWidget(minButton);
    buttonLayout->addWidget(maxButton);
    buttonLayout->addWidget(closeButton);
    buttonLayout->addWidget(quitFullButton);

    rightArea->setWindowFlag(Qt::WindowTransparentForInput);
    rightArea->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Expanding);
    rightLayout->setContentsMargins(0, 0, 0, 0);
    auto rightAreaLayout = new QHBoxLayout(rightArea);
    rightAreaLayout->setContentsMargins(0, 0, 0, 0);
    rightAreaLayout->setMargin(0);
    rightAreaLayout->setSpacing(0);
    rightAreaLayout->addLayout(rightLayout);
    rightAreaLayout->addWidget(buttonArea);

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

    q->connect(quitFullButton, &DImageButton::clicked, q, [ = ]() {
        bool isFullscreen = targetWindow()->windowState().testFlag(Qt::WindowFullScreen);
        if (isFullscreen) {
            targetWindow()->showNormal();
        } else {
            targetWindow()->showFullScreen();
        }
    });
    q->connect(optionButton, &DWindowOptionButton::clicked, q, &DSplitedBar::optionClicked);
    q->connect(DWindowManagerHelper::instance(), SIGNAL(windowMotifWMHintsChanged(quint32)),
               q, SLOT(_q_onTopWindowMotifHintsChanged(quint32)));

    q->setFrameShape(QFrame::NoFrame);
    q->setBackgroundRole(QPalette::Base);
    q->setAutoFillBackground(true);
    q->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
    // 确保点击标题栏后输入框能失去焦点
    // 另外，让标题栏接收焦点，还是为了避免一个focus控件隐藏时，会把焦点转移给标题栏上的按钮控件
    q->setFocusPolicy(Qt::StrongFocus);

#ifndef QT_NO_MENU
    q->setIconMenu(this->automateIconMenu());
    QObject::connect(iconLabel, &DIconButton::clicked, q, &DSplitedBar::showIconMenu);
#endif
}

QWidget *DSplitedBarPrivate::targetWindow()
{
    D_Q(DSplitedBar);
    return q->topLevelWidget()->window();
}

bool DSplitedBarPrivate::isVisableOnFullscreen()
{
    D_Q(DSplitedBar);
    return !q->property("_restore_height").isValid();
}

void DSplitedBarPrivate::hideOnFullscreen()
{
    D_Q(DSplitedBar);
    if (q->height() > 0) {
        q->setProperty("_restore_height", q->height());
    }
    q->setFixedHeight(0);
}

void DSplitedBarPrivate::showOnFullscreen()
{
    D_Q(DSplitedBar);
    if (q->property("_restore_height").isValid()) {
        q->setFixedHeight(q->property("_restore_height").toInt());
        q->setProperty("_restore_height", QVariant());
    }
}

void DSplitedBarPrivate::updateFullscreen()
{
    D_Q(DSplitedBar);

    if (!autoHideOnFullscreen) {
        return;
    }

    bool isFullscreen = targetWindow()->windowState().testFlag(Qt::WindowFullScreen);
    auto mainWindow = qobject_cast<DSplitedWindow *>(targetWindow());
    if (!isFullscreen) {
        quitFullButton->hide();
        showOnFullscreen();
    } else {
        // must set to empty
        quitFullButton->show();

        q->setParent(mainWindow);
        q->show();
        hideOnFullscreen();
    }
}

void DSplitedBarPrivate::updateButtonsState(Qt::WindowFlags type)
{
    D_Q(DSplitedBar);
    bool useDXcb = DPlatformWindowHandle::isEnabledDXcb(targetWindow());
    bool isFullscreen = targetWindow()->windowState().testFlag(Qt::WindowFullScreen);

//    bool forceShow = !useDXcb;
    bool forceShow = false;
#ifndef Q_OS_LINUX
    forceShow = false;
#endif

    bool showTitle = (type.testFlag(Qt::WindowTitleHint) || forceShow) && !embedMode;
    if (titleLabel) {
        titleLabel->setVisible(showTitle);
    }

    // Never show in embed/fullscreen
    bool forceHide = (!useDXcb) || embedMode || isFullscreen;

    bool showMin = (type.testFlag(Qt::WindowMinimizeButtonHint) || forceShow) && !forceHide;
    minButton->setVisible(showMin);


    bool allowResize = true;

    if (q->window() && q->window()->windowHandle()) {
        auto functions_hints = DWindowManagerHelper::getMotifFunctions(q->window()->windowHandle());
        allowResize = functions_hints.testFlag(DWindowManagerHelper::FUNC_RESIZE);
    }

    bool showMax = (type.testFlag(Qt::WindowMaximizeButtonHint) || forceShow) && !forceHide && allowResize;
//    qDebug() << "max:"
//             << "allowResize" << allowResize
//             << "useDXcb" << useDXcb
//             << "forceHide" << forceHide
//             << "type.testFlag(Qt::WindowMaximizeButtonHint)" << type.testFlag(Qt::WindowMaximizeButtonHint);
    maxButton->setVisible(showMax);

    bool showClose = (type.testFlag(Qt::WindowCloseButtonHint) || forceShow) && !forceHide;
    closeButton->setVisible(showClose);
}

void DSplitedBarPrivate::updateButtonsFunc()
{
    // TASK-18145 (bug-17474) do not setMotifFunctions on wayland
    if (!targetWindowHandle || !qgetenv("WAYLAND_DISPLAY").isEmpty()) {
        return;
    }

    // 根据 disableFlags 更新窗口标志，而标题栏上具体按钮的开启/禁用状态只根据
    // 窗口标志改变后做更新，且窗口标志改变后也会同步更新 disableFlags 的值，
    // 也就是说，实际上窗口按钮的状态只受窗口标志影响，而 setDisableFlags
    // 只是单纯的记录新的 disableFlags 的值，由于调用 setDisableFlags
    // 是应用中的主动行为，所以同时也会根据这个值更新窗口标志。
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

void DSplitedBarPrivate::updateCenterArea()
{
    D_QC(DSplitedBar);

    if (centerArea->isHidden()) {
        return;
    }

    int padding = qMax(leftArea->width(), rightArea->width());
    QRect rect(0, 0, q->width() - 2 * padding, q->height());
    rect.moveCenter(q->rect().center());
    centerArea->setGeometry(rect);
}

void DSplitedBarPrivate::handleParentWindowStateChange()
{
    maxButton->setMaximized(targetWindow()->windowState() == Qt::WindowMaximized);
    updateFullscreen();
    updateButtonsState(targetWindow()->windowFlags());
}

//!
//! \brief DSplitedBarPrivate::handleParentWindowIdChange
//! Them WindowStateChnage Event will miss some state changed message,
//! So use windowHandle::windowStateChanged instead
void DSplitedBarPrivate::handleParentWindowIdChange()
{
    if (!targetWindowHandle) {
        targetWindowHandle = targetWindow()->windowHandle();

        updateButtonsFunc();
    } else if (targetWindow()->windowHandle() != targetWindowHandle) {
        // Parent change???, show never here
        qWarning() << "targetWindowHandle change" << targetWindowHandle << targetWindow()->windowHandle();
    }
}

void DSplitedBarPrivate::_q_toggleWindowState()
{
    QWidget *parentWindow = targetWindow();

    if (!parentWindow || disableFlags.testFlag(Qt::WindowMaximizeButtonHint)) {
        return;
    }

    if (parentWindow->isMaximized()) {
        parentWindow->showNormal();
    } else if (!parentWindow->isFullScreen()
               && (maxButton->isVisible())) {
        parentWindow->showMaximized();
    }
}

void DSplitedBarPrivate::_q_showMinimized()
{
    targetWindow()->showMinimized();
}

#if QT_VERSION < QT_VERSION_CHECK(5, 7, 0)
static Qt::WindowFlags &setWindowFlag(Qt::WindowFlags &flags, Qt::WindowType type, bool on)
{
    return on ? (flags |= type) : (flags &= ~int(type));
}
#endif

void DSplitedBarPrivate::_q_onTopWindowMotifHintsChanged(quint32 winId)
{
    D_QC(DSplitedBar);

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

void DSplitedBarPrivate::_q_addDefaultMenuItems()
{
    D_Q(DSplitedBar);

    if (!menu) {
        q->setMenu(new QMenu(q));
    }

    // add switch theme sub menu
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

    // add help menu item.
    if (!helpAction && DApplicationPrivate::isUserManualExists()) {
        helpAction = new QAction(qApp->translate("TitleBarMenu", "Help"), menu);
        QObject::connect(helpAction, SIGNAL(triggered(bool)), q, SLOT(_q_helpActionTriggered()));
        menu->addAction(helpAction);
    }

    // add about menu item.
    if (!aboutAction) {
        aboutAction = new QAction(qApp->translate("TitleBarMenu", "About"), menu);
        QObject::connect(aboutAction, SIGNAL(triggered(bool)), q, SLOT(_q_aboutActionTriggered()));
        menu->addAction(aboutAction);
    }

    // add quit menu item.
    if (!quitAction) {
        quitAction = new QAction(qApp->translate("TitleBarMenu", "Exit"), menu);
        QObject::connect(quitAction, SIGNAL(triggered(bool)), q, SLOT(_q_quitActionTriggered()));
        menu->addAction(quitAction);
    }
}

void DSplitedBarPrivate::_q_helpActionTriggered()
{
    DAddonApplication *dapp = qobject_cast<DAddonApplication *>(qApp);
    if (dapp) {
        dapp->handleHelpAction();
    }
}

void DSplitedBarPrivate::_q_aboutActionTriggered()
{
    DAddonApplication *dapp = qobject_cast<DAddonApplication *>(qApp);
    if (dapp) {
        dapp->handleAboutAction();
    }
}

void DSplitedBarPrivate::_q_quitActionTriggered()
{
    DAddonApplication *dapp = qobject_cast<DAddonApplication *>(qApp);
    if (dapp) {
        dapp->handleQuitAction();
    }
}

void DSplitedBarPrivate::_q_switchThemeActionTriggered(QAction *action)
{
    DGuiApplicationHelper::ColorType type = DGuiApplicationHelper::UnknownType;

    if (action == lightThemeAction) {
        type = DGuiApplicationHelper::LightType;
    } else if (action == darkThemeAction) {
        type = DGuiApplicationHelper::DarkType;
    }

    DGuiApplicationHelper::instance()->setPaletteType(type);
}

void DSplitedBarPrivate::setIconVisible(bool visible)
{
    if (iconLabel->isVisible() == visible)
        return;

    if (visible) {
        leftLayout->insertSpacing(0, 10);
        leftLayout->insertWidget(1, iconLabel, 0, Qt::AlignLeading | Qt::AlignVCenter);
        iconLabel->show();
    } else {
        iconLabel->hide();
        // 从布局中移除图标相关的东西
        delete leftLayout->takeAt(0);
        delete leftLayout->takeAt(1);
    }
}

void DSplitedBarPrivate::updateTabOrder()
{
    D_Q(DSplitedBar);

    QList<QWidget *> orderWidget;
    QList<QHBoxLayout *> orderLayout;
    orderLayout << leftLayout << centerLayout << rightLayout;

    //查找 leftLayout、centerLayout、rightLayout 三个区域中有 TabFocus 属性的 widget
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

    //对筛选出来的 widget 重新设置 taborder
    QWidget::setTabOrder(q, orderWidget.first());
    for (int i = 0; i < orderWidget.count() - 1; ++i) {
        QWidget::setTabOrder(orderWidget.at(i), orderWidget.at(i + 1));
    }
}

#endif

/*!
 * \~english \class DSplitedBar
 * \brief The DSplitedBar class is an universal title bar on the top of windows.
 * \param parent is the parent widget to be attached on.
 *
 * Usually you don't need to construct a DSplitedBar instance by your self, you
 * can get an DSplitedBar instance by DMainWindow::titlebar .
 */
/*!
 * \~chinese \class DSplitedBar
 * \brief DSplitedBar是Dtk程序通用的标题栏组件，用于实现标题栏的高度定制化。
 * \param 父组件，一般为标题栏所在的窗口
 *
 * 一般情况下，请使用Dtk::Widget::DMainWindow::titlebar()来获取已经自动初始化的标题栏，
 * 不要自己来创建这个标题栏。
 */


/*!
 *  \~english @brief DSplitedBar::DSplitedBar create an default widget with icon/title/and buttons
 * @param parent
 */
/*!
 *  \~chinese @brief 创建一个DSplitedBar对象，包含默认的窗口按钮。
 */
DSplitedBar::DSplitedBar(QWidget *parent) :
    QFrame(parent),
    DObject(*new DSplitedBarPrivate(this))
{
    if (DApplication::buildDtkVersion() < DTK_VERSION_CHECK(2, 0, 6, 1)) {
        setBackgroundTransparent(true);
    }

    D_D(DSplitedBar);
    d->init();

    // 默认只在普通窗口中显示窗口菜单按钮
    if (parent && parent->window()->windowType() != Qt::Window) {
        d->optionButton->hide();
    }

    this->setIcon(qApp->windowIcon());
}

#ifndef QT_NO_MENU
/*!
 * \~english @brief DSplitedBar::menu holds the QMenu object attached to this title bar.
 * @return the QMenu object it holds, returns null if there's no one set.
 */
/*!
 * \~chinese @brief 获取和标题栏关联的应用查询菜单。
 * @return 如该标题栏没有设置菜单，这里会返回空，但是如该使用 Dtk::Widget::DApplication ,
 * 那么这里一般会自动创建一个程序菜单。
 */
QMenu *DSplitedBar::menu() const
{
    D_DC(DSplitedBar);

    return d->menu;
}

QMenu *DSplitedBar::leftMenu() const
{
    D_DC(DSplitedBar);

    return d->m_iconmenu;
}

/*!
 * \~english @brief DSplitedBar::setMenu attaches a QMenu object to the title bar.
 * @param menu is the target menu.
 */
/*!
 * \~chinese @brief 设置自定义的程序菜单
 * @param 需要被设置的菜单
 */
void DSplitedBar::setMenu(QMenu *menu)
{
    D_D(DSplitedBar);

    d->menu = menu;
    if (d->menu) {
        disconnect(this, &DSplitedBar::optionClicked, nullptr, nullptr);
        connect(this, &DSplitedBar::optionClicked, this, &DSplitedBar::showMenu);
    }
}

void DSplitedBar::setLeftMenu(QMenu *menu)
{
    D_D(DSplitedBar);

    d->m_iconmenu = menu;
    if (d->m_iconmenu) {
        disconnect(this, &DSplitedBar::optionClicked, nullptr, nullptr);
        connect(this, &DSplitedBar::optionClicked, this, &DSplitedBar::showMenu);
    }
}

#endif

/*!
 * \~english @brief DSplitedBar::customWidget
 * @return the customized widget used in this title bar.
 *
 * One can set customized widget to show some extra widgets on the title bar.
 * \see Dtk::Widget::DSplitedBar::setCustomWidget()
 */
/*!
 * \~chinese @brief 标题栏绑定的自定义控件
 * @return 自定义控件
 *
 * 可以通过自定义控件来在标题栏上显示复杂的组合控件
 *
 * \see Dtk::Widget::DSplitedBar::setCustomWidget()
 */
QWidget *DSplitedBar::customWidget() const
{
    D_DC(DSplitedBar);

    return d->customWidget;
}

#ifndef QT_NO_MENU
/*!
 * \~english \brief DSplitedBar::showMenu pop the menu of application on titlebar.
 */
/*!
 * \~chineses \brief 弹出应用程序菜单
 */

void DSplitedBar::setCustomTitleAlign(Qt::Alignment a)
{
    D_D(DSplitedBar);
    d->titleLabel->setAlignment(a);
}

void DSplitedBar::showMenu()
{
    D_D(DSplitedBar);

    if (d->menu) {
        // 更新主题选中的项
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

void DSplitedBar::showEvent(QShowEvent *event)
{
    //fix the width issue and process menu
    D_D(DSplitedBar);
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

void DSplitedBar::mousePressEvent(QMouseEvent *event)
{
    D_D(DSplitedBar);
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

void DSplitedBar::mouseReleaseEvent(QMouseEvent *event)
{
    D_D(DSplitedBar);
    if (event->button() == Qt::LeftButton) {
        d->mousePressed = false;
    }
}

bool DSplitedBar::eventFilter(QObject *obj, QEvent *event)
{
    D_D(DSplitedBar);

    if (event->type() == QEvent::MouseButtonPress &&
            static_cast<QMouseEvent *>(event)->button() == Qt::RightButton &&
            (obj ==d->minButton || obj == d->maxButton ||
            obj == d->closeButton || obj == d->optionButton ||
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

bool DSplitedBar::event(QEvent *e)
{
    if (e->type() == QEvent::LayoutRequest) {
        D_D(DSplitedBar);

        d->updateCenterArea();
    }

    return QFrame::event(e);
}

void DSplitedBar::resizeEvent(QResizeEvent *event)
{
    //override QWidget::resizeEvent to fix button and separator pos.
    D_D(DSplitedBar);

    d->separatorTop->setFixedWidth(event->size().width());
    d->separator->setFixedWidth(event->size().width());
    d->updateCenterArea();

    if (d->blurWidget) {
        d->blurWidget->resize(event->size().width() - left_margin, event->size().height());
        if (isFirstMarginSet == true) {
            d->blurWidget->move(left_margin, 0);
        }
    }

    return QWidget::resizeEvent(event);
}

/*!
 * \~english @brief DSplitedBar::setCustomWidget is an overloaded function.
 * @param w is the widget to be used as the customize widget shown in the title
 * bar.
 * @param fixCenterPos indicates whether it should automatically move the
 * customize widget to the horizontal center of the title bar or not.
 */
/*!
 * \~chinese @brief 设置标题栏上的自定义控件
 * @param w 需要显示的控件。
 * @param fixCenterPos 是否需要自动修正控件位置，用于保持控件居中显示。
 */
void DSplitedBar::setCustomWidget(QWidget *w, bool fixCenterPos)
{
    D_D(DSplitedBar);

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

void DSplitedBar::addWidget(QWidget *w, Qt::Alignment alignment)
{
    D_D(DSplitedBar);

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

void DSplitedBar::removeWidget(QWidget *w)
{
    D_D(DSplitedBar);

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

/*!
 * \~english @brief DSplitedBar::setFixedHeight change the height of the title bar to
 * another value.
 * @param h is the target height.
 */
/*!
 * \~chinese @brief 设置标题栏的高度，默认高度为 50。
 * @param h 需要设置的高度
 */
void DSplitedBar::setFixedHeight(int h)
{
    QWidget::setFixedHeight(h);
}

/*!
 * \~english @brief DSplitedBar::setBackgroundTransparent set the title background transparent
 * @param transparent is the targeting value.
 */
/*!
 * \~chinese @brief 设置标题栏背景是否透明，当为透明时标题栏直接叠加在下层控件上。
 * @param transparent 是否透明
 */
void DSplitedBar::setBackgroundTransparent(bool transparent)
{
    setAutoFillBackground(!transparent);

    if (transparent)
        setBackgroundRole(QPalette::NoRole);
    else
        setBackgroundRole(QPalette::Base);
}

/*!
 * \~english @brief DSplitedBar::setSeparatorVisible sets the bottom separator of the title
 * bar and the window contents to be visible or not.
 * @param visible is the targeting value.
 */
/*!
 * \~chinese @brief 设置菜单下面的分隔线是否可见，默认是可见的。
 * @param 是否可见
 */
void DSplitedBar::setSeparatorVisible(bool visible)
{
    D_D(DSplitedBar);
    if (visible) {
        d->separator->show();
        d->separator->raise();
    } else {
        d->separator->hide();
    }
}

/*!
 * \~english @brief DSplitedBar::setTitle sets the title to be shown on the title bar.
 * @param title is the text to be used as the window title.
 */
/*!
 * \~chinese @brief 设置标题栏文本。
 * @param 待设置内容
 */
void DSplitedBar::setTitle(const QString &title)
{
    D_D(DSplitedBar);
    if (d->titleLabel && !d->embedMode) {
        d->titleLabel->setText(title);
    } else if (parentWidget()) {
        parentWidget()->setWindowTitle(title);
    }
}

/*!
 * \~english @brief DSplitedBar::setIcon sets the icon to be shown on the title bar.
 * @param icon is to be used as the window icon.
 */
/*!
 * \~chinese @brief 设置标题栏图标
 * @param 待设置的图标
 */
void DSplitedBar::setIcon(const QIcon &icon)
{
    D_D(DSplitedBar);
    if (!d->embedMode) {
        d->iconLabel->setIcon(icon);
        d->setIconVisible(!icon.isNull());
    } else if (parentWidget()) {
        d->setIconVisible(false);
        parentWidget()->setWindowIcon(icon);
    }
}

void DSplitedBar::toggleWindowState()
{
    D_D(DSplitedBar);

    d->_q_toggleWindowState();
}

void DSplitedBar::setBlurBackground(bool blurBackground)
{
    D_D(DSplitedBar);

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

/*!
 * \~english @brief DSplitedBar::buttonAreaWidth returns the width of the area that all the
 * window buttons occupies.
 */
/*!
 * \~chinese @brief 按钮区域大小，用于手动定位自定义控件时使用。
 */
int DSplitedBar::buttonAreaWidth() const
{
    D_DC(DSplitedBar);
    return d->buttonArea->width();
}

/*!
 * \~english @brief DSplitedBar::separatorVisible returns the visibility of the bottom
 * separator of the titlebar.
 */
/*!
 * \~chinese @brief 分隔线是否可见。
 */
bool DSplitedBar::separatorVisible() const
{
    D_DC(DSplitedBar);
    return d->separator->isVisible();
}

/*!
 * \~english @brief DSplitedBar::autoHideOnFullscreen returns if titlebar show on fullscreen mode.
 * separator of the titlebar.
 */
/*!
 * \~chinese @brief 全屏模式下标题栏是否自动隐藏
 */
bool DSplitedBar::autoHideOnFullscreen() const
{
    D_DC(DSplitedBar);
    return d->autoHideOnFullscreen;
}

/*!
 * \~english \brief DSplitedBar::setAutoHideOnFullscreen set if titlebar show when window is fullscreen state.
 * \param autohide
 */
/*!
 * \~chinese \brief 设置全屏模式下是否需要自动隐藏标题栏
 * \param 是否自动隐藏
 */
void DSplitedBar::setAutoHideOnFullscreen(bool autohide)
{
    D_D(DSplitedBar);
    d->autoHideOnFullscreen = autohide;
}

void DSplitedBar::setVisible(bool visible)
{
    D_D(DSplitedBar);

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


/*!
 * \~english @brief DSplitedBar::setEmbedMode set a titlebar is in parent;
 */
/*!
 * \~chinese @brief 设置为嵌入模式，而不是替换系统标题栏，用于不支持dxcb的平台
 */
void DSplitedBar::setEmbedMode(bool visible)
{
    D_D(DSplitedBar);
    d->embedMode = visible;
    d->separatorTop->setVisible(visible);
    d->updateButtonsState(windowFlags());
}

/*!
 * \~chinese \brief DSplitedBar::menuIsVisible
 * \~chinese \return true 菜单可见 false菜单不可见
 */
bool DSplitedBar::menuIsVisible() const
{
    D_DC(DSplitedBar);
    return !d->optionButton->isVisible();
}

/*!
 * \~chinese \brief DSplitedBar::setMenuVisible 设置菜单是否可见
 * \~chinese \param visible true 菜单可见 false菜单不可见
 */
void DSplitedBar::setMenuVisible(bool visible)
{
    D_D(DSplitedBar);
    d->optionButton->setVisible(visible);
}

/*!
 * \~chinese \brief DSplitedBar::menuIsDisabled
 * \~chinese \return true 菜单被禁用 false 菜单没有被禁用
 */
bool DSplitedBar::menuIsDisabled() const
{
    D_DC(DSplitedBar);
    return !d->optionButton->isEnabled();
}

/*!
 * \~chinese \brief DSplitedBar::setMenuDisabled 设置菜单是否被禁用
 * \~chinese \param disabled true 菜单被禁用 false菜单没有被禁用
 */
void DSplitedBar::setMenuDisabled(bool disabled)
{
    D_D(DSplitedBar);
    d->optionButton->setDisabled(disabled);
}

/*!
 * \~chinese \brief DSplitedBar::quitMenuIsDisabled
 * \~chinese \return true 退出菜单被禁用 false退出菜单没有被禁用
 */
bool DSplitedBar::quitMenuIsDisabled() const
{
    D_DC(DSplitedBar);

    return d->quitAction && !d->quitAction->isEnabled();
}

/*!
 * \~chinese \brief DSplitedBar::setQuitMenuDisabled 设置退出菜单是否被禁用
 * \~chinese \param disabled true 退出菜单被禁用 false退出菜单没有被禁用
 */
void DSplitedBar::setQuitMenuDisabled(bool disabled)
{
    D_D(DSplitedBar);

    if (!d->quitAction) {
        d->_q_addDefaultMenuItems();
    }

    d->quitAction->setEnabled(!disabled);
}

/*!
 * \~chinese \brief DSplitedBar::setQuitMenuVisible 设置退出菜单是否被可见
 * \~chinese \param visible true 退出菜单可见 false退出菜单不可见
 */
void DSplitedBar::setQuitMenuVisible(bool visible)
{
    D_D(DSplitedBar);

    if (!d->quitAction) {
        d->_q_addDefaultMenuItems();
    }

    d->quitAction->setVisible(visible);
}

/*!
 * \~chinese \brief DSplitedBar::switchThemeMenuIsVisible
 * \~chinese \return true 切换主题菜单可见 false切换主题菜单不可见
 */
bool DSplitedBar::switchThemeMenuIsVisible() const
{
    D_DC(DSplitedBar);

    return d->switchThemeMenu;
}

/*!
 * \~chinese \brief DSplitedBar::setSwitchThemeMenuVisible 设置切换主题菜单是否可见
 * \~chinese \param visible true 切换主题菜单可见 false切换主题菜单不可见
 */
void DSplitedBar::setSwitchThemeMenuVisible(bool visible)
{
    D_D(DSplitedBar);

    if (visible == d->canSwitchTheme) {
        return;
    }

    d->canSwitchTheme = visible;

    if (d->switchThemeMenu) {
        d->switchThemeMenu->menuAction()->setVisible(visible);
        d->themeSeparator->setVisible(visible);
    }
}

/*!
 * \~english \brief DSplitedBar::setDisableFlags will disable button match flags.
 * \param flags
 */
/*!
 * \~chinese \brief 设置需要被禁用的按钮，仅仅是在界面上禁用按钮，还是可以通过事件等机制来调用对应接口。
 * \param flags 需要被禁用的按钮标志位
 */
void DSplitedBar::setDisableFlags(Qt::WindowFlags flags)
{
    D_D(DSplitedBar);
    d->disableFlags = flags;
    d->updateButtonsFunc();
}

/*!
 * \~english \brief DSplitedBar::disableFlags return which button is disabled.
 */
/*!
 * \~chinese \brief 当前被禁用的按钮标志位。
 */
Qt::WindowFlags DSplitedBar::disableFlags() const
{
    D_DC(DSplitedBar);
    return d->disableFlags;
}

QSize DSplitedBar::sizeHint() const
{
    D_DC(DSplitedBar);

    if (d->centerArea->isHidden()) {
        return QFrame::sizeHint();
    }

    int padding = qMax(d->leftArea->sizeHint().width(), d->rightArea->sizeHint().width());
    int width = d->centerArea->sizeHint().width() + 2 * d->mainLayout->spacing() + 2 * padding;

    return QSize(width, DefaultTitlebarHeight);
}

QSize DSplitedBar::minimumSizeHint() const
{
    return sizeHint();
}

bool DSplitedBar::blurBackground() const
{
    D_DC(DSplitedBar);
    return d->blurWidget;
}

void DSplitedBar::mouseMoveEvent(QMouseEvent *event)
{
    D_D(DSplitedBar);

    Qt::MouseButton button = event->buttons() & Qt::LeftButton ? Qt::LeftButton : Qt::NoButton;
    if (event->buttons() == Qt::LeftButton /*&& d->mousePressed*/) {
        if (!d->mousePressed) {
            return;
        }
        Q_EMIT mouseMoving(button);
    }

#ifdef DTK_TITLE_DRAG_WINDOW
    D_D(DSplitedBar);
    if (d->mousePressed) {
        Q_EMIT mousePosMoving(button, event->globalPos());
    }
#endif
    QWidget::mouseMoveEvent(event);
}

void DSplitedBar::mouseDoubleClickEvent(QMouseEvent *event)
{
    D_D(DSplitedBar);

    if (event->buttons() == Qt::LeftButton) {
        d->mousePressed = false;
        Q_EMIT doubleClicked();
    }
}

void DSplitedBar::setLeftMargin(int margin)
{
    D_D(DSplitedBar);
    left_margin = margin;
    d->leftArea->setFixedWidth(margin);
    this->update();
    if (d->blurWidget != nullptr) {
        d->blurWidget->move(left_margin, 0);
    }
}

void DSplitedBar::runHideAnim()
{
    timer->start();
}

void DSplitedBar::runShowAnim()
{
    timer->start();
}

#ifndef QT_NO_MENU

void DSplitedBar::setIconMenu(QMenu *menu)
{
    m_iconMenu = menu;
}

QMenu *DSplitedBarPrivate::automateIconMenu()
{
    if (m_iconmenu == nullptr) {
        m_iconmenu = new QMenu;
        m_iconmenu->addAction("Hello!");
        m_iconmenu->addAction("Devs forgot");
        m_iconmenu->addAction("Me ;[");
    }
    return m_iconmenu;
}

void DSplitedBar::showIconMenu()
{
    D_D(DSplitedBar);
    m_iconMenu->exec(mapToGlobal(QPoint(d->iconLabel->rect().bottomLeft().x() +5, this->height())));
}

#endif

LDA_END_NAMESPACE
