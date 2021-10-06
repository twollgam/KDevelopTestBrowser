#pragma once

#include "ITestData.h"

#include <string>

class GoogleTestGroup : public ITestData
{
public:
    GoogleTestGroup(const std::string& executable, const std::string& testcase);
    ~GoogleTestGroup() override = default;
    
    std::string getProjectName() const override;
    std::string getTestHostName() const override;
    std::string getTestCaseName() const override;
    std::string getTestName() const override;
    
    TestState getState() const override;
    std::string getHtmlDetailMessage() const override;
    
    void start() override;
    void execute() override;
    
    std::list<KJob*> createJobs(QStandardItem*) override;

    void addTest(const TestDataPtr&);
    
private:
    const std::string& _executable;
    const std::string _testcase;
    TestState _state;
    std::vector<TestDataPtr> _tests;
};


