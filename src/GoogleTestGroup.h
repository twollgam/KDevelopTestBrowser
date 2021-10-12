#pragma once

#include "GoogleTest.h"

#include <string>

class GoogleTestGroup : public GoogleTest, public std::enable_shared_from_this<GoogleTestGroup>
{
public:
    GoogleTestGroup(const std::string& executable, const std::string& name);
    ~GoogleTestGroup() override = default;

    TestState getState() const override;
    std::string getHtmlDetailMessage() const override;
    
    void execute() override;
    
    void add(const TestPtr&);
    
    std::list<TestPtr> getChildren() const override;
    void updateChildren(const TestNameProvider&, const TestCreator&) override;
    
protected:
    void swap(std::vector<TestPtr>& tests);
    
private:
    std::vector<TestPtr> _tests;
};


