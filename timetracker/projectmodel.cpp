#include "projectmodel.h"

void ProjectModel::_internalUpdate()
{
    _projects = _provider->getAllProjects();

    emit dataChanged(index(0), index(_projects.count()));
}

ProjectModel::ProjectModel(QObject *parent, DataProvider *provider): QAbstractListModel(parent), _provider(provider)
{
    _internalUpdate();
}

int ProjectModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _projects.count();
}

QVariant ProjectModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole || _projects.empty())
    {
        return QVariant();
    }

    const auto& project = _projects[index.row()];

    return project.name;
}

Project &ProjectModel::getRow(const int index)
{
    return _projects[index];
}

int ProjectModel::getIndex(const ENTITY_ID_TYPE id) const
{
    for (auto i = 0; i < _projects.count(); ++i)
    {
        if (_projects[i].id == id)
        {
            return i;
        }
    }

    return -1;
}

void ProjectModel::addRow(const Project &project)
{
    const auto rows = _projects.count();
    beginInsertRows(QModelIndex(), rows, rows);
    _provider->addProject(project.name);
    endInsertRows();

    _internalUpdate();
}

void ProjectModel::updateRow(const Project &project)
{
    _provider->updateProject(project);
    _internalUpdate();
}

void ProjectModel::removeRow(const int index, const Project &project)
{
    beginRemoveRows(QModelIndex(), index, index);
    _provider->deleteProject(project);
    endRemoveRows();

    _internalUpdate();
}

void ProjectModel::refresh()
{
    _internalUpdate();
}
