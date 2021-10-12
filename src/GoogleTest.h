#pragma once

#include "ITest.h"

#include <string>

class GoogleTest : public virtual ITest
{
public:
    GoogleTest(const std::string& executable, const std::string& name);
    ~GoogleTest() override = default;
    
    std::string getName() const override;
    
    TestState getState() const override;
    std::string getHtmlDetailMessage() const override;
    
    void start() override;
    void execute() override;
    
    void setParent(TestPtr);
    TestPtr getParent() const override;
    std::list<TestPtr> getChildren() const override;
    void updateChildren(const TestNameProvider&, const TestCreator&) override;
    
    QStandardItem* getItem() const override;
    void setItem(QStandardItem*) override;

protected:
    void setIcon(const QIcon&);
    void setTime(const std::string&);
    std::string getExecutable() const;
    
private:
    const std::string& _executable;
    const std::string _name;
    TestState _state;
    TestPtr _parent;
    QStandardItem* _item = nullptr;
};

