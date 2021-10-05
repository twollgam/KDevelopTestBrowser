#pragma once

#include "ITestRunner.h"

#include <interfaces/itestsuite.h>

class TestFactory
{
public:
    TestRunnerPtr create(const KDevelop::ITestSuite& suite);
};

