#ifndef DSPLITEDBAR_H
#define DSPLITEDBAR_H

#include "libda_global.h"

#include <dtkwidget_global.h>
#include <DGuiApplicationHelper>

#include <DObjectPrivate>

#include <dwindowclosebutton.h>
#include <dwindowmaxbutton.h>
#include <dwindowminbutton.h>
#include <dwindowoptionbutton.h>
#include <dplatformwindowhandle.h>
#include <daboutdialog.h>
#include <dapplication.h>
#include <dmainwindow.h>
#include <DHorizontalLine>
#include <dimagebutton.h>
#include <dblureffectwidget.h>
#include <dwidgetstype.h>
#include <dlabel.h>

#include <QMenu>
#include <QFrame>

class QHBoxLayout;
class QTimer;

DGUI_USE_NAMESPACE
DWIDGET_USE_NAMESPACE

LDA_BEGIN_NAMESPACE

class DSplitedWindow;
class DSplitedBarPrivate;

/**
 * @brief Modern splited bar for DSplitedWindow
 */

class LIBDA_SHARED_EXPORT DSplitedBar : public QFrame, public DTK_CORE_NAMESPACE::DObject
{
    Q_OBJECT
    Q_PROPERTY(bool blurBackground READ blurBackground WRITE setBlurBackground)

public:
    explicit DSplitedBar(QWidget *parent = Q_NULLPTR);

#ifndef QT_NO_MENU
    /**
     * @return Current menu of the right side
     */
    QMenu *menu() const;
    /**
     * @brief Current menu of the left side
     * @return
     */
    QMenu *leftMenu() const;
    /**
     * @brief Change the menu of the left side
     * @param menu
     */
    void setLeftMenu(QMenu *menu);
    /**
     * @brief change the menu of the right side
     * @param menu
     */
    void setMenu(QMenu *menu);
#endif

    /**
     * @brief Custom widget currently used (instead of title label)
     * @return
     */
    QWidget *customWidget() const;
    /**
     * @brief Set the widget to use instead of the title label
     * @param fixCenterPos
     */
    void setCustomWidget(QWidget *, bool fixCenterPos = false);

    /**
     * @brief Add a widget to the bar
     * @param w
     * @param alignment
     */
    void addWidget(QWidget *w, Qt::Alignment alignment = Qt::Alignment());
    /**
     * @brief Remove a widget of the bar
     * @param w
     */
    void removeWidget(QWidget *w);

    /**
     * @brief Right side buttons area width
     * @return
     */
    int buttonAreaWidth() const;
    /**
     * @brief Should the separator be visible
     * @return
     */
    bool separatorVisible() const;

    bool autoHideOnFullscreen() const;
    /**
     * @brief Auto change bar's visibility in fullscreen
     * @param autohide
     */
    void setAutoHideOnFullscreen(bool autohide);

    void setVisible(bool visible) Q_DECL_OVERRIDE;
    /**
     * @brief Embedded mode hides some buttons and other stuff
     * @param embed
     */
    void setEmbedMode(bool embed);

    /**
     * @brief Right menu visibility
     * @return Is the right side menu visible?
     */
    bool menuIsVisible() const;
    /**
     * @brief Set right side menu visible
     * @param visible
     */
    void setMenuVisible(bool visible);

    /**
     * @return Is the right side menu visible?
     */
    bool menuIsDisabled() const;
    /**
     * @brief Set if the menu of the right side have to be visible
     * @param disabled
     */
    void setMenuDisabled(bool disabled);

    bool quitMenuIsDisabled() const;
    /**
     * @brief Must the quit menu disbled
     * @param disabled
     */
    void setQuitMenuDisabled(bool disabled);
    /**
     * @brief Should the quit menu visible
     * @param visible
     */
    void setQuitMenuVisible(bool visible);

    /**
     * @return Have the switch theme menu to be visible
     */
    bool switchThemeMenuIsVisible() const;
    /**
     * @brief Change if the theme menu have to be visible or not
     * @param visible
     */
    void setSwitchThemeMenuVisible(bool visible);

    /**
     * @brief Disable already set flags
     * @param flags
     */
    void setDisableFlags(Qt::WindowFlags flags);
    /**
     * @return Get disabled flags
     */
    Qt::WindowFlags disableFlags() const;

    QSize sizeHint() const override;
    QSize minimumSizeHint() const override;

    /**
     * @return Is the background being blurred?
     */
    bool blurBackground() const;

