#pragma once

#include "ITestSuite.h"
#include "ITest.h"
#include "GoogleTestGroup.h"

class GoogleTestSuite : virtual public ITestSuite, virtual public GoogleTestGroup
{
public:
    GoogleTestSuite(const std::string& executable, const std::string& name);
    ~GoogleTestSuite() override = default;
        
    std::string getHtmlDetailMessage() const override;

    void update() override;
            
    void updateChildren(const TestNameProvider&, const TestCreator&) override;

private:
};
