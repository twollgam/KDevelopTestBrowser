#ifndef TESTBROWSER_H
#define TESTBROWSER_H

#include <interfaces/iplugin.h>

class TestToolViewFactory;

class TestBrowser : public KDevelop::IPlugin
{
    Q_OBJECT

public:
    // KPluginFactory-based plugin wants constructor with this signature
    TestBrowser(QObject* parent, const QVariantList& args);

    void unload() override;
    
private Q_SLOTS:
    void runAllTests();
    void stopRunningTests();
    void jobStateChanged();

private:
    TestToolViewFactory* m_viewFactory = nullptr;
};

#endif // TESTBROWSER_H
