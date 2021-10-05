#include "testview.h"
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
#include <interfaces/idocumentcontroller.h>
#include <interfaces/isession.h>

#include <util/executecompositejob.h>

#include <language/duchain/indexeddeclaration.h>
#include <language/duchain/duchainlock.h>
#include <language/duchain/duchain.h>
#include <language/duchain/declaration.h>

#include <KActionCollection>
#include <KJob>
#include <KLocalizedString>
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
#include "ITestData.h"
#endif

#include "TestDataJob.h"
#include "InfoTextDelegate.h"
#include "InfoTextPainter.h"

#include <string>
#include "GoogleTestMessage.h"

using namespace KDevelop;
//using namespace std::literals::string_literals;


TestView::TestView(TestBrowser* plugin, QWidget* parent)
: QWidget(parent), m_plugin(plugin), m_tree(new TreeView(this)), _textBrowser(new QTextBrowser(this))
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
    connect(m_tree, &QTreeView::doubleClicked, this, &TestView::doubleClicked);
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
    //m_tree->setPainter(2, new InfoTextPainter());
    
    auto showSource = new QAction( QIcon::fromTheme(QStringLiteral("code-context")), i18nc("@action:inmenu", "Show Source"), this );
    //connect (showSource, &QAction::triggered, this, &TestView::showSource);
    m_contextMenuActions << showSource;

    auto runTest = new QAction(i18nc("@action:inmenu", "Run Test"), this );
    connect (runTest, &QAction::triggered, this, &TestView::runSelectedTests);
    m_contextMenuActions << runTest;

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
    connect (pc, &IProjectController::projectClosed, this, &TestView::removeProject);

    auto tc = ICore::self()->testController();
    connect(tc, &ITestController::testSuiteAdded, this, &TestView::addTestSuite);
    connect(tc, &ITestController::testSuiteRemoved, this, &TestView::removeTestSuite);
    connect(tc, &ITestController::testRunFinished, this, &TestView::updateTestSuite);
    connect(tc, &ITestController::testRunStarted, this, &TestView::notifyTestCaseStarted);
    
    const auto suites = tc->testSuites();
    for (auto suite : suites) {
        addTestSuite(suite);
    }
}

TestView::~TestView()
{
}

