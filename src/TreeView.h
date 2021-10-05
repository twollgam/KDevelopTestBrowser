#pragma once

#include <QTreeView>

class TreeView;

struct ColumnPainter
{
    virtual ~ColumnPainter() = default;
    
    virtual void paint(const TreeView&, int column, int row, QPainter&) const = 0;
};

class TreeView : public QTreeView
{
public:
    TreeView(QWidget* parent = nullptr);
    
    TreeView& setPainter(int column, ColumnPainter*);
    
protected:
    void drawRow(QPainter * painter, const QStyleOptionViewItem & options, const QModelIndex & index) const override;
    //void mouseMoveEvent(QMouseEvent * event) override;
    
private:
    std::map<int, ColumnPainter*> _painters;
};

