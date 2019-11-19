#ifndef DATAPROVIDER_H
#define DATAPROVIDER_H

#include "database.h"
#include <QDateTime>

struct Project
{
    int id;
    QString name;
};

struct Task
{
    int id;
    Project* project;
    QString name;
};

struct Entry
{
    int id;
    QDateTime from;
    QDateTime until;
    Task* task;
    QString text;
};

class DataProvider final
{
private:
    Database _db;
    QVector<Project> _projects;
    QVector<Task> _tasks;
    QVector<Entry> _entries;
public:
    DataProvider();

    bool isInitialized() const;

    Project addProject(const QString& projectName);
    Project updateProject(const Project& project, const QString& projectName);
    bool deleteProject(const Project& task);
    QVector<Project>& getAllProjects();

    Task addTask(const Project& project, const QString& taskName);
    Task updateTask(const Task& task, const Project& projectId, const QString& taskName);
    bool deleteTask(const Task& task);
    QVector<Task>& getAllTasks(const Project& project) const;

    Entry addEntry(const QDateTime& from, const QDateTime& until, const Task& task, const QString& text);
    Entry updateEntry(const Entry& entry, const QDateTime& from, const QDateTime& until, const Task& task, const QString& text);
    bool deleteEntry(const Entry& entry);
    QVector<Entry>& getAllEntries() const;
};

#endif // DATAPROVIDER_H
