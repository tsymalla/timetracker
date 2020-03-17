#include "dataprovider.h"
#include <QVariant>

DataProvider::DataProvider(QObject *parent, const DatabaseConfiguration::Settings& settings): QObject(parent)
{
    _initMapping();
    _db = new Database(this, settings);
}

DataProvider::DataProvider(QObject *parent, const DatabaseConfiguration::Settings& settings, bool clean): QObject(parent)
{
    _initMapping();
    _db = new Database(this, settings);

    if (clean)
    {
        _db->_cleanupData();
    }
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

void DataProvider::updateProject(const Project &project)
{
    QVariantList args = { project.name };
    _update<Project>(project.id, args);
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

void DataProvider::updateTask(const Task &task)
{
    QVariantList args = { task.projectId, task.name };
    _update<Task>(task.id, args);
}

bool DataProvider::deleteTask(const Task &task)
{
    return _db->_genericDelete<Task>(task.id);
}

QVector<Task> &DataProvider::getAllTasksByProject(const ENTITY_ID_TYPE projectId)
{
    _tasks[projectId].clear();

    auto result = _db->_genericSelect<Task>({ "PROJECT_ID" }, { projectId });
    while (result.next())
    {
        _tasks[projectId].push_back(Task::fromResult(result));
    }

    return _tasks[projectId];
}

void DataProvider::addEntry(ENTITY_ID_TYPE taskId, const QString &entryContent, const QDateTime &from, const QDateTime &until)
{
    const auto fromFormat   = from.toString(DATE_TIME_FORMAT);
    const auto untilFormat  = until.toString(DATE_TIME_FORMAT);
    QVariantList args       = { taskId, entryContent, fromFormat, untilFormat };
    _insert<Entry>(args);
}

void DataProvider::updateEntry(const Entry &entry)
{
    const auto fromFormat   = entry.from.toString(DATE_TIME_FORMAT);
    const auto untilFormat  = entry.until.toString(DATE_TIME_FORMAT);
    QVariantList args       = { entry.taskId, entry.entryContent, fromFormat, untilFormat };
    _update<Entry>(entry.id, args);
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
                                   "e.TS_UNTIL FROM ENTRY e INNER JOIN TASK t ON e.TASK_ID = t.ID INNER JOIN PROJECT p ON t.PROJECT_ID = p.ID "
                                   "ORDER BY e.ID DESC");
    while (result.next())
    {
        entries.push_back(Entry::fromResult(result));
    }

    return entries;
}

QVector<Entry> DataProvider::getEntriesByFilter(const QDate &start, const QDate &end, const ENTITY_ID_TYPE projectId, const ENTITY_ID_TYPE taskId) const
{
    QVector<Entry> entries;
    QVariantList args = { start.toString(DATE_FORMAT), QDateTime(end).addDays(1).addSecs(-1).toString(DATE_FORMAT) };
    QString query = "SELECT e.ID,"
                    "p.ID,"
                    "t.ID,"
                    "p.NAME,"
                    "t.NAME,"
                    "e.ENTRY_CONTENT,"
                    "e.TS_FROM,"
                    "e.TS_UNTIL FROM ENTRY e INNER JOIN TASK t ON e.TASK_ID = t.ID INNER JOIN PROJECT p ON t.PROJECT_ID = p.ID "
                    "WHERE DATE(e.TS_FROM) >= DATE(:1) AND DATE(e.TS_UNTIL) <= DATE(:2) ";

    if (projectId > 0)
    {
        query += " AND p.ID = :3";
        args.push_back(projectId);
    }

    if (taskId > 0)
    {
        query += " AND t.ID = :4";
        args.push_back(taskId);
    }

    query += " ORDER BY e.ID DESC";

    auto result = _db->executeQuery(query, args);

    while (result.next())
    {
        entries.push_back(Entry::fromResult(result));
    }

    return entries;
}
