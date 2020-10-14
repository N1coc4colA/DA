#include "effectwidget.h"

#include <iostream>

#include "graphictools.h"
#include "effectwidget_p.h"

#include <DPlatformWindowHandle>
#include <DWindowManagerHelper>
#include <DGuiApplicationHelper>

#include <QPainter>
#include <QBackingStore>
#include <QPaintEvent>

#include <qpa/qplatformbackingstore.h>
#include <private/qwidget_p.h>
#ifndef slots
#define slots Q_SLOTS
#endif

#define private public

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
#include <private/qwidgetrepaintmanager_p.h>
#else
#include <private/qwidgetbackingstore_p.h>
#endif

#undef private

#define MASK_COLOR_ALPHA_DEFAULT 204

QT_BEGIN_NAMESPACE
Q_WIDGETS_EXPORT void qt_blurImage(QPainter *p, QImage &blurImage, qreal radius, bool quality, bool alphaOnly, int transposed = 0);
QT_END_NAMESPACE

DGUI_USE_NAMESPACE

LDA_BEGIN_NAMESPACE

EffectWidgetPrivate::EffectWidgetPrivate(EffectWidget *qq)
    : DObjectPrivate(qq)
{

}

bool EffectWidgetPrivate::isFull() const
{
    D_QC(EffectWidget);
    return full || (q->isTopLevel() && maskPath.isEmpty());
}

void EffectWidgetPrivate::setMaskAlpha(const quint8 alpha) {
    maskAlpha = alpha;

    // refresh alpha
    setMaskColor(maskColor);
}

quint8 EffectWidgetPrivate::getMaskColorAlpha() const
{
    if (maskAlpha < 0)
        return 102;

    return static_cast<quint8>(maskAlpha);
}

QColor EffectWidgetPrivate::getMaskColor(const QColor &baseColor) const
{
    QColor color = baseColor;
    DGuiApplicationHelper::ColorType ct = DGuiApplicationHelper::toColorType(color);

    if (DGuiApplicationHelper::DarkType == ct) {
        color = DGuiApplicationHelper::adjustColor(color, 0, 0, -10, 0, 0, 0, 0);
    } else {
        color = DGuiApplicationHelper::adjustColor(color, 0, 0, -5, 0, 0, 0, 0);
    }

    int maskAlpha = this->getMaskColorAlpha();

    if (DWindowManagerHelper::instance()->hasComposite()) {
        color.setAlpha(maskAlpha);
    } else {
        return ct == DGuiApplicationHelper::DarkType ? "#202020" : "#D2D2D2";
    }

    return color;
}

void EffectWidgetPrivate::resetSourceImage()
{
    if (customSourceImage)
        return;

    sourceImage = QImage();
}

void EffectWidgetPrivate::setMaskColor(const QColor &color)
{
    maskColor = color;

    D_Q(EffectWidget);

    q->update();
}

EffectWidget::EffectWidget(QWidget *parent)
    : QWidget(parent)
    , DObject(*new EffectWidgetPrivate(this))
{
    setAttribute(Qt::WA_TranslucentBackground);
    setBackgroundRole(QPalette::Window);

    QObject::connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasBlurWindowChanged, this, [this] {
        D_D(EffectWidget);

        d->setMaskColor(d->maskColor);
    });
    QObject::connect(DWindowManagerHelper::instance(), &DWindowManagerHelper::hasCompositeChanged, this, [this] {
        D_D(const EffectWidget);

        if (d->maskColorType != CustomColor)
            update();
    });
}

EffectWidget::~EffectWidget()
{
}

int EffectWidget::radius() const
{
    D_DC(EffectWidget);
    return d->radius;
}

QColor EffectWidget::maskColor() const
{
    D_DC(EffectWidget);

    switch (d->maskColorType) {
    case DarkColor:
        return d->getMaskColor(DGuiApplicationHelper::standardPalette(DGuiApplicationHelper::DarkType).window().color());
    case LightColor:
        return d->getMaskColor(DGuiApplicationHelper::standardPalette(DGuiApplicationHelper::LightType).window().color());
    case AutoColor: {
        QColor color = palette().color(backgroundRole());

        return d->getMaskColor(color);
    }
    default: return d->maskColor;
    }
}

