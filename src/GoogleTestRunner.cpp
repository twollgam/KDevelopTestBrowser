#include "GoogleTestRunner.h"

#include <QByteArray>
#include <QProcess>

#include <fstream>
#include <sstream>

#include "utilities.h"
#include "GoogleTestData.h"
#include "IconManager.h"
#include "Roles.h"

GoogleTestRunner::GoogleTestRunner(const std::string& path, const KDevelop::ITestSuite& suite)
: _suite(suite), _path(path)
{
    trace("create GoogleTestRunner: " + path);
}

bool GoogleTestRunner::isValid() const
{
    auto&& process = QProcess();
    
    process.start(_path.c_str(), QStringList() << "-h");
    
    if (!process.waitForStarted())
        return false;
    if (!process.waitForFinished())
        return false;
    
    auto&& output = std::stringstream(process.readAll().toStdString());
    auto line = std::string{};
    
    while(std::getline(output, line))
    {
        if(line.find("--gtest_list_tests") != std::string::npos)
            return true;
    }
    
    return false;
}

void GoogleTestRunner::load(QStandardItem& item)
{
    trace("load");
    
    auto&& process = QProcess();
    
    process.start(_path.c_str(), QStringList() << "--gtest_list_tests");
    
    if (!process.waitForStarted())
        return;
    if (!process.waitForFinished())
        return;
    
    auto&& output = std::stringstream(process.readAll().toStdString());
    auto line = std::string{};
    auto testcase = std::string{};
    
    while(std::getline(output, line))
    {
        if(line[0] != ' ')
            testcase = line.substr(0, line.length() - 1);
        else
        {
            auto test = line.substr(1);
            
            _tests[testcase].emplace_back(test);
        }
    }
    
    auto manager = IconManager();
    
    for(const auto& testcase : _tests)
    {
        auto testcaseItem = new QStandardItem(manager.getIcon(KDevelop::TestResult::NotRun), testcase.first.c_str());
        {
            auto data = QVariant();
            auto testdata = TestDataPtr(std::make_shared<GoogleTestData>(_path, testcase.first));
            
            data.setValue(testdata);
            testcaseItem->setData(data, TestDataRole);
            testcaseItem->setData(testcase.first.c_str(), SuiteRole);
            item.appendRow(testcaseItem);
        }
        
        for(const auto& test : testcase.second)
        {
            const auto testname = test.substr(1);
            auto testItem = new QStandardItem(manager.getIcon(KDevelop::TestResult::NotRun), testname.c_str());
            auto data = QVariant();
            auto testdata = TestDataPtr(std::make_shared<GoogleTestData>(_path, testcase.first, testname));
            
            data.setValue(testdata);
            testItem->setData(data, TestDataRole);
            testItem->setData(testname.c_str(), CaseRole);
            
            testcaseItem->appendRow(QList<QStandardItem*>() << testItem << new QStandardItem("na") << new QStandardItem(""));
        }
    }
}


