#include "GoogleTest.h"

#include <QProcess>

#include <sstream>

#include "utilities.h"
#include "TestDataJob.h"
#include "Roles.h"
#include "IconManager.h"

GoogleTest::GoogleTest(const std::string& executable, const std::string& name)
: _executable(executable), _name(name)
{
}

std::string GoogleTest::getName() const
{
    return _name;
}

TestState GoogleTest::getState() const
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

std::string GoogleTest::getHtmlDetailMessage() const
{
    const auto header = "Details of test: <b>" + getParent()->getName() + "." + getName() + "</b><br><br>";

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
            //trace(line);
        
            if(line.find("PASSED") != std::string::npos && line.find("0 tests.") != std::string::npos)
            {
                //trace("detected skipped");

                auto state = TestState();
                
                state.result = TestState::Result::Skipped;
                state.state = TestState::State::Stopped;
                
                return state;
            }
            
            const auto position = line.find(id);
            
            if(position == std::string::npos)
            {
                //trace("id not found");
                
                if(runDetected)
                    message += line + '\n';
                
                continue;
            }
            
            if(line[position - 1] != ' ')
            {
                //trace("no exact match");
                continue;
            }
            
            if(position + id.length() < line.length() && line[position + id.length()] != ' ')
            {
                //trace("no exact match: " + std::to_string(position + id.length()) + " < " + std::to_string(line.length()));
                continue;
            }
            
            if(line.find(" RUN ") != std::string::npos)
            {
                //trace("detected run");
                runDetected = true;
                continue;
            }
            
            if(line.find(" FAILED ") != std::string::npos)
            {
                //trace("detected error");
                const auto duration = line.substr(position + id.length() + 2);

                auto state = TestState();
                
                state.result = TestState::Result::Error;
                state.state = TestState::State::Stopped;
                state.durationInMilliseconds = std::stoi(duration);
                state.message = message;
                
                //trace("duration: " + std::to_string(state.durationInMilliseconds));
                //trace("message: " + message);

                return state;
            }
            if(line.find(" OK ") != std::string::npos)
            {
                //trace("detected success");
                const auto duration = line.substr(position + id.length() + 2);

                auto state = TestState();
                
                state.result = TestState::Result::Passed;
                state.state = TestState::State::Stopped;
                state.durationInMilliseconds = std::stoi(duration);
                
                //trace("duration: " + std::to_string(state.durationInMilliseconds));
                
                return state;
            }
        }
        
        return {};
    }
}

void GoogleTest::start() 
{
    _state.state = TestState::State::Started;
    _state.result = TestState::Result::NotRun;

    setIcon(IconManager().getIcon(_state));
    
    if(getParent())
        getParent()->start();
}

void GoogleTest::execute() 
{
    const auto id = getParent()->getName() + "." + getName();

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
    
    auto time = std::string{"na"};
    
    if(_state.result == TestState::Result::Error || _state.result == TestState::Result::Passed)
        time = (_state.durationInMilliseconds == 0 ? std::string{"< 1"} : std::to_string(_state.durationInMilliseconds)) + " ms";
    
    setTime(time);
    
    getParent()->execute();
}

void GoogleTest::setParent(TestPtr parent)
{
    _parent = parent;
}

TestPtr GoogleTest::getParent() const
{
    return _parent;
}

std::list<TestPtr> GoogleTest::getChildren() const
{
    return {};
}

void GoogleTest::updateChildren(const TestNameProvider&, const TestCreator&)
{
}

QStandardItem* GoogleTest::getItem() const 
{
    return _item;
}

void GoogleTest::setItem(QStandardItem* item)
{
    _item = item;
}

void GoogleTest::setIcon(const QIcon& icon)
{
    auto item = getItem();

    if(item)
        item->setIcon(icon);
}

void GoogleTest::setTime(const std::string& text)
{
    if(!getItem() || !getItem()->parent() || !getItem()->row() || getItem()->parent()->columnCount() < 1)
        return;

    auto item = getItem()->parent()->child(getItem()->row(), 1);
    
    if(item)
        item->setText(text.c_str());
}

std::string GoogleTest::getExecutable() const
{
    return _executable;
}
