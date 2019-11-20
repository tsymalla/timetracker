#include "dataprovider.h"
#include <QVariant>

DataProvider::DataProvider()
{
    _initMapping();
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
    return _db.isInitialized();
}

void DataProvider::addProject(const QString &projectName)
{
    QVariantList args = { projectName };
    _insert<Project>(args);
}

bool DataProvider::deleteProject(const Project &project)
{
    return _db._genericDelete<Project>(project.id);
}

QVector<Project> &DataProvider::getAllProjects()
{
    _projects.clear();
    auto result = _db._genericSelectAll<Project>();
    while (result.next())
    {
        _projects.push_back(Project::fromResult(result));
    }

    return _projects;
}

bool DataProvider::deleteTask(const Task &task)
{
    return _db._genericDelete<Task>(task.id);
}

QVector<Task> &DataProvider::getAllTasksByProject(const int projectId)
{
    _tasks[projectId].clear();

    auto result = _db._genericSelect<Task>({ "PROJECT_ID" }, { projectId });
    while (result.next())
    {
        _tasks[projectId].push_back(Task::fromResult(result));
    }

    return _tasks[projectId];
}

void DataProvider::addEntry(const ENTITY_ID_TYPE taskId, const QString &text, const QDateTime &from, const QDateTime &until)
{
    const auto fromTs = static_cast<int>(from.currentDateTimeUtc().toTime_t());
    const auto untilTs = static_cast<int>(until.currentDateTimeUtc().toTime_t());
    QVariantList args = { taskId, text, fromTs, untilTs };
    _insert<Entry>(args);
}

bool DataProvider::deleteEntry(const Entry &entry)
{
    return _db._genericDelete<Entry>(entry.id);
}

QVector<Entry> &DataProvider::getAllEntries()
{
    _entries.clear();

    auto result = _db._genericSelectAll<Entry>();
    while (result.next())
    {
        _entries.push_back(Entry::fromResult(result));
    }

    return _entries;
}
