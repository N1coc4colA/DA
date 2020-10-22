#include "proxypainter.h"

LDA_BEGIN_NAMESPACE

ProxyPainter::ProxyPainter(QPainter *pa, QObject *src, bool lock_any)
{
    check = src;
    p = pa;
    any_way_lock = lock_any;
}

void ProxyPainter::setSource(QObject *checker, QPainter *pa)
{
    if (!any_way_lock) {
        if (this->check == checker) {
            p = pa;
        }
    }
}

LDA_END_NAMESPACE
