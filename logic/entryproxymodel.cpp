#include "entrymodel.h"
#include "entryproxymodel.h"

EntryProxyModel::EntryProxyModel(QObject *parent): QSortFilterProxyModel(parent), _startDt(QDate::currentDate()), _endDt(QDate::currentDate()), _projectId(0), _taskId(0)
{
}

bool EntryProxyModel::filterAcceptsRow(int source_row, const QModelIndex &source_parent) const
{
    bool accepts = true;

    QModelIndex idxStartDt      = sourceModel()->index(source_row, 2, source_parent);
    QModelIndex idxEndDt        = sourceModel()->index(source_row, 3, source_parent);

    const auto start = sourceModel()->data(idxStartDt).toDate();
    const auto end = sourceModel()->data(idxEndDt).toDate();

    accepts &= (start >= _startDt);
    accepts &= (end <= _endDt);

    if (_projectId > 0)
    {
        QModelIndex idxProjectId    = sourceModel()->index(source_row, 6, source_parent);
        const auto projectId = sourceModel()->data(idxProjectId).toInt();
        accepts &= (projectId == _projectId);
    }

    if (_taskId > 0)
    {
        QModelIndex idxTaskId       = sourceModel()->index(source_row, 7, source_parent);
        const auto taskId = sourceModel()->data(idxTaskId).toInt();
        accepts &= (taskId == _taskId);
    }

    return accepts;
}

QVariant EntryProxyModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (section < 6)
    {
        return sourceModel()->headerData(section, orientation, role);
    }

    return QVariant();
}

int EntryProxyModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return COL_COUNT;
}

void EntryProxyModel::refresh()
{
    auto source = qobject_cast<EntryModel*>(sourceModel());
    _filteredData.clear();

    for (auto i = 0; i < rowCount(); ++i)
    {
        auto sourceIndex = mapToSource(index(i, 0));
        auto& item = source->getRow(sourceIndex);
        _filteredData.push_back(item);
    }
}

void EntryProxyModel::update()
{
    invalidateFilter();
    refresh();
}

QVector<Entry> &EntryProxyModel::getRows()
{
    return _filteredData;
}

void EntryProxyModel::setStartDate(const QDate &dt)
{
    if (dt <= _endDt)
    {
        _startDt = dt;
    }

    update();
}

void EntryProxyModel::setEndDate(const QDate &dt)
{
    if (dt >= _startDt)
    {
        _endDt = dt;
    }

    update();
}

void EntryProxyModel::setProjectId(ENTITY_ID_TYPE projectId)
{
    _projectId = projectId;

    update();
}

void EntryProxyModel::setTaskId(ENTITY_ID_TYPE taskId)
{
    _taskId = taskId;

    update();
}
