#pragma once

#include <interfaces/icore.h>
#include <interfaces/itestcontroller.h>
#include "ITest.h"

class IconManager
{
public:
    QIcon getIcon(KDevelop::TestResult::TestCaseResult result);
    QIcon getIcon(const TestState&);
    
    QImage getImage(const std::string& name);
};
