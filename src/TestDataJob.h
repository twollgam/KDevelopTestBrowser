#pragma once

#include <kjob.h>

#include "ITestData.h"

#include <QStandardItemModel>

class TestDataJob : public KJob 
{
public:
    TestDataJob(ITestData& testdata, const QList<QStandardItem*>& items);
    ~TestDataJob() override = default;
    
    void start() override;
    
private:
    void startTest();
    void executeTest();
    
private:
    ITestData& _testdata;
    QList<QStandardItem*> _items;
};