    /**
     * @brief Margin on the left side (of the title label or custom widget)
     * @return
     */
    int leftMargin() const;
    /**
     * @brief Change the margin on the left (of the title label or custom widget)
     * @param margin
     */
    void setLeftMargin(int margin);

Q_SIGNALS:
    void optionClicked();
    void doubleClicked();
    void mousePressed(Qt::MouseButtons buttons);
    void mouseMoving(Qt::MouseButton button);

#ifdef DTK_TITLE_DRAG_WINDOW
    void mousePosPressed(Qt::MouseButtons buttons, QPoint pos);
    void mousePosMoving(Qt::MouseButton button, QPoint pos);
#endif

public Q_SLOTS:
    void setFixedHeight(int h);
    /**
     * @brief Translucent background (no blur)
     * @param transparent
     */
    void setBackgroundTransparent(bool transparent);
    /**
     * @brief Choose wether or not the separator must be visible
     * @param visible
     */
    void setSeparatorVisible(bool visible);
    /**
     * @brief Set title shown in the splitted bar
     * @param title
     */
    void setTitle(const QString &title);
    /**
     * @brief Set icon of the top-left hand corner
     * @param icon
     */
    void setIcon(const QIcon &icon);
    /**
     * @brief Switch between minimized and maximized states
     */
    void toggleWindowState();
    /**
     * @brief Set menu of the top-left hand corner (the icon)
     */
    void setIconMenu(QMenu *);
    /**
     * @brief Choose if the background of the bar have to be blurred (only the right side)
     * @param blurBackground
     */
    void setBlurBackground(bool blurBackground);
    void setCustomTitleAlign(Qt::Alignment);
    /**
     * @brief No complete implementation yet
     */
    void runHideAnim();
    /**
     * @brief No complete animation yet
     */
    void runShowAnim();

private Q_SLOTS:
#ifndef QT_NO_MENU
    /**
     * @brief Show classic Dtk option menu
     */
    void showMenu();
    /**
     * @brief Show menu of the icon button (top-left hand corner)
     */
    void showIconMenu();
#endif

protected:
    bool eventFilter(QObject *obj, QEvent *event) Q_DECL_OVERRIDE;
    bool event(QEvent *e) override;
    void showEvent(QShowEvent *event) Q_DECL_OVERRIDE;
    void mousePressEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseReleaseEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseMoveEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void mouseDoubleClickEvent(QMouseEvent *event) Q_DECL_OVERRIDE;
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    int left_margin = 250;
    bool isFirstMarginSet = true;
    QMenu *m_iconMenu;
    QTimer *timer;
    int oldHeight = 40;

    D_DECLARE_PRIVATE(DSplitedBar)
    D_PRIVATE_SLOT(void _q_toggleWindowState())
    D_PRIVATE_SLOT(void _q_showMinimized())
    D_PRIVATE_SLOT(void _q_onTopWindowMotifHintsChanged(quint32))

#ifndef QT_NO_MENU
    D_PRIVATE_SLOT(void _q_addDefaultMenuItems())
    D_PRIVATE_SLOT(void _q_helpActionTriggered())
    D_PRIVATE_SLOT(void _q_aboutActionTriggered())
    D_PRIVATE_SLOT(void _q_quitActionTriggered())
    D_PRIVATE_SLOT(void _q_switchThemeActionTriggered(QAction*))
#endif
};

/**
 * @brief Private class used for DSplittedBar alias DSplittedBarPrivate
 */
class DSplitedBarPrivate : public DTK_CORE_NAMESPACE::DObjectPrivate
{
protected:
    DSplitedBarPrivate(DSplitedBar *qq);

private:
    void init();
    QWidget *targetWindow();
    QWidget *titleSource;
    bool isVisableOnFullscreen();
    void hideOnFullscreen();
    void showOnFullscreen();

    void updateFullscreen();
    void updateButtonsState(Qt::WindowFlags type);
    void updateButtonsFunc();
    void updateCenterArea();

    void handleParentWindowStateChange();
    void handleParentWindowIdChange();
    void _q_toggleWindowState();
    void _q_showMinimized();
    void _q_onTopWindowMotifHintsChanged(quint32 winId);

#ifndef QT_NO_MENU
    void _q_addDefaultMenuItems();
    void _q_helpActionTriggered();
    void _q_aboutActionTriggered();
    void _q_quitActionTriggered();
    void _q_switchThemeActionTriggered(QAction*action);
#endif

    void setIconVisible(bool visible);
    void updateTabOrder();

    QHBoxLayout         *mainLayout;
    QWidget             *leftArea;
    QHBoxLayout         *leftLayout;
    QWidget             *rightArea;
    QHBoxLayout         *rightLayout;
    DLabel              *centerArea;
    QHBoxLayout         *centerLayout;
    DIconButton         *iconLabel;
    QWidget             *buttonArea;
    DWindowMinButton    *minButton;
    DWindowMaxButton    *maxButton;
    DWindowCloseButton  *closeButton;
    DWindowOptionButton *optionButton;
    DImageButton        *quitFullButton;
    DLabel              *titleLabel;
    QWidget             *customWidget = nullptr;

    DHorizontalLine     *separatorTop;
    DHorizontalLine     *separator;

    DBlurEffectWidget   *blurWidget = nullptr;

#ifndef QT_NO_MENU
    QMenu               *menu             = Q_NULLPTR;
    QAction             *helpAction       = Q_NULLPTR;
    QAction             *aboutAction      = Q_NULLPTR;
    QAction             *quitAction       = Q_NULLPTR;
    bool                canSwitchTheme    = true;
    QAction             *themeSeparator   = nullptr;
    QMenu               *switchThemeMenu  = nullptr;
    QAction             *autoThemeAction  = nullptr;
    QAction             *lightThemeAction = nullptr;
    QAction             *darkThemeAction  = nullptr;
    QMenu               *automateIconMenu();
    QMenu               *m_iconmenu       = nullptr;
#endif

    QWindow            *targetWindowHandle = Q_NULLPTR;

    Qt::WindowFlags     disableFlags;
    bool                mousePressed    = false;
    bool                embedMode       = false;
    bool                autoHideOnFullscreen = false;

    Q_DECLARE_PUBLIC(DSplitedBar)
};

LDA_END_NAMESPACE

#endif // DSPLITEDBAR_H
