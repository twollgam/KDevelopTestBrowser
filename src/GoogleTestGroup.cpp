#include "GoogleTestGroup.h"

#include <QProcess>

#include <sstream>
#include <algorithm>

#include "utilities.h"
#include "TestDataJob.h"
#include "Roles.h"

GoogleTestGroup::GoogleTestGroup(const std::string& executable, const std::string& testcase)
: _executable(executable), _testcase(testcase)
{
}
 
std::string GoogleTestGroup::getProjectName() const 
{
    return {};
}

std::string GoogleTestGroup::getTestHostName() const
{
    return _executable;
}


std::string GoogleTestGroup::getTestCaseName() const
{
    return _testcase;
}

std::string GoogleTestGroup::getTestName() const
{
    return {};
}

TestState GoogleTestGroup::getState() const
{
    return _state;
}

namespace
{
    bool testsAreRunning(const std::vector<TestDataPtr>& tests)
    {
        return std::any_of(tests.begin(), tests.end(), [](const TestDataPtr& test) { return test->getState().state != TestState::State::Stopped; });
    }
    
    size_t countExecuted(const std::vector<TestDataPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestDataPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result != TestState::Result::NotRun;                
            });
    }    
    
    size_t countSuccess(const std::vector<TestDataPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestDataPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result == TestState::Result::Passed;                
            });
    }
    
    size_t countFailed(const std::vector<TestDataPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestDataPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result == TestState::Result::Error;                
            });
    }
    
    size_t countSkipped(const std::vector<TestDataPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestDataPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result == TestState::Result::Skipped;                
            });
    }
    
}

std::string GoogleTestGroup::getHtmlDetailMessage() const
{   
    const auto header = "Details of group: <b>" + getTestCaseName() + "</b><br><br>";

    for(const auto& test : _tests)
        trace("test: " + std::to_string(size_t(test.get())));

    if(testsAreRunning(_tests))
        return header + "Test are running";
    
    const auto executed = countExecuted(_tests);
    const auto success = countSuccess(_tests);
    const auto failed = countFailed(_tests);
    const auto skipped = countSkipped(_tests);
    
    return header + 
        std::to_string(_tests.size()) + " tests in group<br>" +
        std::to_string(executed) + " tests executed<br>" +
        std::to_string(skipped) + " tests skipped<br>" +
        std::to_string(success) + " tests sucess<br>" +
        std::to_string(failed) + " tests failed" ;
}

void GoogleTestGroup::start() 
{
    _state.state = TestState::State::Started;
    _state.result = TestState::Result::NotRun;
}

void GoogleTestGroup::execute() 
{
}

namespace
{
    QList<QStandardItem*> createItemList(QStandardItem* item)
    {
        const auto parent = item->parent();

        return QList<QStandardItem*>() << item << parent->child(item->row(), 1) << parent->child(item->row(), 2);
    }
}

std::list<KJob *> GoogleTestGroup::createJobs(QStandardItem* item)
{
    auto jobs = std::list<KJob*>();
    
    auto parent = item;
    
    for(auto i = 0; i < parent->rowCount(); ++i)
    {
        auto item = parent->child(i);

        if(item->data(TestDataRole).isValid())
        {
            trace("has TestData");
            auto testdata = item->data(TestDataRole).value<TestDataPtr>();
            trace("has TestData");
            
            if(testdata)
                jobs.push_back(new TestDataJob(*testdata, createItemList(item)));
        }
    }
    
    return jobs;
}


void GoogleTestGroup::addTest(const TestDataPtr& test)
{
    _tests.emplace_back(test);
}

