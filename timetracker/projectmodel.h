#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "dataprovider.h"

#include <QAbstractListModel>
#include <QObject>

class ProjectModel : public QAbstractListModel
{
private:
    DataProvider& _provider;
    QVector<Project> _projects;

    void _internalUpdate();
public:
    ProjectModel(QObject* parent, DataProvider& provider);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Project& getRow(const int index);
    void addRow(const Project& project);
    void updateRow(const Project& project);
    void removeRow(const QModelIndex &index, const Project& project);
};

#endif // PROJECTMODEL_H
