#ifndef TASKMODEL_H
#define TASKMODEL_H

#include "dataprovider.h"

#include <QAbstractListModel>
#include <QObject>
#include <QMap>

class TaskModel : public QAbstractListModel
{
private:
    DataProvider& _provider;
    QMap<ENTITY_ID_TYPE, QVector<Task>> _tasks;
    void _internalUpdate();
public:
    TaskModel(QObject *parent, DataProvider& provider);
};

#endif // TASKMODEL_H
