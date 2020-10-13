#include "elementlist.h"
#include "element.h"
#include "emptyitemdelegate.h"

ElementList::ElementList(QWidget *parent) : QListWidget(parent)
{
   // this->setItemDelegate(new EmptyItemDelegate());
    this->setAcceptDrops(true);
    this->setDragDropMode(QAbstractItemView::InternalMove);
    this->setDragEnabled(true);
    this->setSelectionMode(QAbstractItemView::SingleSelection);
}

ElementList::~ElementList()
{
    delete m_radius;
    this->QListWidget::~QListWidget();
}

void ElementList::startDrag(Qt::DropActions supportedActions)
{
    QDrag *drag = new QDrag(this);
    drag->setMimeData(model()->mimeData(selectedIndexes()));
    QPixmap pixmap(viewport()->visibleRegion().boundingRect().size());
    pixmap.fill(Qt::transparent);
    QPainter painter(&pixmap);
    for (QModelIndex index: selectedIndexes()) {
        painter.drawPixmap(visualRect(index), viewport()->grab(visualRect(index)));
    }
    drag->setPixmap(pixmap);
    drag->setHotSpot(viewport()->mapFromGlobal(QCursor::pos()));
    drag->exec(supportedActions, Qt::MoveAction);
}

void ElementList::setRadius(int r)
{
    m_radius = new int(r);
}

void ElementList::addElement(Element *e)
{
    e->rad = this->m_radius;
    QListWidgetItem* listItem = new QListWidgetItem;
    listItem->setSizeHint(e->sizeHint());
    this->addItem(listItem);
    this->setItemWidget(listItem, e);
}
void ElementList::setRadiusOnElement(Element *e, int *r)
{
    e->rad = r;
}