void EffectWidget::setSourceImage(const QImage &image, bool autoScale)
{
    D_D(EffectWidget);

    d->sourceImage = image;
    d->customSourceImage = !image.isNull();
    d->autoScaleSourceImage = autoScale && d->customSourceImage;

    if (autoScale && isVisible()) {
        d->sourceImage.setDevicePixelRatio(devicePixelRatioF());
        d->sourceImage = d->sourceImage.scaled((size() + QSize(d->radius * 1, d->radius * 2)) * devicePixelRatioF());
        d->sourceImage.setDevicePixelRatio(devicePixelRatioF());
    }
}

bool EffectWidget::isFull() const
{
    D_DC(EffectWidget);

    return d->full;
}

bool EffectWidget::blurEnabled() const
{
    D_DC(EffectWidget);

    return d->blurEnabled;
}

void EffectWidget::setRadius(int radius)
{
    D_D(EffectWidget);
/*
    if (d->radius == radius) {
        return;
    }

    d->radius = radius;*/
    internal_blur_radius = radius;
    d->resetSourceImage();

    update();

    Q_EMIT radiusChanged(radius);
}

void EffectWidget::setMaskColor(QColor maskColor)
{
    D_D(EffectWidget);

    if (!maskColor.isValid()) {
        maskColor = Qt::transparent;
    }

    d->maskColorType = CustomColor;
    d->setMaskColor(maskColor);

    Q_EMIT maskColorChanged(maskColor);
}

void EffectWidget::setMaskColor(EffectWidget::MaskColorType type)
{
    D_D(EffectWidget);

    if (d->maskColorType == type) {
        return;
    }

    d->maskColorType = type;

    update();
}

void EffectWidget::setFull(bool full)
{
    D_D(EffectWidget);
    if (d->full == full)
        return;
    d->full = full;
    Q_EMIT fullChanged(full);
}

void EffectWidget::setBlurEnabled(bool blurEnabled)
{
    D_D(EffectWidget);

    if (d->blurEnabled == blurEnabled)
        return;

    d->blurEnabled = blurEnabled;
    update();

    Q_EMIT blurEnabledChanged(d->blurEnabled);
}

void EffectWidget::setDirection(Qt::Edge e)
{
    direction = e;
    update();
}

inline QRect operator *(const QRect &rect, qreal scale)
{
    return QRect(int(rect.left() * scale), int(rect.top() * scale), int(rect.width() * scale), int(rect.height() * scale));
}

void EffectWidget::updateBlurSourceImage(const QRegion &ren)
{
    D_D(EffectWidget);

    if (d->customSourceImage)
        return;

    const qreal device_pixel_ratio = devicePixelRatioF();
    const QPoint point_offset = mapTo(window(), QPoint(0, 0));

    if (d->sourceImage.isNull()) {
        const QRect &tmp_rect = rect().translated(point_offset).adjusted(-d->radius, -d->radius, d->radius, d->radius);

        d->sourceImage = window()->backingStore()->handle()->toImage().copy(tmp_rect * device_pixel_ratio);
        d->sourceImage = d->sourceImage.scaledToWidth(int(d->sourceImage.width() / device_pixel_ratio));
    } else {
        transformAlpha(d->sourceImage, this->direction);
        QPainter pa_image(&d->sourceImage);

        pa_image.setCompositionMode(QPainter::CompositionMode_Source);

        if (device_pixel_ratio > 1) {
            const QRect &tmp_rect = this->rect().translated(point_offset);
            QImage area = window()->backingStore()->handle()->toImage().copy(tmp_rect * device_pixel_ratio);
            area = area.scaledToWidth(int(area.width() / device_pixel_ratio));

            for (const QRect &rect : ren.rects()) {
                pa_image.drawImage(rect.topLeft() + QPoint(d->radius, d->radius), rect == area.rect() ? area : area.copy(rect));
            }
        } else {
            for (const QRect &rect : ren.rects()) {
                pa_image.drawImage(rect.topLeft() + QPoint(d->radius, d->radius),
                                   window()->backingStore()->handle()->toImage().copy(rect.translated(point_offset)));
            }
        }

        pa_image.end();
    }
}

