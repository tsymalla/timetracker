#include "testentrymodel.h"
#include <QDateTime>

TestEntryModel::TestEntryModel()
{
}

void TestEntryModel::init()
{
}

void TestEntryModel::cleanup()
{
}

void TestEntryModel::initTestCase()
{
    _provider = new DataProvider(this);
    _model = new EntryModel(this, _provider);
    _projectModel = new ProjectModel(this, _provider);
    _taskModel = new TaskModel(this, _provider);
}

void TestEntryModel::cleanupTestCase()
{
    delete _provider;
    delete _model;
    delete _projectModel;
    delete _taskModel;
}

void TestEntryModel::testAddEntryIncreasesByOne()
{
    Project p;
    p.name = "Test project";
    _projectModel->addRow(p);

    Task t;
    t.name = "Test task";
    t.projectId = 1;
    _taskModel->addRow(t);

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
