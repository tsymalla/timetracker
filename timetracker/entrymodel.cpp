#include "entrymodel.h"

void EntryModel::_internalUpdate()
{
    _entries = _provider.getAllEntries();
}

EntryModel::EntryModel(QObject *parent, DataProvider &provider): QAbstractTableModel(parent), _provider(provider)
{
    _internalUpdate();
}

int EntryModel::rowCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return _entries.count();
}

int EntryModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return 4;
}

QVariant EntryModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole)
    {
        return QVariant();
    }

    const auto& entry = _entries[index.row()];

    switch (index.column())
    {
    case 0:
        return entry.taskId;
    case 1:
        return entry.from;
    case 2:
        return entry.until;
    case 3:
        return entry.entryContent;
    }

    return QVariant();
}

QVariant EntryModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (role == Qt::DisplayRole && orientation == Qt::Horizontal)
    {
        switch (section)
        {
        case 0:
            return QString("Task");
        case 1:
            return QString("From");
        case 2:
            return QString("Until");
        case 3:
            return QString("Text");
        }
    }

    return QVariant();
}

Entry &EntryModel::getRow(const QModelIndex &index)
{
    return _entries[index.row()];
}

void EntryModel::addRow(const Entry &entry)
{
    const auto row = _entries.count();

    beginInsertRows(QModelIndex(), row, row);
    _provider.addEntry(entry.taskId, entry.entryContent, entry.from, entry.until);
    endInsertRows();

    _internalUpdate();
}

void EntryModel::updateRow(const Entry &entry)
{
    _internalUpdate();
}

void EntryModel::removeRow(const QModelIndex &index, const Entry &entry)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    _provider.deleteEntry(entry);
    endRemoveRows();

    _internalUpdate();
}