EffectWidget::EffectWidget(EffectWidgetPrivate &dd, QWidget *parent) : QWidget(parent) , DObject(dd) {}

void EffectWidget::paintBlurEffect(QPainter &painter)
{
    D_D(EffectWidget);
    if (d->blurEnabled == true) {
        updateBlurSourceImage(QRegion(0,0, this->width(), this->height()));

        int radius = 20;
        const QRect &paintRect = this->rect();
        QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;

        blur->setBlurRadius(internal_blur_radius);
        painter.drawImage(this->rect(),
                          applyEffectToImage(d->sourceImage.copy(paintRect.adjusted(0, 0, 2 * radius, 2 * radius)), blur)
                                                      .copy(QRect(int(1.75*radius), int(1.75*radius), this->width(), this->height())),
                     this->rect());
    }
    painter.fillRect(this->rect(), maskColor());
}

void EffectWidget::paintMaskedBlurEffect(QPainter &painter)
{
    D_D(EffectWidget);
    if (d->blurEnabled == true) {
        updateBlurSourceImage(QRegion(0,0, this->width(), this->height()));

        int radius = 20;
        const QRect &paintRect = this->rect();
        QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
        blur->setBlurRadius(internal_blur_radius);

        painter.drawImage(this->rect(),
                     transformAlpha(applyEffectToImage(d->sourceImage.copy(paintRect.adjusted(0, 0, 2 * radius, 2 * radius)), blur)
                                                      .copy(QRect(int(1.75*radius), int(1.75*radius), this->width(), this->height())),
                                    direction),
                     this->rect());
    }
    QLinearGradient grad;
    grad.setColorAt(0, maskColor());
    grad.setColorAt(1, Qt::transparent);
    grad.setStart(QPoint(0,0));
    grad.setFinalStop(QPoint(0, this->height()));

    switch (direction) {
    case Qt::Edge::BottomEdge: grad.setStart(0,this->height()); grad.setFinalStop(0, 0); break;
    case Qt::Edge::TopEdge: grad.setStart(0,0); grad.setFinalStop(0,this->height()); break;
    case Qt::Edge::RightEdge: grad.setStart(0,0); grad.setFinalStop(this->width(),0); break;
    case Qt::Edge::LeftEdge: grad.setStart(this->width(), 0); grad.setFinalStop(0,0); break;
    }
    painter.fillRect(this->rect(), grad);
}

void EffectWidget::paintBlurChannel(QPainter &painter)
{
    D_D(EffectWidget);

    if (d->blurEnabled == true) {
        updateBlurSourceImage(QRegion(0,0, this->width(), this->height()));
        const QRect &paintRect = this->rect();
        int radius = 20;

        QImage image = blendChannel(d->sourceImage
                                .copy(paintRect.adjusted(0, 0, 2 * radius, 2 * radius)),
                             effect->channel,
                             effect->splitMode,
                             internal_blur_radius)
                            .copy(QRect(int(1.75 * radius), int(1.75 * radius), this->width(), this->height()));
        painter.drawImage(this->rect(), image, this->rect());
    }
    painter.fillRect(this->rect(), maskColor());
}

void EffectWidget::paintGrayScale(QPainter &painter)
{
    D_D(EffectWidget);

    if (d->blurEnabled == true) {
        updateBlurSourceImage(QRegion(0,0, this->width(), this->height()));
        const QRect &paintRect = this->rect();
        int radius = 20;

        QImage image = bindToGS(d->sourceImage.copy(paintRect.adjusted(0, 0, 2 * radius, 2 * radius))).copy(QRect(int(1.75 * radius), int(1.75 * radius), this->width(), this->height()));

        painter.drawImage(this->rect(), image, this->rect());
    }
    painter.fillRect(this->rect(), maskColor());
}

