#ifndef DSPLITEDWINDOW_H
#define DSPLITEDWINDOW_H

#include "libda_global.h"
#include <DBlurEffectWidget>

LDA_BEGIN_NAMESPACE

DWIDGET_USE_NAMESPACE

class DSplitedBar;

/**
 * @brief Class that provides modern Ui with splited (left/right) window.
 */

class LIBDA_SHARED_EXPORT DSplitedWindow : public QWidget
{
    Q_OBJECT
public:
    explicit DSplitedWindow(QWidget *parent = nullptr, bool leftBlurEnabled = true, DBlurEffectWidget::BlendMode blendMode = DBlurEffectWidget::BlendMode::BehindWindowBlend);

    /**
     * @brief Widget used on the left side
     * @return
     */
    QWidget *leftWidget() const;
    /**
     * @brief Widget used on the right side
     * @return
     */
    QWidget *rightWidget() const;
    /**
     * @brief Widget at the bottom of the right side
     * @return
     */
    QWidget *bottomWidget() const;
    /**
     * @brief DSplittedBar used in this window
     * @return
     */
    DSplitedBar *splitedbar() const;

    void setLeftBlur(bool leftBlur);
    /**
     * @brief set blur mode of the left DBlurEffectWidget
     */
    void setLeftBlurMode(DBlurEffectWidget::BlurMode);
    /**
     * @brief set blend mode of the left DBlurEffectWidget
     */
    void setLeftBlendMode(DBlurEffectWidget::BlendMode);
    /**
     * @brief Blur color of the left side
     * @param color
     */
    void setLeftBlurColor(QColor color);

    /**
     * @brief Choose the widget to use at the right side
     * @param w
     */
    void setRightWidget(QWidget *w);
    /**
     * @brief Add a widget at the bottom of the right area
     * @param w
     */
    void setBottomWidget(QWidget *w);

    /**
     * @brief Change the left area size
     * @param width
     */
    void setLeftAreaWidth(int width);
    /**
     * @brief Width of the left area, adapted with the splitted bar's too
     * @return
     */
    int leftAreaWidth();

    /**
     * @brief Toggle Fullscreen mode
     */
    void switchFullScreen();
    /**
     * @brief Enforce fullscreen, if user wants to resize it or any, impossible!
     * @param force
     */
    void enableForceFullScreen(bool force);
    /**
     * @brief Enable fullscreen mode or don't
     * @param full
     */
    void setFullScreen(bool full);

protected:
    void resizeEvent(QResizeEvent *e) override;

private:
    DSplitedBar *m_bar;
    DBlurEffectWidget *m_leftw = nullptr;
    QWidget *m_bottomw = nullptr;
    QWidget *m_rightw = nullptr;
    bool isOriginal = true;
    bool forceFullScreen = false;
    int m_leftwidth = 250;
};

LDA_END_NAMESPACE

#endif // DSPLITEDWINDOW_H
