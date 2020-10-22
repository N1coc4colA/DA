#include "emptyitemdelegate.h"

LDA_BEGIN_NAMESPACE

EmptyItemDelegate::EmptyItemDelegate(QObject *parent) : QItemDelegate(parent) {}
EmptyItemDelegate::~EmptyItemDelegate() {this->QItemDelegate::~QItemDelegate();}
void EmptyItemDelegate::paint(QPainter *, const QStyleOptionViewItem &, const QModelIndex &) const {}

LDA_END_NAMESPACE
