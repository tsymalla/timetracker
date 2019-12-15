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
    _provider = new DataProvider(this, true);
    _model = new EntryModel(this, _provider);
    _projectModel = new ProjectModel(this, _provider);
    _taskModel = new TaskModel(this, _provider);

    _model->setDateFilter(QDate::currentDate(), QDate::currentDate());
    _initTestData();
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
    Entry e;
    e.taskId = _taskModel->getRow(0).id;
    e.from = QDateTime::currentDateTime();
    e.until = QDateTime::currentDateTime();
    e.entryContent = "Test";

    _model->addRow(e);

    auto savedEntry = _model->getRows()[0];
    savedEntry.entryContent = "Test2";
    _model->updateRow(savedEntry);

    savedEntry = _model->getRows()[0];
    QVERIFY(savedEntry.entryContent == "Test2");
}

void TestEntryModel::testRemoveEntryDecreasesByOne()
{
    Entry e;
    e.taskId = _taskModel->getRow(0).id;
    e.from = QDateTime::currentDateTime();
    e.until = QDateTime::currentDateTime();
    e.entryContent = "Test";

    _model->addRow(e);

    const auto currentEntryCount = _model->getRows().length();

    const auto entry = _model->getRows()[0];
    _model->removeRow(entry);

    const auto newEntryCount = _model->getRows().length();
    QVERIFY(newEntryCount == currentEntryCount - 1);
}

void TestEntryModel::_initTestData() const
{
    Project p;
    p.name = "Test project";
    _projectModel->addRow(p);

    Task t;
    t.name = "Test task";
    t.projectId = _projectModel->getRow(0).id;
    _taskModel->addRow(t);
}

QTEST_APPLESS_MAIN(TestEntryModel)
