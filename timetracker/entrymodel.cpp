#include "entrymodel.h"

void EntryModel::_internalUpdate()
{
    _entries = _provider.getAllEntries();
}

const QString EntryModel::_getDurationString(const QDateTime &dt) const
{
    QStringList result;
#define APPEND_VALUE(value, description) if (value != 0) result << QString::number(value) + " " + description;

    const auto& date = dt.date();
    const auto days = date.day();
    const auto months = date.month();
    const auto years = date.year();
    const auto& time = dt.time();
    const auto hours = time.hour();
    const auto minutes = time.minute();

    APPEND_VALUE(days,      "day(s)")
    APPEND_VALUE(months,    "month(s)")
    APPEND_VALUE(years,     "year(s)")
    APPEND_VALUE(hours,     "hour(s)")
    APPEND_VALUE(minutes,   "minute(s)")

    return result.join(", ");
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
    return COL_COUNT;
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
        return entry.projectName;
    case 1:
        return entry.taskName;
    case 2:
        return entry.from;
    case 3:
        return entry.until;
    case 4:
        return _getDurationString(entry.duration);
    case 5:
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
            return QString("Project");
        case 1:
            return QString("Task");
        case 2:
            return QString("From");
        case 3:
            return QString("Until");
        case 4:
            return QString("Duration");
        case 5:
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
