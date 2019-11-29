#include "taskmodel.h"

void TaskModel::_internalUpdate()
{
    assert(_projectId > 0);
    _tasks = _provider->getAllTasksByProject(_projectId);

    emit dataChanged(index(0), index(_tasks.count()));
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
    if (!index.isValid() || role != Qt::DisplayRole || _tasks.empty())
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

int TaskModel::getIndex(const ENTITY_ID_TYPE id) const
{
    for (auto i = 0; i < _tasks.count(); ++i)
    {
        if (_tasks[i].id == id)
        {
            return i;
        }
    }

    return -1;
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
    _provider->updateTask(task);
    _internalUpdate();
}

void TaskModel::removeRow(const int index, const Task &task)
{
    beginRemoveRows(QModelIndex(), index, index);
    _provider->deleteTask(task);
    endRemoveRows();

    _internalUpdate();
}

void TaskModel::refresh()
{
    _internalUpdate();
}
