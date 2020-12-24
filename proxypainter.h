#ifndef PROXYPAINTER_H
#define PROXYPAINTER_H

#include "libda_global.h"
#include <QPainter>

LDA_BEGIN_NAMESPACE

/**
 * @brief Restricted QPainter, used to draw Element[s], this makes plugins unable to glitch or abuse your UI
 */
class LIBDA_SHARED_EXPORT ProxyPainter
{
public:
    explicit ProxyPainter(QPainter *p, QObject *src = nullptr, bool lock_any = false);
    ~ProxyPainter();

    inline QPainter::CompositionMode composistionMode() {return p->compositionMode();}
    inline void setCompositionMode(QPainter::CompositionMode mode) {p->setCompositionMode(mode);}

    inline QPainter::RenderHints renderHints() {return p->renderHints();}
    inline void setRenderHints(QPainter::RenderHint hint, bool on = true) {p->setRenderHints(hint, on);}

    inline const QFont &font() const {return p->font();}
    inline void setFont(const QFont &f) {p->setFont(f);}

    inline QFontMetrics fontMetrics() const {return p->fontMetrics();}
    inline QFontInfo fontInfo() const {return p->fontInfo();}

    inline void setPen(const QColor &color) {p->setPen(color);}
    inline void setPen(const QPen &pen) {p->setPen(pen);}
    inline void setPen(Qt::PenStyle style) {p->setPen(style);}
    inline const QPen &pen() const {return p->pen();}

    inline void setBrush(const QBrush &brush) {p->setBrush(brush);}
    inline void setBrush(Qt::BrushStyle style) {p->setBrush(style);}
    inline const QBrush &brush() const {return p->brush();}

    // drawing functions
    inline void strokePath(const QPainterPath &path, const QPen &pen) {p->strokePath(path, pen);}
    inline void fillPath(const QPainterPath &path, const QBrush &brush) {p->fillPath(path, brush);}
    inline void drawPath(const QPainterPath &path) {p->drawPath(path);}

    inline void drawPoint(const QPointF &pt) {p->drawPoint(pt);}
    inline void drawPoint(const QPoint &pt) {p->drawPoint(pt);}
    inline void drawPoint(int x, int y) {p->drawPoint(x, y);}

    inline void drawPoints(const QPointF *points, int pointCount) {p->drawPoints(points, pointCount);}
    inline void drawPoints(const QPolygonF &points) {p->drawPoints(points);}
    inline void drawPoints(const QPoint *points, int pointCount) {p->drawPoints(points, pointCount);}
    inline void drawPoints(const QPolygon &points) {p->drawPoints(points);}

    inline void drawLine(const QLineF &line) {p->drawLine(line);}
    inline void drawLine(const QLine &line) {p->drawLine(line);}
    inline void drawLine(int x1, int y1, int x2, int y2) {p->drawLine(x1, y1, x2, y2);}
    inline void drawLine(const QPoint &p1, const QPoint &p2) {p->drawLine(p1, p2);}
    inline void drawLine(const QPointF &p1, const QPointF &p2) {p->drawLine(p1, p2);}

    inline void drawLines(const QLineF *lines, int lineCount) {p->drawLines(lines, lineCount);}
    inline void drawLines(const QVector<QLineF> &lines) {p->drawLines(lines);}
    inline void drawLines(const QPointF *pointPairs, int lineCount) {p->drawLines(pointPairs, lineCount);}
    inline void drawLines(const QVector<QPointF> &pointPairs) {p->drawLines(pointPairs);}
    inline void drawLines(const QLine *lines, int lineCount) {p->drawLines(lines, lineCount);}
    inline void drawLines(const QVector<QLine> &lines) {p->drawLines(lines);}
    inline void drawLines(const QPoint *pointPairs, int lineCount) {p->drawLines(pointPairs, lineCount);}
    inline void drawLines(const QVector<QPoint> &pointPairs) {p->drawLines(pointPairs);}

    inline void drawRect(const QRectF &rect) {p->drawRect(rect);}
    inline void drawRect(int x1, int y1, int w, int h) {p->drawRect(x1, y1, w, h);}
    inline void drawRect(const QRect &rect) {p->drawRect(rect);}

