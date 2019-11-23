#include "dataprovider.h"
#include <QVariant>

DataProvider::DataProvider(QObject *parent): QObject(parent)
{
    _initMapping();
    _db = new Database(this);
}

void DataProvider::_initMapping()
{
    registerEntityColumn<Project>("NAME");

    registerEntityColumn<Task>("PROJECT_ID");
    registerEntityColumn<Task>("NAME");

    registerEntityColumn<Entry>("TASK_ID");
    registerEntityColumn<Entry>("ENTRY_CONTENT");
    registerEntityColumn<Entry>("TS_FROM");
    registerEntityColumn<Entry>("TS_UNTIL");
}

bool DataProvider::isInitialized() const
{
    return _db->isInitialized();
}

void DataProvider::addProject(const QString &projectName)
{
    QVariantList args = { projectName };
    _insert<Project>(args);
}

bool DataProvider::deleteProject(const Project &project)
{
    return _db->_genericDelete<Project>(project.id);
}

QVector<Project> &DataProvider::getAllProjects()
{
    _projects.clear();
    auto result = _db->_genericSelectAll<Project>();
    while (result.next())
    {
        _projects.push_back(Project::fromResult(result));
    }

    return _projects;
}

void DataProvider::addTask(const Task& task)
{
    QVariantList args = { task.projectId, task.name };
    _insert<Task>(args);
}

bool DataProvider::deleteTask(const Task &task)
{
    return _db->_genericDelete<Task>(task.id);
}

QVector<Task> &DataProvider::getAllTasksByProject(const int projectId)
{
    _tasks[projectId].clear();

    auto result = _db->_genericSelect<Task>({ "PROJECT_ID" }, { projectId });
    while (result.next())
    {
        _tasks[projectId].push_back(Task::fromResult(result));
    }

    return _tasks[projectId];
}

void DataProvider::addEntry(const ENTITY_ID_TYPE taskId, const QString &text, const QDateTime &from, const QDateTime &until)
{
    const auto fromFormat = from.toString(DATE_TIME_FORMAT);
    const auto untilFormat = until.toString(DATE_TIME_FORMAT);
    QVariantList args = { taskId, text, fromFormat, untilFormat };
    _insert<Entry>(args);
}

bool DataProvider::deleteEntry(const Entry &entry)
{
    return _db->_genericDelete<Entry>(entry.id);
}

QVector<Entry> DataProvider::getAllEntries() const
{
    QVector<Entry> entries;

    auto result = _db->executeQuery("SELECT e.ID,"
                                   "p.ID,"
                                   "t.ID,"
                                   "p.NAME,"
                                   "t.NAME,"
                                   "e.ENTRY_CONTENT,"
                                   "e.TS_FROM,"
                                   "e.TS_UNTIL FROM ENTRY e INNER JOIN TASK t ON e.TASK_ID = t.ID INNER JOIN PROJECT p ON t.PROJECT_ID = p.ID");
    while (result.next())
    {
        entries.push_back(Entry::fromResult(result));
    }

    return entries;
}
