#include "dynamicsvgparser.h"

#include <iostream>
#include <QFile>
#include <QDomNode>
#include <QSvgRenderer>
#include <QPainter>
#include <QIcon>
#include <QPalette>
#include <QWidget>
#include <QMetaEnum>
#include <QTextStream>

static QMetaEnum m_enum = QMetaEnum::fromType<QPalette::ColorRole>();

QString dataParsing(QString input, QPalette *palette)
{
    if (input.startsWith("$")) {
        int value = m_enum.keyToValue(input.remove(0, 1).toUtf8());
        if (value != -1) {
            return palette->color(QPalette::ColorGroup::Current, static_cast<QPalette::ColorRole>(value)).name();
        } else {
            return input;
        }
    } else {
        return input;
    }
}

void SetAttrRecur(QDomElement &elem, QString strtagname, QString strattr, QString strattrval, bool state, QPalette *palette)
{
       // if it has the tagname
       if (elem.tagName().compare(strtagname) == 0)
       {
           QString data = elem.attribute(strattr);
           if (data.isEmpty() == false && data.startsWith("${") && data.endsWith("}")) {
               //Parse to use the right color!
               QStringList list = data.remove("${").remove("}").split("|");
               int i = 0;
               if (2 >= list.length() && list.isEmpty() == false) {
                   while (i < list.length()) {
                       QString out;
                       if (state) {
                           int toUse = 1;
                           if (list.length() == 1) {
                               toUse = 0;
                           }
                           out = dataParsing(list.at(toUse), palette);
                       } else {
                           out = dataParsing(list.at(0), palette);
                       }
                       elem.setAttribute(strattr, out);
                       i++;
                   }
               }
           }
       }
       // loop all children
       for (int i = 0; i < elem.childNodes().count(); i++)
       {
           if (!elem.childNodes().at(i).isElement())
           {
               continue;
           }
           QDomElement el = elem.childNodes().at(i).toElement();
           SetAttrRecur(el, strtagname, strattr, strattrval, state, palette);
           elem.replaceChild(el, elem.childNodes().at(i));
       }
}

QByteArray ChangeSVGColor(QByteArray src, bool state, QPalette *palette)
{
    QDomDocument doc;
    doc.setContent(src);
    QDomElement el = doc.documentElement();
    SetAttrRecur(el, "path", "fill", "", state, palette);
    QString str;
    QTextStream stream(&str);
    QDomNode node = el;
    node.save(stream,0);
    return doc.toByteArray();
}

QPixmap *svg2Pixmap(const QByteArray& svgContent, const QSize& size, QPainter::CompositionMode mode)
{
       QSvgRenderer rr(svgContent);
       QImage image(size.width(), size.height(), QImage::Format_ARGB32);
       QPainter painter(&image);
       painter.setCompositionMode(mode);
       image.fill(Qt::transparent);
       rr.render(&painter);
       return new QPixmap(QPixmap::fromImage(image));
}

QIcon svg2Icon(const QByteArray& svgContent, QPainter::CompositionMode mode = QPainter::CompositionMode_SourceOver)
{
       return QIcon(*svg2Pixmap(svgContent, QSize(128, 128), mode));
}

LDA_BEGIN_NAMESPACE

DynamicSvgParser::DynamicSvgParser(QByteArray arr, QPalette source, QObject *parent) : QObject(parent)
{
    src = source;
    data = arr;
}

void DynamicSvgParser::setRenderedSize(QSize size)
{
    rendered = size;
    mustReload = true;
}

void DynamicSvgParser::clearCache()
{
    if (firstVersion) {
        firstVersion->~QPixmap();
    }
    if (secondVersion) {
        secondVersion->~QPixmap();
    }
}

QPixmap *DynamicSvgParser::result()
{
    if (caching) {
        if (mustReload) {
            clearCache();
            mustReload = false;
        }
        if (toggled) {
            if (!firstVersion) {
                firstVersion = svg2Pixmap(ChangeSVGColor(data, toggled, &src), rendered, QPainter::CompositionMode::CompositionMode_Source);
            }
            return firstVersion;
        } else {
            if (!secondVersion) {
                secondVersion = svg2Pixmap(ChangeSVGColor(data, toggled, &src), rendered, QPainter::CompositionMode::CompositionMode_Source);
            }
            return secondVersion;
        }
    } else {
        if (mustReload) {
            mustReload = false;
        }
        return svg2Pixmap(ChangeSVGColor(data, toggled, &src), rendered, QPainter::CompositionMode::CompositionMode_Source);
    }
}

QSize DynamicSvgParser::renderedSize() { return rendered; }
bool DynamicSvgParser::isCaching() { return caching; }
bool DynamicSvgParser::isToggled() { return toggled; }
void DynamicSvgParser::toggle() { toggled = !toggled; }
void DynamicSvgParser::setToggled(bool tog) { toggled = tog; }
void DynamicSvgParser::setCaching(bool cac) { caching = cac; }
void DynamicSvgParser::setInput(QByteArray arr) { data = arr; }
void DynamicSvgParser::setSource(QPalette source) { src = source; }

LDA_END_NAMESPACE
