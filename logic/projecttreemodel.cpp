#include "projecttreemodel.h"

TreeItem::TreeItem(TreeItemData data, TreeItem *parent): _parent(parent), _data(data)
{
}

TreeItem::~TreeItem()
{
    qDeleteAll(_children);
}

TreeItem *TreeItem::parent() const
{
    return _parent;
}

void TreeItem::addChild(TreeItem* child)
{
    _children.append(child);
}

TreeItem *TreeItem::getChild(int row)
{
    if (row < 0 || row >= childCount())
    {
        return nullptr;
    }

    return _children.at(row);
}

int TreeItem::childCount() const
{
    return _children.count();
}

int TreeItem::row() const
{
    if (_parent)
    {
        return _parent->_children.indexOf(const_cast<TreeItem*>(this));
    }

    return 0;
}

QVariant TreeItem::data(int column) const
{
    if (column == 0)
    {
        return _data.title;
    }
    else if (column == 1)
    {
        return _data.id;
    }
    else if (column == 2)
    {
        return static_cast<unsigned int>(_data.type);
    }

    return QVariant();
}

/**
 * @brief ProjectTreeModel::_initializeTree
 * Creates the internal tree data structure by iterating over the projects and the corresponding tasks
 */
void ProjectTreeModel::_initializeTree()
{
    const auto& projects = _provider->getAllProjects();
    for (const auto& project: projects)
    {
        auto rootData = TreeItem::TreeItemData();
        rootData.id     = project.id;
        rootData.type   = TreeItem::TYPE::PROJECT;
        rootData.title  = project.name;

        auto projectRoot = new TreeItem(rootData, _root);
        _root->addChild(projectRoot);

        const auto& tasks = _provider->getAllTasksByProject(project.id);

        for (const auto& task: tasks)
        {
            auto taskData = TreeItem::TreeItemData();
            taskData.id     = task.id;
            taskData.type   = TreeItem::TYPE::TASK;
            taskData.title  = task.name;

            projectRoot->addChild(new TreeItem(taskData, projectRoot));
        }
    }
}

ProjectTreeModel::ProjectTreeModel(QObject *parent, DataProvider* provider) : QAbstractItemModel(parent), _provider(provider)
{
    refreshTree();
}

ProjectTreeModel::~ProjectTreeModel()
{
    delete _root;
}

QModelIndex ProjectTreeModel::index(int row, int column, const QModelIndex &parent) const
{
    if (!hasIndex(row, column, parent))
    {
        return QModelIndex();
    }

    TreeItem* parentNode = nullptr;

    if (!parent.isValid())
    {
        parentNode = _root;
    }
    else
    {
        parentNode = static_cast<TreeItem*>(parent.internalPointer());
    }

    TreeItem* child = parentNode->getChild(row);

    if (!child)
    {
        return QModelIndex();
    }

    return createIndex(row, column, child);
}

QModelIndex ProjectTreeModel::parent(const QModelIndex &index) const
{
    if (!index.isValid())
    {
        return QModelIndex();
    }

    auto child = static_cast<TreeItem*>(index.internalPointer());
    auto parent = child->parent();

    if (parent == _root)
    {
        return QModelIndex();
    }

    return createIndex(parent->row(), 0, parent);
}

int ProjectTreeModel::rowCount(const QModelIndex &parent) const
{
    if (parent.column() > 0)
    {
        return 0;
    }

    TreeItem* parentNode = nullptr;

    if (!parent.isValid())
    {
        parentNode = _root;
    }
    else
    {
        parentNode = static_cast<TreeItem*>(parent.internalPointer());
    }

    return parentNode->childCount();
}

int ProjectTreeModel::columnCount(const QModelIndex &parent) const
{
    Q_UNUSED(parent)
    return TreeItem::colCount();
}

TreeItem *ProjectTreeModel::getNode(const QModelIndex &index) const
{
    if (!index.isValid() || index.column() == 1)
    {
        return nullptr;
    }

    return static_cast<TreeItem*>(index.internalPointer());
}

QVariant ProjectTreeModel::data(const QModelIndex &index, int role) const
{
    if (!index.isValid() || role != Qt::DisplayRole || index.column() == 1)
    {
        return QVariant();
    }

    auto node = static_cast<TreeItem*>(index.internalPointer());
    return node->data(index.column());
}

QVariant ProjectTreeModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation == Qt::Horizontal && role == Qt::DisplayRole)
    {
        if (section == 0)
        {
            return "Title";
        }
    }

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

void ProjectTreeModel::refreshTree()
{
    emit beginResetModel();

    if (_root != nullptr)
    {
        delete _root;
        _root = nullptr;
    }

    _root = new TreeItem({ 0, TreeItem::TYPE::OTHER, tr("Root") });
    _root->addChild(new TreeItem({ 0, TreeItem::TYPE::OTHER, tr("All projects") }, _root));
    _initializeTree();

    emit endResetModel();
}