    inline void drawRects(const QRectF *rects, int rectCount) {p->drawRects(rects, rectCount);}
    inline void drawRects(const QVector<QRectF> &rectangles) {p->drawRects(rectangles);}
    inline void drawRects(const QRect *rects, int rectCount) {p->drawRects(rects, rectCount);}
    inline void drawRects(const QVector<QRect> &rectangles) {p->drawRects(rectangles);}

    inline void drawEllipse(const QRectF &r) {p->drawEllipse(r);}
    inline void drawEllipse(const QRect &r) {p->drawEllipse(r);}
    inline void drawEllipse(int x, int y, int w, int h) {p->drawEllipse(x, y, w, h);}

    inline void drawEllipse(const QPointF &center, qreal rx, qreal ry) {p->drawEllipse(center, rx, ry);}
    inline void drawEllipse(const QPoint &center, int rx, int ry) {p->drawEllipse(center, rx, ry);}

    inline void drawPolyline(const QPointF *points, int pointCount) {p->drawPolyline(points, pointCount);}
    inline void drawPolyline(const QPolygonF &polyline) {p->drawPolyline(polyline);}
    inline void drawPolyline(const QPoint *points, int pointCount) {p->drawPolyline(points, pointCount);}
    inline void drawPolyline(const QPolygon &polygon) {p->drawPolyline(polygon);}

    inline void drawPolygon(const QPointF *points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill) {p->drawPolygon(points, pointCount, fillRule);}
    inline void drawPolygon(const QPolygonF &polygon, Qt::FillRule fillRule = Qt::OddEvenFill) {p->drawPolygon(polygon, fillRule);}
    inline void drawPolygon(const QPoint *points, int pointCount, Qt::FillRule fillRule = Qt::OddEvenFill) {p->drawPolygon(points, pointCount, fillRule);}
    inline void drawPolygon(const QPolygon &polygon, Qt::FillRule fillRule = Qt::OddEvenFill) {p->drawPolygon(polygon, fillRule);}

    inline void drawConvexPolygon(const QPointF *points, int pointCount) {p->drawConvexPolygon(points, pointCount);}
    inline void drawConvexPolygon(const QPolygonF &polygon) {p->drawConvexPolygon(polygon);}
    inline void drawConvexPolygon(const QPoint *points, int pointCount) {p->drawConvexPolygon(points, pointCount);}
    inline void drawConvexPolygon(const QPolygon &polygon) {p->drawConvexPolygon(polygon);}

    inline void drawArc(const QRectF &rect, int a, int alen) {p->drawArc(rect, a, alen);}
    inline void drawArc(const QRect &r, int a, int alen) {p->drawArc(r, a, alen);}
    inline void drawArc(int x, int y, int w, int h, int a, int alen) {p->drawArc(x, y, w, h, a, alen);}

    inline void drawPie(const QRectF &rect, int a, int alen) {p->drawPie(rect, a, alen);}
    inline void drawPie(int x, int y, int w, int h, int a, int alen) {p->drawPie(x, y, w, h, a, alen);}
    inline void drawPie(const QRect &r, int a, int alen) {p->drawPie(r, a, alen);}

    inline void drawChord(const QRectF &rect, int a, int alen) {p->drawChord(rect, a, alen);}
    inline void drawChord(int x, int y, int w, int h, int a, int alen) {p->drawChord(x, y, w, h, a, alen);}
    inline void drawChord(const QRect &r, int a, int alen) {p->drawChord(r, a, alen);}

    inline void drawRoundedRect(const QRectF &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize) {p->drawRoundedRect(rect, xRadius, yRadius, mode);}
    inline void drawRoundedRect(int x, int y, int w, int h, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize) {p->drawRoundedRect(x, y, w, h, xRadius, yRadius, mode);}
    inline void drawRoundedRect(const QRect &rect, qreal xRadius, qreal yRadius, Qt::SizeMode mode = Qt::AbsoluteSize) {p->drawRoundedRect(rect, xRadius, yRadius, mode);}

    inline void drawRoundRect(const QRectF &r, int xround = 25, int yround = 25) {p->drawRoundRect(r, xround, yround);}
    inline void drawRoundRect(int x, int y, int w, int h, int xr = 25, int yr = 25) {p->drawRoundRect(x, y, w, h, xr, yr);}
    inline void drawRoundRect(const QRect &r, int xround = 25, int yround = 25) {p->drawRoundRect(r, xround, yround);}

