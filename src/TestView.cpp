#include "TestView.h"
#include "testbrowser.h"
#include "IconManager.h"
#include "Roles.h"
#include "TestFactory.h"
#include "utilities.h"

#include <debug.h>

#include <interfaces/icore.h>
#include <interfaces/iproject.h>
#include <interfaces/iprojectcontroller.h>
#include <interfaces/itestcontroller.h>
#include <interfaces/itestsuite.h>
#include <interfaces/iruncontroller.h>
#include <interfaces/ilauncher.h>
#include <interfaces/ilaunchconfiguration.h>
#include <interfaces/launchconfigurationtype.h>
#include <interfaces/idebugcontroller.h>
#include <debugger/interfaces/idebugsession.h>
#include <interfaces/idocumentcontroller.h>
#include <interfaces/isession.h>
#include <project/interfaces/ibuildsystemmanager.h>
#include <project/interfaces/iprojectbuilder.h>

#include <util/executecompositejob.h>

#include <language/duchain/indexeddeclaration.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/duchain.h>
#include <language/duchain/declaration.h>

#include <KActionCollection>
#include <KJob>
#include <KLocalizedString>
#include <KConfigGroup>
#if QT_VERSION < QT_VERSION_CHECK(5, 10, 0)
#include <KRecursiveFilterProxyModel>
#endif

#include <QAction>
#include <QHeaderView>
#include <QIcon>
#include <QLineEdit>
#include <QStandardItem>
#include <QStandardItemModel>
#include <QVBoxLayout>
#include <QWidgetAction>
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
#include <QSortFilterProxyModel>
#include "ITest.h"
#endif

#include "TestDataJob.h"
#include "InfoTextDelegate.h"
#include "ITestSuite.h"

#include <algorithm>
#include <regex>
#include "SelectFailed.h"

using KDevelop::ICore;

