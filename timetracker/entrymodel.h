#ifndef ENTRYMODEL_H
#define ENTRYMODEL_H

#include "dataprovider.h"

#include <QAbstractTableModel>
#include <QObject>

class EntryModel : public QAbstractTableModel
{
private:
    DataProvider& _provider;
    QVector<Entry> _entries;
    void _internalUpdate();
public:
    EntryModel(QObject* parent, DataProvider& provider);
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;

    Entry& getRow(const QModelIndex &index);
    void addRow(const Entry& entry);
    void updateRow(const Entry& entry);
    void removeRow(const QModelIndex &index, const Entry& entry);
};

#endif // ENTRYMODEL_H
