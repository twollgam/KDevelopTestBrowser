#include "GoogleTestData.h"

#include <QProcess>

#include <sstream>

#include "utilities.h"
#include "TestDataJob.h"
#include "Roles.h"

GoogleTestData::GoogleTestData(const std::string& executable, const std::string& testcase, const std::string& testname)
: _executable(executable), _testcase(testcase), _testname(testname)
{
}
 
TestState GoogleTestData::getState() const
{
    return _state;
}

namespace
{
    TestState getState(const std::string& id, std::istream& stream)
    {
        auto line = std::string{};
        auto runDetected = false;
        auto message = std::string{};
        
        trace("result from " + id);
        while(std::getline(stream, line))
        {
            trace(line);
        
            const auto position = line.find(id);
            
            if(position == std::string::npos)
            {
                trace("id not found");
                
                if(runDetected)
                    message += line + '\n';
                
                continue;
            }
            
            if(line[position - 1] != ' ')
            {
                trace("no exact match");
                continue;
            }
            
            if(position + id.length() < line.length() && line[position + id.length()] != ' ')
            {
                trace("no exact match: " + std::to_string(position + id.length()) + " < " + std::to_string(line.length()));
                continue;
            }
            
            if(line.find(" RUN ") != std::string::npos)
            {
                trace("detected run");
                runDetected = true;
                continue;
            }
            
            if(line.find(" FAILED ") != std::string::npos)
            {
                trace("detected error");
                const auto duration = line.substr(position + id.length() + 2);

                auto state = TestState();
                
                state.result = TestState::Result::Error;
                state.state = TestState::State::Stopped;
                state.durationInMilliseconds = std::stoi(duration);
                state.message = message;
                
                trace("duration: " + std::to_string(state.durationInMilliseconds));
                trace("message: " + message);

                return state;
            }
            if(line.find(" OK ") != std::string::npos)
            {
                trace("detected success");
                const auto duration = line.substr(position + id.length() + 2);

                auto state = TestState();
                
                state.result = TestState::Result::Passed;
                state.state = TestState::State::Stopped;
                state.durationInMilliseconds = std::stoi(duration);
                
                trace("duration: " + std::to_string(state.durationInMilliseconds));
                
                return state;
            }
        }
        
        return {};
    }
}

void GoogleTestData::start() 
{
    _state.state = TestState::State::Started;
    _state.result = TestState::Result::NotRun;
}

void GoogleTestData::execute() 
{
    const auto id = _testcase + "." + (_testname.empty() ? std::string{"*"} :  _testname);

    trace("execute: " + id);

    auto&& process = QProcess();
    
    process.start(_executable.c_str(), QStringList() << ("--gtest_filter=" + id).c_str());
    
    if (!process.waitForStarted())
    {
        _state.state = TestState::State::Stopped;
        _state.result = TestState::Result::Failed;
        return;
    }
    if (!process.waitForFinished())
    {
        _state.state = TestState::State::Stopped;
        _state.result = TestState::Result::Failed;
        return;
    }
    
    auto&& output = std::stringstream(process.readAll().toStdString());
    
    _state = ::getState(id, output);
}

namespace
{
    QList<QStandardItem*> createItemList(QStandardItem* item)
    {
        const auto parent = item->parent();

        return QList<QStandardItem*>() << item << parent->child(item->row(), 1) << parent->child(item->row(), 2);
    }
}

std::list<KJob *> GoogleTestData::createJobs(QStandardItem* item)
{
    auto jobs = std::list<KJob*>();
    
    if(_testname.empty())
    {        
        auto parent = item;
        
        for(auto i = 0; i < parent->rowCount(); ++i)
        {
            auto item = parent->child(i);

            if(item->data(TestDataRole).isValid())
            {
                trace("has TestData");
                auto testdata = item->data(TestDataRole).value<TestDataPtr>();
                trace("has TestData");
                
                if(testdata)
                    jobs.push_back(new TestDataJob(*testdata, createItemList(item)));
            }
        }
    }
    else
        jobs.push_back(new TestDataJob(*this, createItemList(item)));
    
    return jobs;
}