    inline void drawTiledPixmap(const QRectF &rect, const QPixmap &pm, const QPointF &offset = QPointF()) {p->drawTiledPixmap(rect, pm, offset);}
    inline void drawTiledPixmap(int x, int y, int w, int h, const QPixmap &pm, int sx=0, int sy=0) {p->drawTiledPixmap(x, y, w, h, pm, sx, sy);}
    inline void drawTiledPixmap(const QRect &r, const QPixmap &pm, const QPoint &po = QPoint()) {p->drawTiledPixmap(r, pm, po);}

#ifndef QT_NO_PICTURE
    inline void drawPicture(const QPointF &po, const QPicture &picture) {p->drawPicture(po, picture);}
    inline void drawPicture(int x, int y, const QPicture &picture) {p->drawPicture(x, y, picture);}
    inline void drawPicture(const QPoint &po, const QPicture &picture) {p->drawPicture(po, picture);}
#endif

    inline void drawPixmap(const QRectF &targetRect, const QPixmap &pixmap, const QRectF &sourceRect) {p->drawPixmap(targetRect, pixmap, sourceRect);}
    inline void drawPixmap(const QRect &targetRect, const QPixmap &pixmap, const QRect &sourceRect) {p->drawPixmap(targetRect, pixmap, sourceRect);}
    inline void drawPixmap(int x, int y, int w, int h, const QPixmap &pm,
                           int sx, int sy, int sw, int sh) {p->drawPixmap(x, y, w, h, pm, sx, sy, sw, sh);}
    inline void drawPixmap(int x, int y, const QPixmap &pm,
                           int sx, int sy, int sw, int sh) {p->drawPixmap(x, y, pm, sx, sy, sw, sh);}
    inline void drawPixmap(const QPointF &po, const QPixmap &pm, const QRectF &sr) {p->drawPixmap(po, pm, sr);}
    inline void drawPixmap(const QPoint &po, const QPixmap &pm, const QRect &sr) {p->drawPixmap(po, pm, sr);}
    inline void drawPixmap(const QPointF &po, const QPixmap &pm) {p->drawPixmap(po, pm);}
    inline void drawPixmap(const QPoint &po, const QPixmap &pm) {p->drawPixmap(po, pm);}
    inline void drawPixmap(int x, int y, const QPixmap &pm) {p->drawPixmap(x, y, pm);}
    inline void drawPixmap(const QRect &r, const QPixmap &pm) {p->drawPixmap(r, pm);}
    inline void drawPixmap(int x, int y, int w, int h, const QPixmap &pm) {p->drawPixmap(x, y, w, h, pm);}

    inline void drawPixmapFragments(const QPainter::PixmapFragment *fragments, int fragmentCount, const QPixmap &pixmap, QPainter::PixmapFragmentHints hints = QPainter::PixmapFragmentHints()) {p->drawPixmapFragments(fragments, fragmentCount, pixmap, hints);}

    inline void drawImage(const QRectF &targetRect, const QImage &image, const QRectF &sourceRimageect, Qt::ImageConversionFlags flags = Qt::AutoColor) {p->drawImage(targetRect, image, sourceRimageect, flags);}
    inline void drawImage(const QRect &targetRect, const QImage &image, const QRect &sourceRect, Qt::ImageConversionFlags flags = Qt::AutoColor) {p->drawImage(targetRect, image, sourceRect, flags);}
    inline void drawImage(const QPointF &po, const QImage &image, const QRectF &sr, Qt::ImageConversionFlags flags = Qt::AutoColor) {p->drawImage(po, image, sr, flags);}
    inline void drawImage(const QPoint &po, const QImage &image, const QRect &sr, Qt::ImageConversionFlags flags = Qt::AutoColor) {p->drawImage(po, image, sr, flags);}
    inline void drawImage(const QRectF &r, const QImage &image) {p->drawImage(r, image);}
    inline void drawImage(const QRect &r, const QImage &image) {p->drawImage(r, image);}
    inline void drawImage(const QPointF &po, const QImage &image) {p->drawImage(po,image);}
    inline void drawImage(const QPoint &po, const QImage &image) {p->drawImage(po, image);}
    inline void drawImage(int x, int y, const QImage &image, int sx = 0, int sy = 0, int sw = -1, int sh = -1, Qt::ImageConversionFlags flags = Qt::AutoColor) {p->drawImage(x, y, image, sx, sy, sw, sh, flags);}

#if !defined(QT_NO_RAWFONT)
    inline void drawGlyphRun(const QPointF &position, const QGlyphRun &glyphRun) {p->drawGlyphRun(position, glyphRun);}
#endif

