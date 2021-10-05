#include "TreeView.h"

#include "utilities.h"
#include <string>

TreeView::TreeView(QWidget* parent)
: QTreeView(parent)
{
}

TreeView & TreeView::setPainter(int column, ColumnPainter* painter)
{
    _painters[column] = painter;
    
    return *this;
}

void TreeView::drawRow(QPainter* painter, const QStyleOptionViewItem& options, const QModelIndex& index) const
{
    //trace("drawRow: " + std::to_string(index.column()) + ":" + std::to_string(index.row()));
    
    const auto it = _painters.find(index.column());
    
    if(it == _painters.end())
        QTreeView::drawRow(painter, options, index);
    else
        it->second->paint(*this, index.column(), index.row(), *painter);
}

/*
void TreeView::mouseMoveEvent(QMouseEvent* event)
{
    QTreeView::mouseMoveEvent(event);
    
    
}
*/
