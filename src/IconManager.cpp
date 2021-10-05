#include "IconManager.h"

#include <QIcon>

QIcon IconManager::getIcon(KDevelop::TestResult::TestCaseResult result)
{
    switch (result)
    {
        case KDevelop::TestResult::NotRun:
            return QIcon::fromTheme(QStringLiteral("star-new")); 

        case KDevelop::TestResult::Skipped:
            return QIcon::fromTheme(QStringLiteral("task-delegate"));

        case KDevelop::TestResult::Passed:
            return QIcon::fromTheme(QStringLiteral("dialog-ok-apply"));

        case KDevelop::TestResult::UnexpectedPass:
            // This is a very rare occurrence, so the icon should stand out
            return QIcon::fromTheme(QStringLiteral("dialog-warning"));

        case KDevelop::TestResult::Failed:
            return QIcon::fromTheme(QStringLiteral("dialog-cancel"));

        case KDevelop::TestResult::ExpectedFail:
            return QIcon::fromTheme(QStringLiteral("dialog-ok"));

        case KDevelop::TestResult::Error:
            return QIcon::fromTheme(QStringLiteral("window-close"));
    }
    Q_UNREACHABLE();
}

QIcon IconManager::getIcon(const TestState& state)
{
    if(state.state == TestState::State::Started)
        return QIcon::fromTheme(QStringLiteral("view-refresh"));
    if(state.state == TestState::State::Executing)
        return QIcon::fromTheme(QStringLiteral("view-refresh"));

    return getIcon(state.result);
}

