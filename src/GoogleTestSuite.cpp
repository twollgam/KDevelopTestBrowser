#include "GoogleTestSuite.h"
#include "utilities.h"
#include "Roles.h"
#include "GoogleTestRunner.h"
#include "GoogleTestGroup.h"
#include "GoogleTest.h"

GoogleTestSuite::GoogleTestSuite(const std::string& executable, const std::string& name)
: _executable(executable), _name(name)
{
}

std::string GoogleTestSuite::getName() const
{
    return _name;
}

TestState GoogleTestSuite::getState() const
{
    return _state;
}

void GoogleTestSuite::execute()
{
    const auto parentItem = _item->parent();
    auto item = parentItem->child(_item->row(), 1);
    
    auto duration = 0UL;
    for(const auto& group : _tests)
        for(const auto& test : group->getChildren())
            duration += test->getState().durationInMilliseconds;
    
    if(duration == 0)
        item->setText("< 1 ms");
    else
    {
        const auto display = std::to_string(duration) + " ms";
            
        item->setText(display.c_str());
    }
}

void GoogleTestSuite::start()
{
}

std::string GoogleTestSuite::getHtmlDetailMessage() const
{
    const auto header = "Details of suite: <b>" + getName() + "</b><br><br>";

    return header;
/*
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
*/
}

void GoogleTestSuite::update()
{
    trace("GoogleTestSuite::update");
    
    auto runner = GoogleTestRunner(_executable);
    const auto testNames = GoogleTestRunner::loadTestNames(_executable);
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
            auto group = std::make_shared<GoogleTestGroup>(_executable, name);
            
            suite->add(group);
            group->setParent(suite);
            
            runner.create(group);
            
            return group;
        }
        
        auto group = std::dynamic_pointer_cast<GoogleTestGroup>(parent);
        auto test = std::make_shared<GoogleTest>(_executable, name);
            
        group->addTest(test);
        test->setParent(group);

        runner.create(test);
        
        return test;
    };
    
    updateChildren(provider, create);
}

void GoogleTestSuite::add(const TestPtr& test)
{
    _tests.emplace_back(test);
}

TestPtr GoogleTestSuite::getParent() const
{
    return {};
}

std::list<TestPtr> GoogleTestSuite::getChildren() const
{
    return std::list<TestPtr>(_tests.begin(), _tests.end());
}

void GoogleTestSuite::updateChildren(const TestNameProvider& nameProvider, const TestCreator& create)
{
    auto tests = std::vector<TestPtr>();
    const auto testNames = nameProvider(getName());
    
    for(auto testcase : _tests)
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
    
    _tests.swap(tests);
}

void GoogleTestSuite::setItem(QStandardItem* item)
{
    _item = item;
}

QStandardItem * GoogleTestSuite::getItem() const
{
    return _item;
}

