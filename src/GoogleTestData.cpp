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
 
std::string GoogleTestData::getProjectName() const 
{
    return {};
}

std::string GoogleTestData::getTestHostName() const
{
    return _executable;
}


std::string GoogleTestData::getTestCaseName() const
{
    return _testcase;
}

std::string GoogleTestData::getTestName() const
{
    return _testname;
}

TestState GoogleTestData::getState() const
{
    return _state;
}

namespace
{
    std::string toHtml(const std::string& message)
    {
        //trace("toHtml: " + message);
        auto html = message;
        
        for(auto position = html.find('/'); position != std::string::npos; position = html.find('/', position + 1))
        {
            auto colonPosition = html.find(':', position);
            
            if(colonPosition != std::string::npos)
            {
                auto endPosition = html.find_first_not_of("0123456789", colonPosition + 1);
                
                if(endPosition != std::string::npos)
                {
                    const auto url = html.substr(position, endPosition - position);
                    
                    //trace("url: " + url);
                    
                    html.insert(endPosition, "</a>");
                    html.insert(position, "<a href=\"" + url + "\">");
                    
                    position = endPosition + 4 + 11 + url.size();

                    //html.insert(endPosition, "</u>");
                    //html.insert(position, "<u>");
                    
                    //position = endPosition + 4 + 3;
                    
                    continue;
                }
            }
        }
        
        for(auto position = html.find_first_of("\n"); position != std::string::npos; position = html.find_first_of("\n", position))
        {
            html.replace(position, 1, "<br>");
        }
                
        //trace("html: " + html);
        return html;
    }

}

std::string GoogleTestData::getHtmlDetailMessage() const
{
    if(_testname.empty())
    {
        const auto header = "Details of group: <b>" + getTestCaseName() + "</b><br><br>";
        
        return header;
    }

    const auto header = "Details of test: <b>" + getTestCaseName() + "." + getTestName() + "</b><br><br>";

    if(_state.state != TestState::State::Stopped)
        return header + "Running";
    
    if(_state.result == TestState::Result::Error)
        return header + "Test failed<br><br>" + toHtml(_state.message);
    
    if(_state.result == TestState::Result::Passed)
        return header + "Test success<br><br>" + toHtml(_state.message);
    
    if(_state.result == TestState::Result::Skipped)
        return header + "Test skipped<br><br>" + toHtml(_state.message);

    if(_state.result == TestState::Result::NotRun)
        return header + "Test not run<br><br>" + toHtml(_state.message);

    return header + "Unknown";
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
        
            if(line.find("PASSED") != std::string::npos && line.find("0 tests.") != std::string::npos)
            {
                trace("detected skipped");

                auto state = TestState();
                
                state.result = TestState::Result::Skipped;
                state.state = TestState::State::Stopped;
                
                return state;
            }
            
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
    
    jobs.push_back(new TestDataJob(*this, createItemList(item)));
    
    return jobs;
}


