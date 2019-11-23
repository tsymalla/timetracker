#include "taskmodel.h"

void TaskModel::_internalUpdate()
{
    assert(_projectId > 0);
    _tasks = _provider->getAllTasksByProject(_projectId);

    emit dataChanged(index(0), index(_tasks.count() - 1));
}

TaskModel::TaskModel(QObject *parent, DataProvider *provider): QAbstractListModel(parent), _provider(provider), _projectId(0)
{
}

void TaskModel::setProjectId(const ENTITY_ID_TYPE projectId)
{
    assert(projectId > 0);
    _projectId = projectId;

    _internalUpdate();
}

int TaskModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _tasks.count();
}

QVariant TaskModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    const auto& task = _tasks[index.row()];

    return task.name;
}

Task &TaskModel::getRow(const int index)
{
    return _tasks[index];
}

void TaskModel::addRow(const Task &task)
{
    const auto row = _tasks.count();

    beginInsertRows(QModelIndex(), row, row);
    _provider->addTask(task);
    endInsertRows();

    _internalUpdate();
}

void TaskModel::updateRow(const Task &task)
{
    _internalUpdate();
}

void TaskModel::removeRow(const QModelIndex &index, const Task &task)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    _provider->deleteTask(task);
    endRemoveRows();

    _internalUpdate();
}