TestView::TestView(TestBrowser* plugin, QWidget* parent)
: QWidget(parent), m_plugin(plugin), m_tree(new TreeView(this)), _textBrowser(new TextBrowser(this))
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
, m_filter(new QSortFilterProxyModel(this))
#else
, m_filter(new KRecursiveFilterProxyModel(this))
#endif
{
    setWindowIcon(QIcon::fromTheme(QStringLiteral("preflight-verifier"), windowIcon()));
    setWindowTitle(i18nc("@title:window", "Testbrowser"));
    setMouseTracking(true);
    
    auto* layout = new QVBoxLayout(this);
    layout->setContentsMargins(0, 0, 0, 0);
    setLayout(layout);
    layout->addWidget(m_tree);
    layout->addWidget(_textBrowser);

    _textBrowser->setHtml("No information");
    _textBrowser->setOpenLinks(false);
    connect(_textBrowser, &QTextBrowser::anchorClicked, this, &TestView::anchorClicked);
    
    m_tree->setSortingEnabled(true);
    //m_tree->header()->hide();
    m_tree->setIndentation(10);
    m_tree->setEditTriggers(QTreeView::NoEditTriggers);
    m_tree->setSelectionBehavior(QTreeView::SelectRows);
    m_tree->setSelectionMode(QTreeView::ExtendedSelection);
    m_tree->setExpandsOnDoubleClick(false);
    m_tree->sortByColumn(0, Qt::AscendingOrder);

    connect(m_tree, &QAbstractItemView::clicked, this, &TestView::onItemClicked);

    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(3);
    m_model->setHeaderData(0, Qt::Horizontal, "Tests");
    m_model->setHeaderData(1, Qt::Horizontal, "Duration");   
    m_model->setHeaderData(2, Qt::Horizontal, "Message");   
    
#if QT_VERSION >= QT_VERSION_CHECK(5, 10, 0)
    m_filter->setRecursiveFilteringEnabled(true);
#endif
    m_filter->setSourceModel(m_model);
    m_tree->setModel(m_filter);
    m_tree->header()->setSectionResizeMode(QHeaderView::ResizeMode::ResizeToContents);
    m_tree->setItemDelegateForColumn(2, new InfoTextDelegate(m_tree));
    
    auto showSourceAction = new QAction( QIcon::fromTheme(QStringLiteral("code-context")), i18nc("@action:inmenu", "Show Source"), this );
    connect (showSourceAction, &QAction::triggered, this, [&](){ showSource(); });
    m_contextMenuActions << showSourceAction;

    auto runTest = new QAction(i18nc("@action:inmenu", "Run Tests"), this );
    connect (runTest, &QAction::triggered, this, &TestView::runSelectedTests);
    m_contextMenuActions << runTest;
    
    auto runFailedTest = new QAction(i18nc("@action:inmenu", "Run Failed Tests"), this );
    connect (runFailedTest, &QAction::triggered, this, &TestView::runFailedTests);
    m_contextMenuActions << runFailedTest;
    
    auto debugTest = new QAction( QIcon::fromTheme(QStringLiteral("system-debug")), i18nc("@action:inmenu", "Debug Test"), this );
    connect(debugTest, &QAction::triggered, this, &TestView::debugSelectedTests);
    m_contextMenuActions << debugTest;

    auto reloadTest = new QAction( QIcon::fromTheme(QStringLiteral("system-reload")), i18nc("@action:inmenu", "Reload"), this );
    connect(reloadTest, &QAction::triggered, this, &TestView::reload);
    m_contextMenuActions << reloadTest;


    addAction(plugin->actionCollection()->action(QStringLiteral("run_all_tests")));
    addAction(plugin->actionCollection()->action(QStringLiteral("stop_running_tests")));

    auto runSelected = new QAction( QIcon::fromTheme(QStringLiteral("system-run")), i18nc("@action", "Run Selected Tests"), this );
    connect (runSelected, &QAction::triggered, this, &TestView::runSelectedTests);
    addAction(runSelected);

    auto edit = new QLineEdit(parent);
    edit->setPlaceholderText(i18nc("@info:placeholder", "Filter..."));
    edit->setClearButtonEnabled(true);
    auto* widgetAction = new QWidgetAction(this);
    widgetAction->setDefaultWidget(edit);
    connect(edit, &QLineEdit::textChanged, this, &TestView::changeFilter);
    addAction(widgetAction);

    setFocusProxy(edit);

    auto pc = ICore::self()->projectController();
    connect (pc, &KDevelop::IProjectController::projectClosed, this, &TestView::removeProject);

    auto tc = ICore::self()->testController();
    connect(tc, &KDevelop::ITestController::testSuiteAdded, this, &TestView::addTestSuite);
    connect(tc, &KDevelop::ITestController::testSuiteRemoved, this, &TestView::removeTestSuite);
    connect(tc, &KDevelop::ITestController::testRunFinished, this, &TestView::updateTestSuite);
    connect(tc, &KDevelop::ITestController::testRunStarted, this, &TestView::notifyTestCaseStarted);

    const auto suites = tc->testSuites();
    for (auto suite : suites)
        addTestSuite(suite);
}

TestView::~TestView()
{
}

void TestView::onItemClicked(const QModelIndex& index)
{
    trace("onItemClicked: " + std::to_string(index.column()) + ":" + std::to_string(index.row()));

    const auto data = index.model()->data(index, TestDataRole); 
    
    if(!data.isValid())
    {
        trace("has no data");
        return;
    }
    
    trace("has data");
    auto testdata = data.value<TestPtr>();
    
    if(testdata)
    {
        trace("has TestData");
        trace("print info");

        _textBrowser->setHtml(testdata->getHtmlDetailMessage().c_str());
    }
}

void TestView::anchorClicked(const QUrl& link)
{
    const auto url = link.url().toStdString();
    const auto file = url.substr(0, url.find(':'));
    const auto line = std::stoi(url.substr(url.find(':') + 1));

    showSource(file, line - 1);
}

void TestView::showSource(const std::string& file, int line)
{
    auto documentController = ICore::self()->documentController();
    
    qCDebug(PLUGIN_TESTBROWSER) << "Activating declaration in" << file.c_str();
    
    documentController->openDocument(QUrl(("file://" + file).c_str()), KTextEditor::Cursor(line, 0));
}

void TestView::showSource(const std::string& lineToSearch)
{
    auto documentController = ICore::self()->documentController();

    for(auto document : documentController->openDocuments())
    {
        const auto text = document->textDocument()->text().toStdString();
        const auto position = text.find(lineToSearch);
        
        if(position != std::string::npos)
        {
            document->activeTextView();
            return;
        }
    }
}

