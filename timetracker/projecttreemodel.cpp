#include "projecttreemodel.h"

ProjectTreeModel::ProjectTreeModel(QObject *parent) : QAbstractItemModel(parent)
{

}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    return QModelIndex();
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &child) const
{
    return QModelIndex();
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const
{
    return 0;
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const
{
    return 1;
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const
{
    return QVariant();
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    return QVariant();
}

Qt::ItemFlags ProjectTreeModel::flags(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return Qt::NoItemFlags;
    }

    return QAbstractItemModel::flags(index);
}
