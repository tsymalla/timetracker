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
    registerEntityColumn<Entry>("FROM");
    registerEntityColumn<Entry>("UNTIL");
    registerEntityColumn<Entry>("TEXT");
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
        Project p{result.value(0).toInt(), result.value(1).toString()};
        _projects.push_back(std::move(p));
    }

    return _projects;
}

bool DataProvider::deleteTask(const Task &task)
{
    return _db._genericDelete<Task>(task.id);
}

bool DataProvider::deleteEntry(const Entry &entry)
{
    return _db._genericDelete<Entry>(entry.id);
}

QVector<Task> &DataProvider::getAllTasksByProject(const int projectId)
{
    _tasks[projectId].clear();

    auto result = _db._genericSelect<Task>({ "PROJECT_ID" }, { projectId });
    while (result.next())
    {
        Task t{result.value(0).toInt(), result.value(1).toInt(), result.value(2).toString()};
        _tasks[projectId].push_back(std::move(t));
    }

    return _tasks[projectId];
}
