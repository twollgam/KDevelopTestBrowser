#pragma once

#include "TreeView.h"

class InfoTextPainter : public ColumnPainter
{
public:
    ~InfoTextPainter() override = default;
    
    void paint(const TreeView&, int column, int row, QPainter&) const override;
};

