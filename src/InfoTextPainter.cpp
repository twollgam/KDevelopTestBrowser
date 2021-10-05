#include "InfoTextPainter.h"

#include <QTextDocument>

void InfoTextPainter::paint(const TreeView& treeview, int column, int row, QPainter& painter) const
{
    auto&& textDocument = QTextDocument();
    
    textDocument.setHtml("Helo World");
    textDocument.drawContents(&painter);
}

