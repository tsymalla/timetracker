#include "projectmodel.h"

void ProjectModel::_internalUpdate()
{
    _projects = _provider.getAllProjects();
}

ProjectModel::ProjectModel(QObject *parent, DataProvider &provider): QAbstractListModel(parent), _provider(provider)
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
    if (!index.isValid() || role != Qt::DisplayRole)
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

void ProjectModel::addRow(const Project &project)
{
    const auto rows = _projects.count();
    beginInsertRows(QModelIndex(), rows, rows);
    _provider.addProject(project.name);
    endInsertRows();

    _internalUpdate();
}

void ProjectModel::updateRow(const Project &project)
{
    _internalUpdate();
}

void ProjectModel::removeRow(const QModelIndex &index, const Project &project)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    _provider.deleteProject(project);
    endRemoveRows();

    _internalUpdate();
}
