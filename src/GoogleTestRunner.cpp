#include "GoogleTestRunner.h"

#include <QByteArray>
#include <QProcess>

#include <fstream>
#include <sstream>

#include "utilities.h"
#include "GoogleTest.h"
#include "IconManager.h"
#include "Roles.h"
#include "GoogleTestGroup.h"
#include "GoogleTestSuite.h"

GoogleTestRunner::GoogleTestRunner(const std::string& path)
: _path(path)
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

void GoogleTestRunner::create(TestPtr test)
{
    trace("GoogleTestRunner::create(" + test->getName() + ")");
    
    auto manager = IconManager();

    if(!test->getParent())
    {
        trace("suite");
        
        test->getItem()->setIcon(manager.getIcon(KDevelop::TestResult::NotRun));
        
        return;
    }

    auto parentItem = test->getParent()->getItem();

    if(!parentItem)
    {
        trace("parent item is null");
        return;
    }

    auto item = new QStandardItem(manager.getIcon(KDevelop::TestResult::NotRun), test->getName().c_str());
    auto data = QVariant();
    
    data.setValue(test);
    item->setData(data, TestDataRole);
    parentItem->appendRow(QList<QStandardItem*>() << item << new QStandardItem("na") << new QStandardItem(""));      
    test->setItem(item);

    if(!test->getParent()->getParent())
    {
        trace("group");
        
        item->setData(test->getName().c_str(), SuiteRole);
        
        return;
    }

    trace("test");
    
    item->setData(test->getName().c_str(), CaseRole);    
}

void GoogleTestRunner::load(QStandardItem& item)
{
    trace("load");
    
    _tests = loadTestNames(_path);
    
    auto suite = std::make_shared<GoogleTestSuite>(_path, item.data(SuiteRole).toString().toStdString());
    
    suite->setItem(&item);
 
    auto data = QVariant();
        
    data.setValue(TestPtr(suite));
    item.setData(data, TestDataRole);
    
    for(const auto& testGroup : _tests)
    {
        const auto& testGroupName = testGroup.first;
        auto testgroup = std::make_shared<GoogleTestGroup>(_path, testGroupName);
        
        suite->add(testgroup);
        testgroup->setParent(suite);
        
        create(testgroup);
        
        for(const auto& dottedTestName : testGroup.second)
        {
            const auto testname = dottedTestName.substr(1);
            auto test = std::make_shared<GoogleTest>(_path, testname);

            testgroup->addTest(test);
            test->setParent(testgroup);
 
            create(test);
         }
    }
}

std::map<std::string, std::set<std::string> > GoogleTestRunner::loadTestNames(const std::string& path)
{
    auto&& process = QProcess();
    
    process.start(path.c_str(), QStringList() << "--gtest_list_tests");
    
    if (!process.waitForStarted())
        return {};
    if (!process.waitForFinished())
        return {};
    
    auto&& output = std::stringstream(process.readAll().toStdString());
    auto line = std::string{};
    auto testcase = std::string{};
    auto result = std::map<std::string, std::set<std::string>>();
    
    while(std::getline(output, line))
    {
        if(line[0] != ' ')
            testcase = line.substr(0, line.length() - 1);
        else
        {
            auto test = line.substr(1);
            
            result[testcase].emplace(test);
        }
    }
    
    return result;
}

