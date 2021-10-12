#pragma once

#include "ITestRunner.h"
#include "ITest.h"

#include <string>
#include <map>
#include <vector>

class GoogleTestRunner : public ITestRunner
{
public:
    GoogleTestRunner(const std::string& path);
    
    bool isValid() const override;
    
    void load(QStandardItem& item) override;
    
    void create(TestPtr);
    
    static std::map<std::string, std::set<std::string>> loadTestNames(const std::string& path);
    
private:
    const std::string _path;
    std::map<std::string, std::set<std::string>> _tests;
};

