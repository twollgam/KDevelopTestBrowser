#include "testbrowser.h"
#include "TestView.h"

#include <debug.h>
#include <interfaces/icore.h>
#include <interfaces/iuicontroller.h>
#include <interfaces/itestsuite.h>
#include <interfaces/itestcontroller.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/iproject.h>
#include <util/executecompositejob.h>

#include <KPluginFactory>
#include <KLocalizedString>
#include <KActionCollection>

#include <QAction>

using namespace KDevelop;

K_PLUGIN_FACTORY_WITH_JSON(TestBrowserFactory, "testbrowser.json", registerPlugin<TestBrowser>(); )

class TestToolViewFactory: public KDevelop::IToolViewFactory
{
public:
    explicit TestToolViewFactory( TestBrowser *plugin ): mplugin( plugin )
    {}
    QWidget* create( QWidget *parent = nullptr ) override
    {
        return new TestView( mplugin, parent );
    }
    Qt::DockWidgetArea defaultPosition() const override
    {
        return Qt::LeftDockWidgetArea;
    }
    QString id() const override
    {
        return QStringLiteral("org.kdevelop.TestBrowser");
    }
    QList< QAction* > contextMenuActions(QWidget* viewWidget) const override
    {
        return qobject_cast<TestView*>(viewWidget)->contextMenuActions();
    }
    
private:
    TestBrowser *mplugin;
};

TestBrowser::TestBrowser(QObject *parent, const QVariantList& args)
    : KDevelop::IPlugin(QStringLiteral("testbrowser"), parent)
{
    Q_UNUSED(args);

    qCDebug(PLUGIN_TESTBROWSER) << "Hello world, my plugin is loaded!";
    
    auto runAll = new QAction( QIcon::fromTheme(QStringLiteral("system-run")), i18nc("@action", "Run All Tests"), this );
    connect(runAll, &QAction::triggered, this, &TestBrowser::runAllTests);
    actionCollection()->addAction(QStringLiteral("run_all_tests"), runAll);
    
    auto stopTest = new QAction( QIcon::fromTheme(QStringLiteral("process-stop")), i18nc("@action", "Stop Running Tests"), this );
    connect(stopTest, &QAction::triggered, this, &TestBrowser::stopRunningTests);
    actionCollection()->addAction(QStringLiteral("stop_running_tests"), stopTest);

    setXMLFile(QStringLiteral("testbrowser.rc"));

    m_viewFactory = new TestToolViewFactory(this);
    core()->uiController()->addToolView(i18nc("@title:window", "Testbrowser"), m_viewFactory);
    
    connect(core()->runController(),&IRunController::jobRegistered, this, &TestBrowser::jobStateChanged);
    connect(core()->runController(),&IRunController::jobUnregistered, this, &TestBrowser::jobStateChanged);
    
    jobStateChanged();
}

void TestBrowser::unload()
{
    core()->uiController()->removeToolView(m_viewFactory);
}

void TestBrowser::runAllTests()
{
    ITestController* tc = core()->testController();
    const auto projects = core()->projectController()->projects();
    for (IProject* project : projects) {
        QList<KJob*> jobs;
        const auto suites = tc->testSuitesForProject(project);
        for (ITestSuite* suite : suites) {
            if (KJob* job = suite->launchAllCases(ITestSuite::Silent))
            {
                jobs << job;
            }
        }
        if (!jobs.isEmpty())
        {
            auto compositeJob = new KDevelop::ExecuteCompositeJob(this, jobs);
            compositeJob->setObjectName(i18np("Run 1 test in %2", "Run %1 tests in %2",
                                              jobs.size(), project->name()));
            compositeJob->setProperty("test_job", true);
            core()->runController()->registerJob(compositeJob);
        }
    }
}

void TestBrowser::stopRunningTests()
{
    const auto jobs = core()->runController()->currentJobs();
    for (KJob* job : jobs) {
        if (job->property("test_job").toBool())
        {
            job->kill();
        }
    }
}

void TestBrowser::jobStateChanged()
{
    const auto jobs = core()->runController()->currentJobs();
    const bool found = std::any_of(jobs.begin(), jobs.end(), [](KJob* job) {
        return (job->property("test_job").toBool());
    });

    actionCollection()->action(QStringLiteral("run_all_tests"))->setEnabled(!found);
    actionCollection()->action(QStringLiteral("stop_running_tests"))->setEnabled(found);
}


// needed for QObject class created from K_PLUGIN_FACTORY_WITH_JSON
#include "testbrowser.moc"
