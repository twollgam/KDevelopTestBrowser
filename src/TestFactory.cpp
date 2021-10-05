#include "TestFactory.h"

#include "GoogleTestRunner.h"

#include <interfaces/iproject.h>
#include <interfaces/itestsuite.h>
#include <project/interfaces/iprojectfilemanager.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <project/projectmodel.h>
#include <util/path.h>

#include <QMap>
#include <QStringList>

#include "utilities.h"

namespace
{
    std::string getBuildFolder(const KDevelop::IProject& project, const KDevelop::ITestSuite& suite)
    {
        const auto manager = project.buildSystemManager();

        if(!manager)
        {
            trace("no manager");
            return {};
        }
        
        trace("got manager");
        auto&& item = KDevelop::ProjectBaseItem(suite.project(), suite.project()->name());
        const auto path = manager->buildDirectory(&item).toLocalFile().toStdString();
        
        trace("builddir: " + path);

        return path;
    }
}

TestRunnerPtr TestFactory::create(const KDevelop::ITestSuite& suite)
{
    const auto projectPath = suite.project()->path().toLocalFile().toStdString();
    
    trace("project path: " + projectPath);
    
    const auto projectItem = suite.project()->projectItem();
    
    trace("projectitem: " + projectItem->text().toStdString());
    trace("children: " + std::to_string(projectItem->children().count()));
    
    for(const auto& child : projectItem->children())
    {
        trace("child: " + child->text().toStdString());
    }

    trace("targets: " + std::to_string(projectItem->targetList().count()));
    
    for(const auto& target : projectItem->targetList())
    {
        trace("target: " + target->text().toStdString());
    }
        
    const auto buildFolder = getBuildFolder(*suite.project(), suite);
    
    // this is a hack until one can get the infos from system
    const auto path = buildFolder + "/" + suite.name().toStdString();    
    auto runner = std::make_shared<GoogleTestRunner>(path, suite);
    
    if(runner->isValid())
        return runner;
    
    return {};
}