void TestView::showSource(const std::regex& lineToSearch, const std::string& path)
{
    trace("TestView::showSource(const std::regex& lineToSearch)");

    auto documentController = ICore::self()->documentController();

    for(auto document : documentController->openDocuments())
    {
        if(!document->textDocument())
            continue;
        
        const auto file = document->url().path().toStdString();
        
        for(auto n = 0; n < document->textDocument()->lines(); ++n)
        {
            const auto line = document->textDocument()->line(n).toStdString();
        
            //trace("check: " + text);
            
            if(std::regex_search(line, lineToSearch))
            {
                trace("match " + file);
                
                showSource(file, n);
                return;
            }
        }
    }
}

void TestView::showSource(const QModelIndex& index)
{
    trace("TestView::showSource(const QModelIndex& index)");
    
    const auto data = index.model()->data(index, TestDataRole); 
    
    if(!data.isValid())
    {
        trace("has no data");
        return;
    }
    
    trace("has data");
    auto test = data.value<TestPtr>();
    
    if(test)
    {
        trace("has Test");
        trace("open document");

        if(!test->getParent())
            return;
     
        const auto path = "";
        
        if(!test->getParent()->getParent())
            showSource(std::regex("\\( *" + test->getName() + " *,.*\\)"), path);
        else            
            showSource(std::regex("\\( *" + test->getParent()->getName() + " *, *" + test->getName() + " *\\)"), path);
    }
}

void TestView::showSource()
{
    trace("TestView::showSource()");

    auto indexes = m_tree->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return;

    KDevelop::IndexedDeclaration declaration;
    auto tc = ICore::self()->testController();

    QModelIndex index = m_filter->mapToSource(indexes.first());
    
    showSource(index);
    
    QStandardItem* item = m_model->itemFromIndex(index);
    if (item->parent() == nullptr)
    {
        // No sense in finding source code for projects.
        return;
    }
    else if (item->parent()->parent() == nullptr)
    {
        auto project = ICore::self()->projectController()->findProjectByName(item->parent()->data(ProjectRole).toString());
        auto suite =  tc->findTestSuite(project, item->data(SuiteRole).toString());
        declaration = suite->declaration();
    }
    else
    {
        auto project = ICore::self()->projectController()->findProjectByName(item->parent()->parent()->data(ProjectRole).toString());
        auto suite =  tc->findTestSuite(project, item->parent()->data(SuiteRole).toString());
        if(suite)
            declaration = suite->caseDeclaration(item->data(CaseRole).toString());
    }

    KDevelop::DUChainReadLocker locker;
    auto d = declaration.data();
    if (!d)
        return;

    auto url = d->url().toUrl();
    auto cursor = d->rangeInCurrentRevision().start();
    locker.unlock();

    auto dc = ICore::self()->documentController();
    qCDebug(PLUGIN_TESTBROWSER) << "Activating declaration in" << url;
    dc->openDocument(url, cursor);
}

void TestView::updateTestSuite(KDevelop::ITestSuite* suite, const KDevelop::TestResult& result)
{
    auto item = itemForSuite(suite);
    if (!item)
        return;

    qCDebug(PLUGIN_TESTBROWSER) << "Updating test suite" << suite->name();

    item->setIcon(iconForTestResult(result.suiteResult));

    for (int i = 0; i < item->rowCount(); ++i)
    {
        qCDebug(PLUGIN_TESTBROWSER) << "Found a test case" << item->child(i)->text();
        auto caseItem = item->child(i);
        const auto resultIt = result.testCaseResults.constFind(caseItem->text());
        if (resultIt != result.testCaseResults.constEnd()) {
            caseItem->setIcon(iconForTestResult(*resultIt));
        }
    }
}

void TestView::changeFilter(const QString &newFilter)
{
    m_filter->setFilterWildcard(newFilter);
    if (newFilter.isEmpty()) {
        m_tree->collapseAll();
    } else {
        m_tree->expandAll();
    }
}

