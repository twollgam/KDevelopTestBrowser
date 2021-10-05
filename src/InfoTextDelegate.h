#pragma once

#include <QStyledItemDelegate>
#include <QTreeView>

class InfoTextDelegate : public QStyledItemDelegate
{
    Q_OBJECT
public:
    InfoTextDelegate(QTreeView* parent);
    
    void paint(QPainter *painter, const QStyleOptionViewItem &option,  const QModelIndex &index) const override;
    
private:
    QTreeView* _parent;
};

