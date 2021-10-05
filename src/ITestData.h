#pragma once

#include <memory>
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

struct ITestData
{   
    virtual ~ITestData() = default;
    
    virtual TestState getState() const = 0;

    virtual void start() = 0;
    virtual void execute() = 0;
    
    virtual std::list<KJob*> createJobs(QStandardItem*) = 0;
};

using TestDataPtr = std::shared_ptr<ITestData>;

Q_DECLARE_METATYPE(TestDataPtr)
