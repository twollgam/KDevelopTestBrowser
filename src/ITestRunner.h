#pragma once

#include <QStandardItem>

#include <memory>

struct ITestRunner
{
    virtual ~ITestRunner() = default;
    
    virtual bool isValid() const = 0;
    
    virtual void load(QStandardItem&) = 0;
};

using TestRunnerPtr = std::shared_ptr<ITestRunner>;

