#pragma once
 
#include "ITest.h"

#include <memory>

 struct ITestSuite : public virtual ITest
 {
     virtual ~ITestSuite() = default;
     
     virtual void update() = 0;
 };

 using TestSuitePtr = std::shared_ptr<ITestSuite>;
 
