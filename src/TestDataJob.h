#pragma once

#include <kjob.h>

#include "ITest.h"

#include <QStandardItemModel>

class TestDataJob : public KJob 
{
public:
    TestDataJob(ITest& test, const QList<QStandardItem*>& items);
    ~TestDataJob() override = default;
    
    void start() override;
    
private:
    void startTest();
    void executeTest();
    
private:
    ITest& _test;
    QList<QStandardItem*> _items;
};

