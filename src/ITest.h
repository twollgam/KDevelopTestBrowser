#pragma once

#include <any>
#include <functional>
#include <memory>
#include <set>
#include <string>

#include <QMetaType>
#include <QStandardItemModel>
#include <KJob>

#include <interfaces/itestcontroller.h>

struct TestState
{
    using Result = KDevelop::TestResult::TestCaseResult;
    enum class State { Stopped, Started, Executing };
    
    State state = State::Stopped;
    Result result = Result::NotRun;
    unsigned durationInMilliseconds = 0;
    std::string message;
};

struct ITest;

using TestPtr = std::shared_ptr<ITest>;

using TestNameProvider = std::function<std::set<std::string>(const std::string&)>;
using TestCreator = std::function<TestPtr(const TestPtr& parent, const std::string& testname)>;

struct ITest
{   
    virtual ~ITest() = default;
    
    virtual std::string getName() const = 0;
    
    virtual TestState getState() const = 0;
    virtual std::string getHtmlDetailMessage() const = 0;

    virtual void start() = 0;
    virtual void execute() = 0;
        
    virtual TestPtr getParent() const = 0;
    virtual std::list<TestPtr> getChildren() const = 0;
    virtual void updateChildren(const TestNameProvider&, const TestCreator&) = 0;
    
    virtual QStandardItem* getItem() const = 0;
    virtual void setItem(QStandardItem*) = 0;

};

Q_DECLARE_METATYPE(TestPtr)
