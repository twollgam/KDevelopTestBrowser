#include "GoogleTestGroup.h"

#include <QProcess>

#include <sstream>
#include <algorithm>

#include "utilities.h"
#include "TestDataJob.h"
#include "Roles.h"

GoogleTestGroup::GoogleTestGroup(const std::string& executable, const std::string& name)
: GoogleTest(executable, name), _executable(executable)
{
}
 
namespace
{
    bool testsAreRunning(const std::vector<TestPtr>& tests)
    {
        return std::any_of(tests.begin(), tests.end(), [](const TestPtr& test) { return test->getState().state != TestState::State::Stopped; });
    }
    
    size_t countExecuted(const std::vector<TestPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result != TestState::Result::NotRun;                
            });
    }    
    
    size_t countSuccess(const std::vector<TestPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result == TestState::Result::Passed;                
            });
    }
    
    size_t countFailed(const std::vector<TestPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result == TestState::Result::Error;                
            });
    }
    
    size_t countSkipped(const std::vector<TestPtr>& tests)
    {
        return std::count_if(tests.begin(), tests.end(), [](const TestPtr& test) 
            { 
                const auto& state = test->getState();
                
                return state.state == TestState::State::Stopped && state.result == TestState::Result::Skipped;                
            });
    }
    
}

std::string GoogleTestGroup::getHtmlDetailMessage() const
{   
    const auto header = "Details of group: <b>" + getName() + "</b><br><br>";

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
    GoogleTest::start();
}

void GoogleTestGroup::execute() 
{   
    const auto duration = std::accumulate(_tests.begin(), _tests.end(), 0UL, [](unsigned sum, TestPtr test){ return sum + test->getState().durationInMilliseconds; });
    const auto text = (duration == 0 ? std::string{"< 1"} : std::to_string(duration)) + " ms";
            
    setTime(text);
    
    getParent()->execute();
}

void GoogleTestGroup::addTest(const TestPtr& test)
{
    _tests.emplace_back(test);
}

std::list<TestPtr> GoogleTestGroup::getChildren() const
{
    return std::list<TestPtr>(_tests.begin(), _tests.end());
}

void GoogleTestGroup::updateChildren(const TestNameProvider&, const TestCreator&)
{
}

