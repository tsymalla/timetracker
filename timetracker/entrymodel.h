#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include "dataprovider.h"

#include <QAbstractTableModel>
#include <QObject>
#include <QVector>

class EntryModel : public QAbstractTableModel
{
    struct DateFilter
    {
        QDate start;
        QDate end;
    };

private:
    constexpr static int COL_COUNT = 6;

    DataProvider*   _provider;
    QVector<Entry>  _entries;
    DateFilter      _dateFilter;
    void _internalUpdate();
public:
    EntryModel(QObject* parent, DataProvider* provider);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Entry& getRow(const QModelIndex &index);
    QVector<Entry>& getRows();
    void addRow(const Entry& entry);
    void updateRow(const Entry& entry);
    void removeRow(const QModelIndex &index, const Entry& entry);
    void setDateFilter(const QDate& start, const QDate& end);

    static const QString getDurationString(const QDateTime& dt);

    void refresh();
};

#endif // ENTRYMODEL_H
