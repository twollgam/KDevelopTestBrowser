#pragma once

#include <QAction>
#include <interfaces/itestcontroller.h>
#include <project/projectmodel.h>
#include "ITestRunner.h"
#include "TextBrowser.h"
#include "TreeView.h"

#include <regex>
#include <set>
#include "ITestFilter.h"
#include "SelectAll.h"

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
    void showSource();

    void addTestSuite(KDevelop::ITestSuite* suite);
    void removeTestSuite(KDevelop::ITestSuite* suite);
    void updateTestSuite(KDevelop::ITestSuite* suite, const KDevelop::TestResult& result);
    void notifyTestCaseStarted(KDevelop::ITestSuite* suite, const QStringList& test_cases);
    QStandardItem* addProject(KDevelop::IProject* project);
    void removeProject(KDevelop::IProject* project);
    void onItemClicked(const QModelIndex&);

    QList<QAction*> contextMenuActions();

private:
    void runTests(const std::set<QStandardItem*>& items, const ITestFilter& = SelectAll());
    void runSelectedTests();
    void runFailedTests();
    void debugSelectedTests();
    void reload();
    void changeFilter(const QString &newFilter);
    void anchorClicked(const QUrl& link);
    void showSource(const std::string& file, int line);
    void showSource(const std::string& lineToSearch);
    void showSource(const std::regex& lineToSearch, const std::string& path);
    void showSource(const QModelIndex& index);
    void built(KDevelop::ProjectBaseItem*);


    QIcon iconForTestResult(KDevelop::TestResult::TestCaseResult result);
    QStandardItem* itemForSuite(KDevelop::ITestSuite* suite);
    QStandardItem* itemForProject(KDevelop::IProject* project);
    
    std::set<QStandardItem*> getSelectedItems(const QList<QModelIndex>& indexes);

private:
    TestBrowser* m_plugin;
    TreeView* m_tree;
    TextBrowser* _textBrowser;
    QStandardItemModel* m_model;
    QSortFilterProxyModel* m_filter;
    QList<QAction*> m_contextMenuActions;
    std::vector<TestRunnerPtr> _testRunners;
};


