#ifndef DAddonSplittedWINDOW_H
#define DAddonSplittedWINDOW_H

#include "libda_global.h"
#include <DBlurEffectWidget>

LDA_BEGIN_NAMESPACE


DWIDGET_USE_NAMESPACE

/**
 * @brief Class that provides modern Ui with splited (left/right) window.
 */
class DAddonSplittedBar;
class LIBDA_SHARED_EXPORT DAddonSplittedWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DAddonSplittedWindow(QWidget *parent = nullptr, bool leftBlurEnabled = true, DBlurEffectWidget::BlendMode blendMode = DBlurEffectWidget::BlendMode::BehindWindowBlend);

    /**
     * @brief Widget displayed IN the blur area
     * @return The widget being used
     */
    QWidget *leftWidget() const;
    /**
     * @brief Widget on the right
     * @return The widget being used
     */
    QWidget *rightWidget() const;
    /**
     * @brief Widget at the bottom of the right widget (top of)
     * @return The widget being used
     */
    QWidget *bottomWidget() const;
    /**
     * @brief The win's splitted bar
     * @return DSplittedBar being used
     */
    DAddonSplittedBar *splitedbar() const;

    /**
     * @brief Have the left widget container use BehindWin blur
     * @param Left blur state
     */
    void setLeftBlur(bool leftBlur);
    /**
     * @brief Left container blur mode (DBlurEffectWidget)
     */
    void setLeftBlurMode(DBlurEffectWidget::BlurMode);
    /**
     * @brief setLeftBlendMode
     */
    void setLeftBlendMode(DBlurEffectWidget::BlendMode);
    /**
     * @brief Left container blur color (DBlurEffectWidget)
     * @param color
     */
    void setLeftBlurColor(QColor color);

    /**
     * @brief Use the widget w as right widget side
     * @param w
     */
    void setRightWidget(QWidget *w);
    /**
     * @brief Use the bottom widget you want on the right side widget (on top)
     * @param w
     */
    void setBottomWidget(QWidget *w);

    /**
     * @brief Use custom width for the right side area
     * @param width
     */
    void setLeftAreaWidth(int width);
    /**
     * @brief Width of the right area
     * @return Width currently used
     */
    int leftAreaWidth();

    /**
     * @brief Switch from non full screen (so Maximized and lower) to full and conversly
     */
    void switchFullScreen();
    /**
     * @brief Force use of fullscreen, even if the user won't
     * @param force
     */
    void enableForceFullScreen(bool force);
    /**
     * @brief Set if the window have to be shown in fullscreen or not
     * @param full
     */
    void setFullScreen(bool full);

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    DAddonSplittedBar *m_bar;
    DBlurEffectWidget *m_leftw = nullptr;
    QWidget *m_bottomw = nullptr;
    QWidget *m_rightw = nullptr;
    bool isOriginal = true;
    bool forceFullScreen = false;
    int m_leftwidth = 250;
};

LDA_END_NAMESPACE

#endif // DAddonSplittedWINDOW_H
