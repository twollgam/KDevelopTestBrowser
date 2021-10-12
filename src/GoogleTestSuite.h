#pragma once

#include "ITestSuite.h"
#include "ITest.h"

class GoogleTestSuite : public ITestSuite, public std::enable_shared_from_this<GoogleTestSuite>
{
public:
    GoogleTestSuite(const std::string& executable, const std::string& name);
    ~GoogleTestSuite() override = default;
    
    std::string getName() const override;
    
    TestState getState() const override;
    std::string getHtmlDetailMessage() const override;

    void start() override;
    void execute() override;
    
    void update() override;
    
    void add(const TestPtr&);
        
    TestPtr getParent() const override;
    
    std::list<TestPtr> getChildren() const override;
    void updateChildren(const TestNameProvider&, const TestCreator&) override;

    QStandardItem* getItem() const override;
    void setItem(QStandardItem*) override;

private:
    std::string _executable;
    std::string _name;
    std::vector<TestPtr> _tests;
    TestState _state;
    QStandardItem* _item = nullptr;
};
