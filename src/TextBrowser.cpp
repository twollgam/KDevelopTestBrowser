#include "TextBrowser.h"
#include "IconManager.h"

TextBrowser::TextBrowser(QWidget* parent)
: QTextBrowser(parent)
{
}

QVariant TextBrowser::loadResource(int type, const QUrl& name)
{
    const auto image = IconManager().getImage(name.toString().toStdString());
    
    if(image.isNull())
        return QVariant();
    
    return QVariant(image.scaledToHeight(16, Qt::SmoothTransformation));
}

