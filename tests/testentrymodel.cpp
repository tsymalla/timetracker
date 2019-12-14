#include "testentrymodel.h"
#include <QDateTime>

TestEntryModel::TestEntryModel()
{
}

void TestEntryModel::init()
{
    _provider = new DataProvider(this);
}

void TestEntryModel::cleanup()
{
    delete _provider;
}

void TestEntryModel::initTestCase()
{
    _model = new EntryModel(this, _provider);
}

void TestEntryModel::cleanupTestCase()
{
    delete _model;
}

void TestEntryModel::testAddEntryIncreasesByOne()
{
    const auto currentEntryCount = _model->getRows().length();
    Entry e;
    e.taskId = 1;
    e.from = QDateTime::currentDateTime();
    e.until = e.from.addDays(1);
    e.entryContent = "Test";

    _model->addRow(e);

    const auto newEntryCount = _model->getRows().length();
    QVERIFY(newEntryCount == currentEntryCount + 1);
}

void TestEntryModel::testUpdateEntryChangesProperties()
{

}

void TestEntryModel::testRemoveEntryDecreasesByOne()
{

}

QTEST_APPLESS_MAIN(TestEntryModel)
