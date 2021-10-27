#include "SelectFailed.h"

bool SelectFailed::operator()(const TestPtr& test) const
{
    return test->getChildren().empty() && test->getState().result == TestState::Result::Error;
}

