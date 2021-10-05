#include "TestDataJob.h"

#include <QTimer>
#include "utilities.h"
#include "IconManager.h"

TestDataJob::TestDataJob(ITestData& testdata, const QList<QStandardItem*>& items)
: _testdata(testdata), _items(items)
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
    
    _testdata.start();
    setIcon(*_items[0], _testdata.getState());

    QTimer::singleShot(1, this, &TestDataJob::executeTest);
}

void TestDataJob::executeTest()
{
    trace("execute test");
    
    _testdata.execute();
    
    trace("executed test");

    setIcon(*_items[0], _testdata.getState());
    
    if(_testdata.getState().result != TestState::Result::NotRun && _items.size() > 1)
    {
        if(_testdata.getState().durationInMilliseconds == 0)
            _items[1]->setText("< 1 ms");
        else
        {
            const auto display = std::to_string(_testdata.getState().durationInMilliseconds) + " ms";
                
            _items[1]->setText(display.c_str());
        }
    }
    //if(_testdata.getState().result != TestState::Result::NotRun && _items.size() > 2)
    //{
    //    _items[2]->setText(_testdata.getState().message.c_str());
    //}
}
