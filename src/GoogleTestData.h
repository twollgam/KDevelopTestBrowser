#pragma once

#include "ITestData.h"

#include <string>

class GoogleTestData : public ITestData
{
public:
    GoogleTestData(const std::string& executable, const std::string& testcase, const std::string& testname = {});
    ~GoogleTestData() override = default;
    
    TestState getState() const override;
    
    void start() override;
    void execute() override;
    
    std::list<KJob*> createJobs(QStandardItem*) override;

private:
    const std::string& _executable;
    const std::string _testcase;
    const std::string _testname;
    TestState _state;
};

