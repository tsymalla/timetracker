#include "dataprovider.h"
#include <QVariant>

DataProvider::DataProvider()
{

}

bool DataProvider::isInitialized() const
{
    return _db.isInitialized();
}

QVector<Project> &DataProvider::getAllProjects()
{
    _projects.clear();
    auto result = _db.executeQuery("SELECT * FROM PROJECT");
    while (result.next())
    {
        Project p{result.value(0).toInt(), result.value(1).toString()};
        _projects.push_back(std::move(p));
    }

    return _projects;
}

QVector<Task> &DataProvider::getAllTasksByProject(const int projectId)
{
    _tasks[projectId].clear();

    auto result = _db.executeQuery("SELECT * FROM TASK WHERE PROJECT_ID=:1", {projectId});
    while (result.next())
    {
        Task t{result.value(0).toInt(), result.value(1).toInt(), result.value(2).toString()};
        _tasks[projectId].push_back(std::move(t));
    }

    return _tasks[projectId];
}
