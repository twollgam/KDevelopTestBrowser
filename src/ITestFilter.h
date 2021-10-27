#pragma once

#include "ITest.h"

struct ITestFilter
{
    virtual ~ITestFilter() = default;
    
    virtual bool operator()(const TestPtr&) const = 0;
};