void TestView::notifyTestCaseStarted(KDevelop::ITestSuite* suite, const QStringList& test_cases)
{
    auto item = itemForSuite(suite);
    if (!item)
        return;

    qCDebug(PLUGIN_TESTBROWSER) << "Notify a test of the suite " << suite->name() << " has started";

    // Global test suite icon
    item->setIcon(QIcon::fromTheme(QStringLiteral("process-idle")));

    for (int i = 0; i < item->rowCount(); ++i)
    {
        qCDebug(PLUGIN_TESTBROWSER) << "Found a test case" << item->child(i)->text();
        QStandardItem* caseItem = item->child(i);
        if (test_cases.contains(caseItem->text()))
        {
            // Each test case icon
            caseItem->setIcon(QIcon::fromTheme(QStringLiteral("process-idle")));
        }
    }
}

QIcon TestView::iconForTestResult(KDevelop::TestResult::TestCaseResult result)
{
    return IconManager().getIcon(result);
}

QStandardItem* TestView::itemForSuite(KDevelop::ITestSuite* suite)
{
    const auto items = m_model->findItems(suite->name(), Qt::MatchRecursive);
    auto it = std::find_if(items.begin(), items.end(), [&](QStandardItem* item) {
        return (item->parent() && item->parent()->text() == suite->project()->name()
                && !item->parent()->parent());
    });
    return (it != items.end()) ? *it : nullptr;
}

QStandardItem* TestView::itemForProject(KDevelop::IProject* project)
{
    auto itemsForProject = m_model->findItems(project->name());
    
    if (!itemsForProject.isEmpty()) {
        return itemsForProject.first();
    }
    
    return addProject(project);
}

void TestView::reload()
{
    trace("reload");
    
    auto indexes = m_tree->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
    {
        //if there's no selection we'll run all of them (or only the filtered)
        //in case there's a filter.
        const int rc = m_filter->rowCount();
        indexes.reserve(rc);
        for(int i=0; i<rc; ++i) {
            indexes << m_filter->index(i, 0);
        }
    }

    for (const auto& idx : indexes) 
    {
        auto index = m_filter->mapToSource(idx);
        if (index.parent().isValid() && indexes.contains(index.parent()))
            continue;

        auto item = m_model->itemFromIndex(index);
        if(item->data(TestDataRole).isValid())
        {
            auto testdata = item->data(TestDataRole).value<TestPtr>();
            
            if(testdata)
            {
                auto suite = std::dynamic_pointer_cast<ITestSuite>(testdata);
                
                if(suite)
                    suite->update();
            }
        }
    }
}

void TestView::debugSelectedTests()
{
    trace("debug");
    
    const auto name = QString("DebugTest");
    const auto& configurations = ICore::self()->runController()->launchConfigurations();
    
    for(const auto& configuration : configurations)
    {
        trace("lc: " + configuration->name().toStdString());
        if(configuration->name() == name)
        {
            const auto config = configuration->config();
            
            for(const auto& item : config.keyList().toStdList())
                trace("key: " + item.toStdString());

            for(const auto& item : config.groupList().toStdList())
                trace("group: " + item.toStdString());

            for(const auto& item : config.entryMap().toStdMap())
                trace("item: " + item.first.toStdString() + "=" + item.second.toStdString());
            
            const auto cconfig = configuration->config().config();
            
            for(const auto& item : cconfig->entryMap().toStdMap())
                trace("citem: " + item.first.toStdString() + "=" + item.second.toStdString());

            for(const auto& item : cconfig->groupList().toStdList())
            {
                trace("cgroup: " + item.toStdString());
                for(const auto& item : cconfig->entryMap(item).toStdMap())
                    trace("citem: " + item.first.toStdString() + "=" + item.second.toStdString());
            }
            
            return;
        }
    }
    
    const auto& configurationTypes = ICore::self()->runController()->launchConfigurationTypes();

    KDevelop::LaunchConfigurationType* type = nullptr;
    
    for(const auto& item : configurationTypes)
    {
        trace("lct: " + item->name().toStdString());
        
        if(item->name() == "Compiled Binary")
        {
            type = item;
            
            break;
        }
    }
    
    if(!type)
        return;
    
    KDevelop::ILauncher* launcher = nullptr;
    
    for(const auto& item : type->launchers())
    {
        trace("l: " + item->name().toStdString());
        
        if(item->name() == "GDB")
        {
            launcher = item;
            break;
        }
    }
    
    if(!launcher)
        return;
    
    auto launcherPair = QPair<QString, QString>();
    
    launcherPair.first = "debug";
    launcherPair.second = launcher->id();
    
    //KDevelop::IProject* project = nullptr;
    auto project = ICore::self()->projectController()->findProjectByName("TestBrowser");

    if(!project)
        trace("no project found");

    
    auto config = ICore::self()->runController()->createLaunchConfiguration(type, launcherPair, project, name);
    
    for(const auto& group : config->config().groupList())
        trace("group: " + group.toStdString());
    
    trace("execute");
    
    auto job = ICore::self()->runController()->execute(QStringLiteral("debug"), config);
    
    if(!job)
        trace("no job created");
    else
        ICore::self()->runController()->registerJob(job);
}

