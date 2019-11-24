#ifndef TASKMODEL_H
#define TASKMODEL_H

#include "dataprovider.h"

#include <QAbstractListModel>
#include <QObject>
#include <QVector>

class TaskModel : public QAbstractListModel
{
private:
    DataProvider* _provider;
    QVector<Task> _tasks;
    ENTITY_ID_TYPE _projectId;
    void _internalUpdate();
public:
    TaskModel(QObject *parent, DataProvider* provider);

    void setProjectId(const ENTITY_ID_TYPE projectId);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Task& getRow(const int index);
    int getIndex(const ENTITY_ID_TYPE id) const;
    void addRow(const Task& task);
    void updateRow(const Task& task);
    void removeRow(const QModelIndex &index, const Task& task);
    void refresh();
};

#endif // TASKMODEL_H
