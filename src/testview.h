#pragma once

#include <QAction>
#include <interfaces/itestcontroller.h>
#include "ITestRunner.h"
#include "TreeView.h"
#include <QTextBrowser>

class TestBrowser;

class QSortFilterProxyModel;
class QStandardItem;
class QStandardItemModel;

namespace KDevelop
{
    class ITestSuite;
}

class TestView : public QWidget
{
    Q_OBJECT
public:
    explicit TestView(TestBrowser* plugin, QWidget* parent = nullptr);
    ~TestView() override;

public Q_SLOTS:
    void runSelectedTests();
    void showSource();

    void addTestSuite(KDevelop::ITestSuite* suite);
    void removeTestSuite(KDevelop::ITestSuite* suite);
    void updateTestSuite(KDevelop::ITestSuite* suite, const KDevelop::TestResult& result);
    void notifyTestCaseStarted(KDevelop::ITestSuite* suite, const QStringList& test_cases);
    QStandardItem* addProject(KDevelop::IProject* project);
    void removeProject(KDevelop::IProject* project);
    void doubleClicked(const QModelIndex&);
    void onItemClicked(const QModelIndex&);

    QList<QAction*> contextMenuActions();

private:
    void changeFilter(const QString &newFilter);
    void anchorClicked(const QUrl& link);
    void showSource(const std::string& file, int line);


    QIcon iconForTestResult(KDevelop::TestResult::TestCaseResult result);
    QStandardItem* itemForSuite(KDevelop::ITestSuite* suite);
    QStandardItem* itemForProject(KDevelop::IProject* project);
    
private:
    TestBrowser* m_plugin;
    TreeView* m_tree;
    QTextBrowser* _textBrowser;
    QStandardItemModel* m_model;
    QSortFilterProxyModel* m_filter;
    QList<QAction*> m_contextMenuActions;
    std::vector<TestRunnerPtr> _testRunners;
};