namespace
{
    std::set<TestPtr> collectTests(TestPtr test, const ITestFilter& filter)
    {
        trace("collectTests: " + test->getName());
        
        auto tests = std::set<TestPtr>();
        
        if(test->getChildren().empty())
        {
            if(filter(test))
                tests.emplace(test);
        }
        else
        {
            for(auto& child : test->getChildren())
            {
                const auto children = collectTests(child, filter);
                
                std::copy_if(children.begin(), children.end(), std::inserter(tests, tests.begin()),
                    [&filter](TestPtr test){ return filter(test);}
                );
            }
        }
        
        return tests;
    }
    
    QList<QStandardItem*> createItemList(QStandardItem* item)
    {
        const auto parent = item->parent();

        return QList<QStandardItem*>() << item << parent->child(item->row(), 1) << parent->child(item->row(), 2);
    }

    KJob* createJob(TestPtr test)
    {
        trace("create job for " + test->getName());
        
        return new TestDataJob(*test, createItemList(test->getItem()));
    }
}

std::set<QStandardItem*> TestView::getSelectedItems(const QList<QModelIndex>& indexes)
{
    auto result = std::set<QStandardItem*>();
    
    for (const auto& idx : qAsConst(indexes)) 
    {
        auto index = m_filter->mapToSource(idx);
        
        trace("index column: " + std::to_string(index.column()));
        if(index.column() > 0)
            continue;
    
        if (index.parent().isValid() && indexes.contains(index.parent()))
            continue;

        auto item = m_model->itemFromIndex(index);
        
        if(item)
            result.insert(item);
    }
    
    return result;
}

void TestView::runFailedTests()
{
    trace("runFailedTests");
    
    auto indexes = m_tree->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
    {
        //if there's no selection we'll run all of them (or only the filtered)
        //in case there's a filter.
        const auto rc = m_filter->rowCount();
        indexes.reserve(rc);
        for(int i = 0; i < rc; ++i)
            indexes << m_filter->index(i, 0);
    }

    const auto items = getSelectedItems(indexes);

    runTests(items, SelectFailed());
}

void TestView::runSelectedTests()
{
    auto indexes = m_tree->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
    {
        //if there's no selection we'll run all of them (or only the filtered)
        //in case there's a filter.
        const auto rc = m_filter->rowCount();
        indexes.reserve(rc);
        for(int i = 0; i < rc; ++i)
            indexes << m_filter->index(i, 0);
    }

    const auto items = getSelectedItems(indexes);

    runTests(items);
}

