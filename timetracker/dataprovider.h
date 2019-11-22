#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "database.h"
#include <QDateTime>
#include <QMap>
#include <QVector>

using ENTITY_ID_TYPE = int;
const auto DATE_TIME_FORMAT = QString("yyyy-MM-dd hh:mm:ss");

struct Project
{
    ENTITY_ID_TYPE id;
    QString name;

    static const QString TYPE() {
        return "PROJECT";
    }

    static Project fromResult(const QSqlQuery& result)
    {
        return Project
        {
            result.value(0).toInt(),
            result.value(1).toString()
        };
    }
};

struct Task
{
    ENTITY_ID_TYPE id;
    ENTITY_ID_TYPE projectId;
    QString name;

    static const QString TYPE() {
        return "TASK";
    }

    static Task fromResult(const QSqlQuery& result)
    {
        return Task
        {
            result.value(0).toInt(),
            result.value(1).toInt(),
            result.value(2).toString()
        };
    }
};

struct Entry
{
    ENTITY_ID_TYPE id;
    ENTITY_ID_TYPE projectId;
    ENTITY_ID_TYPE taskId;
    QString projectName;
    QString taskName;
    QString entryContent;
    QDateTime from;
    QDateTime until;
    QDateTime duration;

    static const QString TYPE() {
        return "ENTRY";
    }

    static Entry fromResult(const QSqlQuery& result)
    {
        Entry e =
        {
            result.value(0).toInt(),
            result.value(1).toInt(),
            result.value(2).toInt(),
            result.value(3).toString(),
            result.value(4).toString(),
            result.value(5).toString(),
            QDateTime::fromString(result.value(6).toString(), DATE_TIME_FORMAT),
            QDateTime::fromString(result.value(7).toString(), DATE_TIME_FORMAT),
        };

        const auto duration = QDateTime::fromSecsSinceEpoch(e.until.toSecsSinceEpoch() - e.from.toSecsSinceEpoch());
        e.duration = duration;

        return e;
    }
};

class DataProvider final
{
private:
    template<typename ENTITY, typename COLUMN>
    void registerEntityColumn(COLUMN column)
    {
        _entityMapping[ENTITY::TYPE()] << column;
    }

    Database _db;
    QMap<QString, QStringList> _entityMapping;
    QVector<Project> _projects;
    QMap<int, QVector<Task>> _tasks;

    void _initMapping();

    template<typename T>
    void _insert(const QVariantList& args)
    {
        _db._genericInsert<T>(_entityMapping[T::TYPE()], args);
    }

    /*template<typename T>
    void _update(const QVariantList& args)
    {
        _db.genericUpdate<T>(_entityMapping[T::TYPE()], args);
    }*/
public:
    DataProvider();

    bool isInitialized() const;

    void addProject(const QString& projectName);
    void updateProject(const Project& project, const QString& projectName);
    bool deleteProject(const Project& project);
    QVector<Project>& getAllProjects();

    void addTask(const Project& project, const QString& taskName);
    void updateTask(const Task& task, const Project& project, const QString& taskName);
    bool deleteTask(const Task& task);
    QVector<Task>& getAllTasksByProject(const ENTITY_ID_TYPE projectId);

    void addEntry(const ENTITY_ID_TYPE taskId, const QString& entryContent, const QDateTime& from, const QDateTime& until);
    void updateEntry(const Entry& entry, const QString& entryContent, const QDateTime& from, const QDateTime& until, const ENTITY_ID_TYPE taskId);
    bool deleteEntry(const Entry& entry);
    QVector<Entry> getAllEntries() const;
};

#endif // DATAPROVIDER_H
