#include "GoogleTestSuite.h"
#include "utilities.h"
#include "Roles.h"
#include "GoogleTestRunner.h"
#include "GoogleTestGroup.h"
#include "GoogleTest.h"

GoogleTestSuite::GoogleTestSuite(const std::string& executable, const std::string& name)
: GoogleTestGroup(executable, name)
{
}

std::string GoogleTestSuite::getHtmlDetailMessage() const
{
    const auto children = getChildren();
    const auto groups = children.size();
    const auto tests = std::accumulate(children.begin(), children.end(), 0UL, [](unsigned sum, const TestPtr& test) { return sum + test->getChildren().size();});
    const auto header = "<h3>Suite Details</h3><h4>" + getName() + "</h4>"
        + " Groups in suite: " + std::to_string(groups) + "<br>"
        + " Tests in suite: " + std::to_string(tests) + "<br>";
    
    const auto state = getState();

    if(state.state != TestState::State::Stopped)
        return header + "Tests are running";
    
    return header;
/*
    const auto executed = countExecuted(_tests);
    const auto success = countSuccess(_tests);
    const auto failed = countFailed(_tests);
    const auto skipped = countSkipped(_tests);
    
    return header + 
        "&nbsp;&nbsp;<img src=\"Clock\">&nbsp;&nbsp;" + std::to_string(state.durationInMilliseconds) + " ms<br><br>" +
        "Results:<br>" +
        //std::to_string(executed) + " tests executed<br>" +
        "&nbsp;&nbsp;<img src=\"Passed\">&nbsp;&nbsp;" + std::to_string(success) + " tests passed<br>" +
        "&nbsp;&nbsp;<img src=\"Error\">&nbsp;&nbsp;" + std::to_string(failed) + " tests failed<br>" +
        "&nbsp;&nbsp;<img src=\"Skipped\">&nbsp;&nbsp;" + std::to_string(skipped) + " tests skipped<br>";
*/
}

void GoogleTestSuite::update()
{
    trace("GoogleTestSuite::update");
    
    auto runner = GoogleTestRunner(getExecutable());
    const auto testNames = GoogleTestRunner::loadTestNames(getExecutable());
    const auto provider = [&testNames](const std::string& name)
    {
        if(name.empty())
        {
            auto names = std::set<std::string>();
            
            for(const auto& item : testNames)
                names.emplace(item.first);
            
            return names;
        }
        
        return testNames.find(name)->second;
    };
    const auto create = [&runner,this](const TestPtr& parent, const std::string& name) -> TestPtr
    {
        if(!parent->getParent())
        {
            auto suite = std::dynamic_pointer_cast<GoogleTestSuite>(parent);
            auto group = std::make_shared<GoogleTestGroup>(getExecutable(), name);
            
            suite->add(group);
            group->setParent(suite);
            
            runner.create(group);
            
            return group;
        }
        
        auto group = std::dynamic_pointer_cast<GoogleTestGroup>(parent);
        auto test = std::make_shared<GoogleTest>(getExecutable(), name);
            
        group->add(test);
        test->setParent(group);

        runner.create(test);
        
        return test;
    };
    
    updateChildren(provider, create);
}

void GoogleTestSuite::updateChildren(const TestNameProvider& nameProvider, const TestCreator& create)
{
    auto tests = std::vector<TestPtr>();
    const auto testNames = nameProvider(getName());
    
    for(auto testcase : getChildren())
    {
        const auto it = testNames.find(testcase->getName());
        
        if(it != testNames.end())
            tests.emplace_back(testcase);
    }
    
    for(const auto& testname : testNames)
    {
        const auto it = std::find_if(tests.begin(), tests.end(), [testname](const TestPtr& test) { return test->getName() == testname;});  
        
        if(it != tests.end())
            tests.emplace_back(create(shared_from_this(), testname));
    }
    
    for(auto& test : tests)
        test->updateChildren(nameProvider, create);
    
    swap(tests);
}