    inline void drawStaticText(int left, int top, const QStaticText &staticText) {p->drawStaticText(left, top, staticText);}
    inline void drawStaticText(const QPoint &topLeftPosition, const QStaticText &staticText) {p->drawStaticText(topLeftPosition, staticText);}
    inline void drawStaticText(const QPointF &topLeftPosition, const QStaticText &staticText) {p->drawStaticText(topLeftPosition, staticText);}

    inline void drawText(int x, int y, const QString &s) {p->drawText(x, y, s);}
    inline void drawText(const QPoint &po, const QString &s) {p->drawText(po, s);}
    inline void drawText(const QPointF &po, const QString &s) {p->drawText(po, s);}
    inline void drawText(const QRect &r, int flags, const QString &text, QRect *br = nullptr) {p->drawText(r, flags, text, br);}
    inline void drawText(const QRectF &r, int flags, const QString &text, QRectF *br = nullptr) {p->drawText(r, flags, text, br);}
    inline void drawText(const QPointF &po, const QString &str, int tf, int justificationPadding) {p->drawText(po, str, tf, justificationPadding);}
    inline void drawText(const QRectF &r, const QString &text, const QTextOption &o = QTextOption()) {p->drawText(r, text, o);}
    inline void drawText(int x, int y, int w, int h, int flags, const QString &text, QRect *br = nullptr) {p->drawText(x, y, w, h, flags, text, br);}

    inline void drawTextItem(const QPointF &po, const QTextItem &ti) {p->drawTextItem(po, ti);}
    inline void drawTextItem(int x, int y, const QTextItem &ti) {p->drawTextItem(x, y, ti);}
    inline void drawTextItem(const QPoint &po, const QTextItem &ti) {p->drawTextItem(po, ti);}

    inline QRect boundingRect(const QRect &rect, int flags, const QString &text) {return p->boundingRect(rect, flags, text);}
    inline QRectF boundingRect(const QRectF &rect, int flags, const QString &text) {return p->boundingRect(rect, flags, text);}
    inline QRect boundingRect(int x, int y, int w, int h, int flags, const QString &text) {return p->boundingRect(x, y, w, h, flags, text);}
    inline QRectF boundingRect(const QRectF &rect, const QString &text, const QTextOption &o = QTextOption()) {return p->boundingRect(rect, text, o);}

    inline void fillRect(const QRect &r, const QBrush &b) {p->fillRect(r, b);}
    inline void fillRect(const QRectF &r, const QBrush &b) {p->fillRect(r, b);}
    inline void fillRect(const QRect &r, Qt::GlobalColor c) {p->fillRect(r, c);}
    inline void fillRect(const QRectF &r, Qt::GlobalColor c) {p->fillRect(r, c);}
    inline void fillRect(const QRect &r, Qt::BrushStyle style) {p->fillRect(r, style);}
    inline void fillRect(const QRectF &r, Qt::BrushStyle style) {p->fillRect(r, style);}
    inline void fillRect(const QRectF &rec, const QColor &color) {p->fillRect(rec, color);}
    inline void fillRect(int x, int y, int w, int h, const QBrush &b) {p->fillRect(x, y, w, h, b);}
    inline void fillRect(int x, int y, int w, int h, Qt::GlobalColor c) {p->fillRect(x, y, w, h, c);}
    inline void fillRect(int x, int y, int w, int h, const QColor &color) {p->fillRect(x, y, w, h, color);}
    inline void fillRect(int x, int y, int w, int h, Qt::BrushStyle style) {p->fillRect(x, y, w, h, style);}

    void setSource(QObject *check, QPainter *p);
private:
    QPainter *p;
    QObject *check;
    bool any_way_lock = false;
};

LDA_END_NAMESPACE

#endif // PROXYPAINTER_H
