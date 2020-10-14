#include "graphictools.h"

#include <QGraphicsBlurEffect>
#include <QGraphicsEffect>
#include <QGraphicsScene>
#include <QGraphicsPixmapItem>
#include <QPainter>

QImage applyEffectToImage(QImage src, QGraphicsEffect *effect, int extent)
{
    if(src.isNull()) return QImage();
    if(!effect) return src;

    QGraphicsScene scene;
    QGraphicsPixmapItem item;
    item.setPixmap(QPixmap::fromImage(src));
    item.setGraphicsEffect(effect);
    scene.addItem(&item);
    QImage res(src.size()+QSize(extent*2, extent*2), QImage::Format_ARGB32);
    res.fill(Qt::transparent);
    QPainter ptr(&res);
    scene.render(&ptr, QRectF(), QRectF( -extent, -extent, src.width()+extent*2, src.height()+extent*2 ) );
    return res;
}

void rotateImage(QImage &image, double angle)
{
    QPoint center = image.rect().center();
    QMatrix matrix;
    matrix.translate(center.x(), center.y());
    matrix.rotate(angle);
    image = image.transformed(matrix);
}

QImage transformAlpha(QImage input, Qt::Edge edge)
{
    // Rotate to let the algo be good with the edge we want
    switch (edge) {
    case Qt::Edge::BottomEdge: break;
    case Qt::Edge::RightEdge: rotateImage(input, 90); break;
    case Qt::Edge::TopEdge: rotateImage(input, 180); break;
    case Qt::Edge::LeftEdge: rotateImage(input, -90); break;
    }
    int x_len = 0, y_len = 0, useAsHeight = input.height(), useAsWidth = input.width();

    // Alpha gradient THE transform
    while(x_len<useAsWidth) {
        y_len = 0;
        while(y_len<useAsHeight) {
            QRgb pixel = input.pixel(x_len, y_len);
            input.setPixel(x_len, y_len, qRgba(qRed(pixel), qGreen(pixel), qBlue(pixel), int((255 * (float(y_len))/float(useAsHeight))) ) );
            y_len++;
        }
        x_len++;
    }

    //Switch back to the right rotation
    switch (edge) {
    case Qt::Edge::BottomEdge: break;
    case Qt::Edge::RightEdge: rotateImage(input, -90); break;
    case Qt::Edge::TopEdge: rotateImage(input, -180); break;
    case Qt::Edge::LeftEdge: rotateImage(input, 90); break;
    }
    return input;
}

//Red excludes green
//Blue exludes red
//Green excludes blue

//When using inclusion it is the opposite

QRgb pixelBlender(QRgb src, VisualEffect::ChannelSplitMode mode, int channel)
{
    QRgb output = 0;
    switch (mode) {
    case VisualEffect::ChannelSplitMode::Exclude: {
        output = src;
        if ((VisualEffect::ColorChannel::Red & channel) == VisualEffect::Red) {
            output = QRgb(qRgba(qRed(0), qGreen(output), qBlue(output), qAlpha(output)));
        } if ((VisualEffect::ColorChannel::Green & channel) == VisualEffect::Green) {
            output = QRgb(qRgba(qRed(output), qGreen(0), qBlue(output), qAlpha(output)));
        } if ((VisualEffect::ColorChannel::Blue & channel) == VisualEffect::Blue) {
            output = QRgb(qRgba(qRed(output), qGreen(output), qBlue(0), qAlpha(output)));
        } if ((VisualEffect::ColorChannel::Alpha & channel) == VisualEffect::Alpha) {
            output = QRgb(qRgba(qRed(output), qGreen(output), qBlue(output), qAlpha(0)));
        }
        break;
    }
    case VisualEffect::ChannelSplitMode::Include: {
        output = qRgba(0,0,0,0);
        if ((VisualEffect::ColorChannel::Red & channel) == VisualEffect::Red) {
            output = QRgb(qRgba(qRed(src), qGreen(output), qBlue(output), qAlpha(output)));
        } if ((VisualEffect::ColorChannel::Green & channel) == VisualEffect::Green) {
            output = QRgb(qRgba(qRed(output), qGreen(src), qBlue(output), qAlpha(output)));
        } if ((VisualEffect::ColorChannel::Blue & channel) == VisualEffect::Blue) {
            output = QRgb(qRgba(qRed(output), qGreen(output), qBlue(src), qAlpha(output)));
        } if ((VisualEffect::ColorChannel::Alpha & channel) == VisualEffect::Alpha) {
            output = QRgb(qRgba(qRed(output), qGreen(output), qBlue(output), qAlpha(src)));
        }
        break;
    }
    }
    return output;
}

