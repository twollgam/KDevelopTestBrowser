#pragma once

#include <QTextBrowser>

class TextBrowser : public QTextBrowser
{
public:
    TextBrowser(QWidget* parent);
    
    QVariant loadResource(int type, const QUrl & name) override;
};
