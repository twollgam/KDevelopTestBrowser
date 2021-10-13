#include "GoogleTestGroup.h"

#include <QProcess>

#include <sstream>
#include <algorithm>

#include "utilities.h"
#include "TestDataJob.h"
#include "Roles.h"
#include "IconManager.h"

GoogleTestGroup::GoogleTestGroup(const std::string& executable, const std::string& name)
: GoogleTest(executable, name)
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

TestState GoogleTestGroup::getState() const
{
    auto state = TestState();
    
    state.state = TestState::State::Stopped;
    state.result = TestState::Result::NotRun;
    
    if(testsAreRunning(_tests))
        state.state = TestState::State::Executing;
    else if(countFailed(_tests))
        state.result = TestState::Result::Error;
    else if(countSuccess(_tests))
        state.result = TestState::Result::Passed;
    
    const auto counter = [](unsigned sum, TestPtr test)
    {
        if(test->getState().state == TestState::State::Stopped)
            return sum + test->getState().durationInMilliseconds;
        return sum;
    };
    
    state.durationInMilliseconds =  std::accumulate(_tests.begin(), _tests.end(), 0UL, counter);

    return state;
}

std::string GoogleTestGroup::getHtmlDetailMessage() const
{   
    const auto header = "<h3>Group Details</h3><h4>" + getName() + "</h4>"
        + " Tests in group: "+ std::to_string(_tests.size()) + "<br>";
    
    if(testsAreRunning(_tests))
        return header + "Tests are running";
    
    const auto executed = countExecuted(_tests);
    const auto success = countSuccess(_tests);
    const auto failed = countFailed(_tests);
    const auto skipped = countSkipped(_tests);
    const auto state = getState();
    
    return header + 
        "&nbsp;&nbsp;<img src=\"Clock\">&nbsp;&nbsp;" + std::to_string(state.durationInMilliseconds) + " ms<br><br>" +
        "Results:<br>" +
        //std::to_string(executed) + " tests executed<br>" +
        "&nbsp;&nbsp;<img src=\"Passed\">&nbsp;&nbsp;" + std::to_string(success) + " tests passed<br>" +
        "&nbsp;&nbsp;<img src=\"Error\">&nbsp;&nbsp;" + std::to_string(failed) + " tests failed<br>" +
        "&nbsp;&nbsp;<img src=\"Skipped\">&nbsp;&nbsp;" + std::to_string(skipped) + " tests skipped<br>";
}

void GoogleTestGroup::execute() 
{
    const auto state = getState();
    
    setIcon(IconManager().getIcon(state));

    const auto duration = state.durationInMilliseconds;
    const auto text = (duration == 0 ? std::string{"< 1"} : std::to_string(duration)) + " ms";
            
    setTime(text);
    
    if(getParent())
        getParent()->execute();
}

void GoogleTestGroup::add(const TestPtr& test)
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

void GoogleTestGroup::swap(std::vector<TestPtr>& tests)
{
    _tests.swap(tests);
}

