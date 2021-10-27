#pragma once

#include "ITestFilter.h"

class SelectFailed : public ITestFilter
{
public:
    bool operator()(const TestPtr & ) const override;
};