void EffectWidget::paintSplittedChannels(QPainter &painter)
{
    D_D(EffectWidget);

    if (d->blurEnabled == true) {
        updateBlurSourceImage(QRegion(0,0, this->width(), this->height()));
        const QRect &paintRect = this->rect();
        int radius = 20;
        QImage image = splitChannel(d->sourceImage
                                .copy(paintRect.adjusted(0, 0, 2 * radius, 2 * radius)),
                             effect->channel,
                             effect->splitMode,
                             effect->direction,
                             effect->splitPadding)
                            .copy(QRect(int(1.75 * radius), int(1.75 * radius), this->width(), this->height()));
        painter.drawImage(this->rect(), image, this->rect());
    }
    painter.fillRect(this->rect(), maskColor());
}

void EffectWidget::paintEvent(QPaintEvent *event)
{
    QPainter pa(this);
    pa.setOpacity(1);
    switch (effect->type) {
    case VisualEffect::EffectType::Blur: paintBlurEffect(pa); break;
    case VisualEffect::EffectType::Masked: paintMaskedBlurEffect(pa); break;
    case VisualEffect::EffectType::GrayScale: paintGrayScale(pa); break;
    case VisualEffect::EffectType::ChannelBlur: paintBlurChannel(pa); break;
    case VisualEffect::EffectType::SplittedChannels: paintSplittedChannels(pa); break;
    }
}

void EffectWidget::moveEvent(QMoveEvent *event)
{
    D_D(EffectWidget);

    if (isTopLevel()) {
        return QWidget::moveEvent(event);
    }
    d->resetSourceImage();
    return QWidget::moveEvent(event);
}

void EffectWidget::resizeEvent(QResizeEvent *event)
{
    D_D(EffectWidget);
    d->resetSourceImage();
    if (d->autoScaleSourceImage) {
        d->sourceImage = d->sourceImage.scaled((size() + QSize(d->radius * 1, d->radius * 2)) * devicePixelRatioF());
        d->sourceImage.setDevicePixelRatio(devicePixelRatioF());
    }
    return QWidget::resizeEvent(event);
}

void EffectWidget::showEvent(QShowEvent *event)
{
    D_D(EffectWidget);
    if (d->autoScaleSourceImage) {
        d->sourceImage = d->sourceImage.scaled((size() + QSize(d->radius * 1, d->radius * 2)) * devicePixelRatioF());
        d->sourceImage.setDevicePixelRatio(devicePixelRatioF());
    }
    topLevelWidget()->installEventFilter(this);
    return QWidget::showEvent(event);
}

void EffectWidget::hideEvent(QHideEvent *event)
{
    topLevelWidget()->removeEventFilter(this);
    return QWidget::hideEvent(event);
}

void EffectWidget::changeEvent(QEvent *event)
{
    return QWidget::changeEvent(event);
}

bool EffectWidget::eventFilter(QObject *watched, QEvent *event)
{
    if (event->type() != QEvent::UpdateRequest) {
        return QWidget::eventFilter(watched, event);
    }

    if (QWidget *widget = qobject_cast<QWidget*>(watched)) {
        auto wd = QWidgetPrivate::get(widget);

#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        if (!wd->maybeRepaintManager()) {
#else
        if (!wd->maybeBackingStore()) {
#endif
            return QWidget::eventFilter(watched, event);
        }

        QRegion dirty;
#if (QT_VERSION >= QT_VERSION_CHECK(5, 14, 0))
        for (const QWidget *w : wd->maybeRepaintManager()->dirtyWidgets) {
#else
        for (const QWidget *w : wd->maybeBackingStore()->dirtyWidgets) {
#endif
            dirty |= QWidgetPrivate::get(w)->dirty.translated(w->mapToGlobal(QPoint(0, 0)));
        }

        if (dirty.isEmpty()) {
            return QWidget::eventFilter(watched, event);
        }

        D_D(EffectWidget);
        const QPoint &offset = mapToGlobal(QPoint(0, 0));
        const QRect frame_rect = rect() + QMargins(d->radius, d->radius, d->radius, d->radius);
        QRegion radius_edge = QRegion(frame_rect) - QRegion(rect());

        if (!(dirty & radius_edge.translated(offset)).isEmpty()) {
            d->resetSourceImage();
            update();
        }
    }

    return QWidget::eventFilter(watched, event);
}

LDA_END_NAMESPACE
