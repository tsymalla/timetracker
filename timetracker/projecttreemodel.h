#ifndef PROJECTTREEMODEL_H
#define PROJECTTREEMODEL_H

#include <QObject>
#include <QAbstractItemModel>
#include <dataprovider.h>
#include <QVector>

class TreeItem
{
public:

    enum class TYPE: unsigned int
    {
        PROJECT = 0,
        TASK    = 1,
        OTHER   = 2
    };

    struct TreeItemData
    {
        ENTITY_ID_TYPE id;
        TYPE type;
        QString title;
    };

private:
    TreeItem*                           _parent;
    QVector<TreeItem*>                  _children;
    TreeItemData                        _data;

public:
    explicit TreeItem(TreeItemData data, TreeItem* parent = nullptr);
    ~TreeItem();

    TreeItem* parent() const;
    void addChild(TreeItem* child);
    TreeItem* getChild(int row);
    int childCount() const;
    int row() const;
    constexpr static int colCount() { return 1; }

    QVariant data(int column) const;
};

class ProjectTreeModel : public QAbstractItemModel
{
    Q_OBJECT
private:
    DataProvider*                       _provider;
    TreeItem*                           _root;
    void                                _initializeTree();

public:
    explicit ProjectTreeModel(QObject *parent, DataProvider* provider);
    ~ProjectTreeModel();

    QModelIndex index(int row, int column, const QModelIndex &parent) const override;
    QModelIndex parent(const QModelIndex &child) const override;
    int rowCount(const QModelIndex &parent) const override;
    int columnCount(const QModelIndex &parent) const override;
    TreeItem* getNode(const QModelIndex &index) const;
    QVariant data(const QModelIndex &index, int role) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role) const override;
    Qt::ItemFlags flags(const QModelIndex &index) const override;
};

#endif // PROJECTTREEMODEL_H
