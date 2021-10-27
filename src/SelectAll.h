#pragma once

#include "ITestFilter.h"

class SelectAll : public ITestFilter
{
public:
    bool operator()(const TestPtr &) const override;
};
