#ifndef PROJECTMODEL_H
#define PROJECTMODEL_H

#include "dataprovider.h"

#include <QAbstractListModel>
#include <QObject>

class ProjectModel : public QAbstractListModel
{
private:
    DataProvider* _provider;
    QVector<Project> _projects;

    void _internalUpdate();
public:
    ProjectModel(QObject* parent, DataProvider* provider);

    int rowCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;

    Project &getRow(int index);
    int getIndex(ENTITY_ID_TYPE id) const;
    void addRow(const Project& project);
    void updateRow(const Project& project);
    void removeRow(int index, const Project& project);
    void refresh();
};

#endif // PROJECTMODEL_H
