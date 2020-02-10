#ifndef ENTRYPROXYMODEL_H
#define ENTRYPROXYMODEL_H

#include "dataprovider.h"

#include <QDateTime>
#include <QObject>
#include <QSortFilterProxyModel>
#include <QVector>

using ENTITY_ID_TYPE = int;

class EntryProxyModel : public QSortFilterProxyModel
{
    Q_OBJECT

    constexpr static int COL_COUNT = 6;
public:
    EntryProxyModel(QObject *parent = nullptr);
    bool filterAcceptsRow(int source_row, const QModelIndex &source_parent) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    int columnCount(const QModelIndex &parent) const override;

    QVector<Entry> getRows();

public slots:
    void setStartDate(const QDate& dt);
    void setEndDate(const QDate& dt);
    void setProjectId(ENTITY_ID_TYPE projectId);
    void setTaskId(ENTITY_ID_TYPE taskId);

private:
    QDate           _startDt;
    QDate           _endDt;
    ENTITY_ID_TYPE  _projectId;
    ENTITY_ID_TYPE  _taskId;
};

#endif // ENTRYPROXYMODEL_H
