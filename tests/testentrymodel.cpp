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

    _model->setDateFilter(QDate::currentDate(), QDate::currentDate());
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
    t.projectId = _projectModel->getRow(0).id;
    _taskModel->addRow(t);

    const auto currentEntryCount = _model->getRows().length();
    Entry e;
    e.taskId = _taskModel->getRow(0).id;
    e.from = QDateTime::currentDateTime();
    e.until = QDateTime::currentDateTime();
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
