#pragma once

#include "GoogleTest.h"

#include <string>

class GoogleTestGroup : public GoogleTest, public std::enable_shared_from_this<GoogleTestGroup>
{
public:
    GoogleTestGroup(const std::string& executable, const std::string& name);
    ~GoogleTestGroup() override = default;
    
    std::string getHtmlDetailMessage() const override;
    
    void start() override;
    void execute() override;
    
    void addTest(const TestPtr&);
    
    std::list<TestPtr> getChildren() const override;
    void updateChildren(const TestNameProvider&, const TestCreator&) override;
    
private:
    const std::string& _executable;
    std::vector<TestPtr> _tests;
};


