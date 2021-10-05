#include "InfoTextDelegate.h"

#include <QPainter>
#include <QTextDocument>
#include <QTextBlock>
#include <QAbstractTextDocumentLayout>
#include "utilities.h"
#include "GoogleTestMessage.h"

InfoTextDelegate::InfoTextDelegate(QTreeView* parent)
: QStyledItemDelegate(parent), _parent(parent)
{
}

void InfoTextDelegate::paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const
{
    //trace("InfoTextDelegate::paint: " + std::to_string(index.column()) + ":" + std::to_string(index.row()));

    QStyledItemDelegate::paint(painter, option, {});
        
    painter->save();
    
    auto&& textDocument = QTextDocument();
    
    textDocument.setHtml(GoogleTestMessage().toHtml(index.data().toString().toStdString()).c_str());

    auto context = QAbstractTextDocumentLayout::PaintContext();
    
    context.palette.setColor(QPalette::Link, QColor(100, 100, 255));
    
    textDocument.setPageSize( option.rect.size());
    painter->translate(option.rect.x(), option.rect.y());
    textDocument.documentLayout()->draw(painter, context);
        
    painter->restore();    
}

