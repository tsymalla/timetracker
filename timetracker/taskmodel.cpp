#include "taskmodel.h"

void TaskModel::_internalUpdate()
{
}

TaskModel::TaskModel(QObject *parent, DataProvider &provider): QAbstractListModel(parent), _provider(provider)
{
    _internalUpdate();
}
