#ifndef PROJECTTREEMODEL_H
#define PROJECTTREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>

class ProjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT
public:
    explicit ProjectTreeModel(QObject *parent = nullptr);

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // PROJECTTREEMODEL_H
