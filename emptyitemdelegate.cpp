#include "emptyitemdelegate.h"

EmptyItemDelegate::EmptyItemDelegate(QObject *parent) : QItemDelegate(parent) {}
EmptyItemDelegate::~EmptyItemDelegate() {this->QItemDelegate::~QItemDelegate();}
void EmptyItemDelegate::paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const {}
