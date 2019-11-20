#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "database.h"
#include <QDateTime>
#include <QMap>

using ENTITY_ID_TYPE = int;

struct Project
{
    ENTITY_ID_TYPE id;
    QString name;

    static const QString TYPE() {
        return "PROJECT";
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
};

struct Entry
{
    ENTITY_ID_TYPE id;
    ENTITY_ID_TYPE taskId;
    QDateTime from;
    QDateTime until;
    QString text;

    static const QString TYPE() {
        return "ENTRY";
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
    QVector<Entry> _entries;

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
    void updateTask(const Task& task, const Project& projectId, const QString& taskName);
    bool deleteTask(const Task& task);
    QVector<Task>& getAllTasksByProject(const ENTITY_ID_TYPE projectId);

    void addEntry(const QDateTime& from, const QDateTime& until, const Task& task, const QString& text);
    void updateEntry(const Entry& entry, const QDateTime& from, const QDateTime& until, const Task& task, const QString& text);
    bool deleteEntry(const Entry& entry);
    QVector<Entry>& getAllEntries() const;
};

#endif // DATAPROVIDER_H
