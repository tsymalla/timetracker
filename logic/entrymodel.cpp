#include "entrymodel.h"
#include <QDebug>

void EntryModel::_internalUpdate()
{
    _entries = _provider->getEntriesByDateRange(_dateFilter.start, _dateFilter.end);

    emit dataChanged(index(0, 0), index(_entries.count(), COL_COUNT));
    emit layoutChanged();
}

EntryModel::EntryModel(QObject *parent, DataProvider *provider): QAbstractTableModel(parent), _provider(provider)
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
        return getDurationString(entry.duration);
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

QVector<Entry> &EntryModel::getRows()
{
    return _entries;
}

void EntryModel::addRow(const Entry &entry)
{
    const auto row = _entries.count();

    beginInsertRows(QModelIndex(), row, row);
    _provider->addEntry(entry.taskId, entry.entryContent, entry.from, entry.until);
    endInsertRows();

    _internalUpdate();
}

void EntryModel::updateRow(const Entry &entry)
{
    _provider->updateEntry(entry);
    _internalUpdate();
}

void EntryModel::removeRow(const Entry &entry)
{
    _provider->deleteEntry(entry);
    _internalUpdate();
}

void EntryModel::removeRow(const QModelIndex &index, const Entry &entry)
{
    beginRemoveRows(QModelIndex(), index.row(), index.row());
    _provider->deleteEntry(entry);
    endRemoveRows();

    _internalUpdate();
}

void EntryModel::setDateFilter(const QDate &start, const QDate &end)
{
    _dateFilter.start   = start;
    _dateFilter.end     = end;

    _internalUpdate();
}

template<typename Value, typename Description>
void appendValue(QStringList& result, Value v, Description d)
{
    if (v != 0)
    {
        result << QString::number(v) + " " + d + (v > 1 ? "s" : "");
    }
}

QString EntryModel::getDurationString(const QDateTime &dt)
{
    QStringList result;

    const qint64 DAY = 86400;
    const auto secs = dt.toSecsSinceEpoch();
    auto days = secs / DAY;
    auto t = QTime(0, 0).addSecs(secs  % DAY);

    const auto hours = t.hour();
    const auto minutes = t.minute();

    appendValue(result, days,      "day");
    appendValue(result, hours,     "hour");
    appendValue(result, minutes,   "minute");

    if (result.empty())
    {
        result << "0 minutes";
    }

    return result.join(", ");
}

void EntryModel::refresh()
{
    _internalUpdate();
}