QImage blendChannel(QImage src, int channel, VisualEffect::ChannelSplitMode mode, int radius)
{
    int x_len = 0, y_len = 0, useAsHeight = src.height(), useAsWidth = src.width();
    QImage output = src;

    QGraphicsBlurEffect *blur = new QGraphicsBlurEffect;
    blur->setBlurRadius(radius);
    output = applyEffectToImage(src, blur);

    while(x_len<useAsWidth) {
        y_len = 0;
        while(y_len<useAsHeight) {
            output.setPixel(x_len, y_len, pixelBlender(output.pixel(x_len, y_len), mode, channel));
            y_len++;
        }
        x_len++;
    }
    return output;
}

QImage splitChannel(QImage src, int channel, VisualEffect::ChannelSplitMode mode, VisualEffect::ChannelSplitDirection direction, int padding)
{
    int x_len = 0, y_len = 0, useAsHeight = src.height(), useAsWidth = src.width();
    QImage output = src;

    int pad = padding;
    bool hor = true;

    switch (direction) {
    case VisualEffect::ChannelSplitDirection::Top: hor = false; break;
    case VisualEffect::ChannelSplitDirection::Bottom: hor = false; pad = -padding; break;
    case VisualEffect::ChannelSplitDirection::Left: pad = -padding; break;
    case VisualEffect::ChannelSplitDirection::Right: break;
    }

    while(x_len<useAsWidth) {
        y_len = 0;
        while(y_len<useAsHeight) {
            QRgb pix = 0;
            bool skip = false;
            if (hor != true) {
                if ((y_len+pad) > -1 && (y_len + pad) < useAsHeight) {
                    pix = src.pixel(x_len, y_len + pad);
                } else {
                    skip = true;
                }
            } else {
                if ((x_len+pad) > -1 && (x_len + pad) < useAsWidth) {
                    pix = src.pixel(x_len + pad, y_len);
                } else {
                    skip = true;
                }
            }
            if (skip == false) {
                output.setPixel(x_len, y_len, pixelBlender(pix, mode, channel));
            }
            y_len++;
        }
        x_len++;
    }
    return output;
}

QImage bindToGS(QImage src)
{
    int x_len = 0, y_len = 0, useAsHeight = src.height(), useAsWidth = src.width();

    while(x_len<useAsWidth) {
        y_len = 0;
        while(y_len<useAsHeight) {
            QRgb pix = src.pixel(x_len, y_len);
            int flatten = (int(qRed(pix)) + int(qGreen(pix)) + int(qBlue(pix))) / 3;
            src.setPixel(x_len, y_len, QRgb(qRgba(flatten, flatten, flatten, 255)));
            y_len++;
        }
        x_len++;
    }

    return src;
}

QImage bindToGSA(QImage src)
{
    int x_len = 0, y_len = 0, useAsHeight = src.height(), useAsWidth = src.width();

    while(x_len<useAsWidth) {
        y_len = 0;
        while(y_len<useAsHeight) {
            QRgb pix = src.pixel(x_len, y_len);
            int flatten = (int(qRed(pix)) + int(qGreen(pix)) + int(qBlue(pix))) / 3;
            src.setPixel(x_len, y_len, QRgb(qRgba(flatten, flatten, flatten, qAlpha(pix))));
            y_len++;
        }
        x_len++;
    }

    return src;
}
