#pragma once

#include <interfaces/icore.h>
#include <interfaces/itestcontroller.h>
#include "ITestData.h"

class IconManager
{
public:
    QIcon getIcon(KDevelop::TestResult::TestCaseResult result);
    QIcon getIcon(const TestState&);
};