void TestView::runTests(const std::set<QStandardItem*>& items, const ITestFilter& filter)
{
    auto jobs = QList<KJob*>();
    auto tc = ICore::self()->testController();

    /*
     * NOTE: If a test suite or a single test case was selected,
     * the job is launched in Verbose mode with raised output window.
     * If a project is selected, it is launched silently.
     *
     * This is the somewhat-intuitive approach. Maybe a configuration should be offered.
     */

    for (const auto& item : items) 
    {
        trace("item: " + item->text().toStdString());

        if(item->data(TestDataRole).isValid())
        {
            auto test = item->data(TestDataRole).value<TestPtr>();

            trace("has testdata: " + test->getName());
            
            if(test)
            {
                trace("create jobs");
                
                const auto tests = collectTests(test, filter);
                
                for(const auto& test : tests)
                    jobs << createJob(test);
            }
        }
        else
        {
            trace("has no testdata");
            
            if (item->parent() == nullptr)
            {
                trace("project: " + item->data(ProjectRole).toString().toStdString());
                // A project was selected
                auto project = ICore::self()->projectController()->findProjectByName(item->data(ProjectRole).toString());
                const auto suites = tc->testSuitesForProject(project);
                for (auto suite : suites) {
                    jobs << suite->launchAllCases(KDevelop::ITestSuite::Silent);
                }
            }
            else if (item->parent()->parent() == nullptr)
            {
                trace("project: " + item->parent()->data(ProjectRole).toString().toStdString());
                trace("suite: " + item->data(SuiteRole).toString().toStdString());
                // A suite was selected
                auto project = ICore::self()->projectController()->findProjectByName(item->parent()->data(ProjectRole).toString());
                auto suite = tc->findTestSuite(project, item->data(SuiteRole).toString());
                
                if(suite)
                    jobs << suite->launchAllCases(KDevelop::ITestSuite::Verbose);
            }
            else
            {
                // This was a single test case
                auto project = ICore::self()->projectController()->findProjectByName(item->parent()->parent()->data(ProjectRole).toString());
                auto suite = tc->findTestSuite(project, item->parent()->data(SuiteRole).toString());
                const auto testCase = item->data(CaseRole).toString();
                
                if(suite)
                    jobs << suite->launchCase(testCase, KDevelop::ITestSuite::Verbose);
            }
        }
    }

    if (!jobs.isEmpty())
    {
        //auto* compositeJob = new KDevelop::ExecuteCompositeJob(this, jobs);
        //compositeJob->setObjectName(i18np("Run 1 test", "Run %1 tests", jobs.size()));
        //compositeJob->setProperty("test_job", true);
        //ICore::self()->runController()->registerJob(compositeJob);
        for(auto& job : jobs) 
            ICore::self()->runController()->registerJob(job);
    }
}

void TestView::built(KDevelop::ProjectBaseItem *dom)
{
    trace("built: " + dom->project()->name().toStdString());
    
    auto item = itemForProject(dom->project());
    
    if(!item)
        return;
    
    if(item->data(TestDataRole).isValid())
    {
        trace("has TestData");
        auto testdata = item->data(TestDataRole).value<TestPtr>();
        
        if(testdata)
        {
            auto suite = std::dynamic_pointer_cast<ITestSuite>(testdata);
            
            if(suite)
                suite->update();
        }
    }    
}

void TestView::addTestSuite(KDevelop::ITestSuite* suite)
{
    trace("enter addTestSuite: " + suite->name().toStdString());
    
    auto builder = suite->project()->buildSystemManager()->builder();
    
    //connect(builder, &KDevelop::IProjectBuilder::built, this, &TestView::built);
    
    auto projectItem = itemForProject(suite->project());
    Q_ASSERT(projectItem);

    auto suiteItem = new QStandardItem(QIcon::fromTheme(QStringLiteral("view-list-tree")), suite->name());

    suiteItem->setData(suite->name(), SuiteRole);
    trace("has " + std::to_string(suite->cases().size()) + " testcases");

    if(suite->cases().empty())
    {
        auto runner = TestFactory().create(*suite);
        
        if(runner)
        {
            trace("got a runner");

            runner->load(*suiteItem);
            
            _testRunners.emplace_back(runner);
        }
    }
    else
    {
        for (const auto& testcaseName : suite->cases()) {
            trace("has testcase " + testcaseName.toStdString());
            
            auto caseItem = new QStandardItem(iconForTestResult(KDevelop::TestResult::NotRun), testcaseName);
            caseItem->setData(testcaseName, CaseRole);
            suiteItem->appendRow(caseItem);
        }
    }
    
    projectItem->appendRow(QList<QStandardItem*>() << suiteItem << new QStandardItem("na"));
}

void TestView::removeTestSuite(KDevelop::ITestSuite* suite)
{
    QStandardItem* item = itemForSuite(suite);
    item->parent()->removeRow(item->row());
}

QStandardItem* TestView::addProject(KDevelop::IProject* project)
{
    trace("enter addProject: " + project->name().toStdString());

    auto projectItem = new QStandardItem(QIcon::fromTheme(QStringLiteral("project-development")), project->name());
    
    projectItem->setData(project->name(), ProjectRole);
    m_model->appendRow(projectItem);
    
    return projectItem;
}

void TestView::removeProject(KDevelop::IProject* project)
{
    QStandardItem* projectItem = itemForProject(project);
    m_model->removeRow(projectItem->row());
}

QList<QAction*> TestView::contextMenuActions()
{
    return m_contextMenuActions;
}

