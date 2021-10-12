#include "TestDataJob.h"

#include <QTimer>
#include "utilities.h"
#include "IconManager.h"

TestDataJob::TestDataJob(ITest& test, const QList<QStandardItem*>& items)
: _test(test), _items(items)
{
    trace("TestDataJob constructed");
}

void TestDataJob::start()
{
    trace("job started");
    QTimer::singleShot(1, this, &TestDataJob::startTest);
}

namespace
{
    void setIcon(QStandardItem& item, TestState state)
    {
        auto icon = IconManager().getIcon(state);
        
        item.setIcon(icon);
    }
}

void TestDataJob::startTest()
{
    trace("start test");
    
    _test.start();
    setIcon(*_items[0], _test.getState());

    QTimer::singleShot(1, this, &TestDataJob::executeTest);
}

void TestDataJob::executeTest()
{
    trace("execute test");
    
    _test.execute();
    
    trace("executed test");

    setIcon(*_items[0], _test.getState());
}