void TestView::doubleClicked(const QModelIndex& index)
{
    trace("doubleClicked: " + std::to_string(index.column()) + ":" + std::to_string(index.row()));

    m_tree->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect);
    runSelectedTests();
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
    auto testdata = data.value<TestDataPtr>();
    
    if(testdata)
    {
        trace("has TestData");
        trace("print info");

        const auto& message = testdata->getState().message;
        
        if(message.empty())
            _textBrowser->setHtml("No information");
        else
            _textBrowser->setHtml(GoogleTestMessage().toHtml(message).c_str());
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

void TestView::showSource()
{
    auto indexes = m_tree->selectionModel()->selectedIndexes();
    if (indexes.isEmpty())
        return;

    IndexedDeclaration declaration;
    auto tc = ICore::self()->testController();

    QModelIndex index = m_filter->mapToSource(indexes.first());
    QStandardItem* item = m_model->itemFromIndex(index);
    if (item->parent() == nullptr)
    {
        // No sense in finding source code for projects.
        return;
    }
    else if (item->parent()->parent() == nullptr)
    {
        IProject* project = ICore::self()->projectController()->findProjectByName(item->parent()->data(ProjectRole).toString());
        ITestSuite* suite =  tc->findTestSuite(project, item->data(SuiteRole).toString());
        declaration = suite->declaration();
    }
    else
    {
        IProject* project = ICore::self()->projectController()->findProjectByName(item->parent()->parent()->data(ProjectRole).toString());
        ITestSuite* suite =  tc->findTestSuite(project, item->parent()->data(SuiteRole).toString());
        if(suite)
            declaration = suite->caseDeclaration(item->data(CaseRole).toString());
    }

    DUChainReadLocker locker;
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

void TestView::updateTestSuite(ITestSuite* suite, const TestResult& result)
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

void TestView::notifyTestCaseStarted(ITestSuite* suite, const QStringList& test_cases)
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

QIcon TestView::iconForTestResult(TestResult::TestCaseResult result)
{
    return IconManager().getIcon(result);
}

QStandardItem* TestView::itemForSuite(ITestSuite* suite)
{
    const auto items = m_model->findItems(suite->name(), Qt::MatchRecursive);
    auto it = std::find_if(items.begin(), items.end(), [&](QStandardItem* item) {
        return (item->parent() && item->parent()->text() == suite->project()->name()
                && !item->parent()->parent());
    });
    return (it != items.end()) ? *it : nullptr;
}

QStandardItem* TestView::itemForProject(IProject* project)
{
    auto itemsForProject = m_model->findItems(project->name());
    
    if (!itemsForProject.isEmpty()) {
        return itemsForProject.first();
    }
    
    return addProject(project);
}

void TestView::runSelectedTests()
{
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

    QList<KJob*> jobs;
    auto tc = ICore::self()->testController();

    /*
     * NOTE: If a test suite or a single test case was selected,
     * the job is launched in Verbose mode with raised output window.
     * If a project is selected, it is launched silently.
     *
     * This is the somewhat-intuitive approach. Maybe a configuration should be offered.
     */

    for (const QModelIndex& idx : qAsConst(indexes)) 
    {
        QModelIndex index = m_filter->mapToSource(idx);
        if (index.parent().isValid() && indexes.contains(index.parent()))
            continue;

        auto item = m_model->itemFromIndex(index);
        if(item->data(TestDataRole).isValid())
        {
            trace("has TestData");
            auto testdata = item->data(TestDataRole).value<TestDataPtr>();
            trace("has TestData");
            
            if(testdata)
            {
                trace("create jobs");
                
                for(const auto& job : testdata->createJobs(item))
                    jobs << job;
            }
        }
        else if (item->parent() == nullptr)
        {
            // A project was selected
            IProject* project = ICore::self()->projectController()->findProjectByName(item->data(ProjectRole).toString());
            const auto suites = tc->testSuitesForProject(project);
            for (ITestSuite* suite : suites) {
                jobs << suite->launchAllCases(ITestSuite::Silent);
            }
        }
        else if (item->parent()->parent() == nullptr)
        {
            // A suite was selected
            IProject* project = ICore::self()->projectController()->findProjectByName(item->parent()->data(ProjectRole).toString());
            ITestSuite* suite =  tc->findTestSuite(project, item->data(SuiteRole).toString());
            jobs << suite->launchAllCases(ITestSuite::Verbose);
        }
        else
        {
            // This was a single test case
            auto project = ICore::self()->projectController()->findProjectByName(item->parent()->parent()->data(ProjectRole).toString());
            auto suite = tc->findTestSuite(project, item->parent()->data(SuiteRole).toString());
            const auto testCase = item->data(CaseRole).toString();
            
            if(suite)
                jobs << suite->launchCase(testCase, ITestSuite::Verbose);
        }
    }

    if (!jobs.isEmpty())
    {
        //auto* compositeJob = new KDevelop::ExecuteCompositeJob(this, jobs);
        //compositeJob->setObjectName(i18np("Run 1 test", "Run %1 tests", jobs.size()));
        //compositeJob->setProperty("test_job", true);
        for(auto& job : jobs) 
            ICore::self()->runController()->registerJob(job);
    }
}

void TestView::addTestSuite(ITestSuite* suite)
{
    trace("enter addTestSuite: " + suite->name().toStdString());
    
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
            
            auto caseItem = new QStandardItem(iconForTestResult(TestResult::NotRun), testcaseName);
            caseItem->setData(testcaseName, CaseRole);
            suiteItem->appendRow(caseItem);
        }
    }
    
    projectItem->appendRow(suiteItem);
}

void TestView::removeTestSuite(ITestSuite* suite)
{
    QStandardItem* item = itemForSuite(suite);
    item->parent()->removeRow(item->row());
}

QStandardItem* TestView::addProject(IProject* project)
{
    trace("enter addProject: " + project->name().toStdString());

    auto projectItem = new QStandardItem(QIcon::fromTheme(QStringLiteral("project-development")), project->name());
    
    projectItem->setData(project->name(), ProjectRole);
    m_model->appendRow(projectItem);
    
    return projectItem;
}

void TestView::removeProject(IProject* project)
{
    QStandardItem* projectItem = itemForProject(project);
    m_model->removeRow(projectItem->row());
}

QList<QAction*> TestView::contextMenuActions()
{
    return m_contextMenuActions;
}

