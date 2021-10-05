#pragma once

#include "ITestRunner.h"

#include <string>
#include <map>
#include <vector>

#include <interfaces/itestsuite.h>

class GoogleTestRunner : public ITestRunner
{
public:
    GoogleTestRunner(const std::string& path, const KDevelop::ITestSuite&);
    
    bool isValid() const override;
    
    void load(QStandardItem& item) override;
    
private:
    const KDevelop::ITestSuite& _suite;
    const std::string _path;
    std::map<std::string, std::vector<std::string>> _tests;
};

