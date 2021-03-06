#include <QObject>
#include <QtTest>
#include "dataprovider.h"
#include "entrymodel.h"
#include "entryproxymodel.h"
#include "taskmodel.h"
#include "projectmodel.h"

class TestEntryModel: public QObject
{
    Q_OBJECT
public:
    TestEntryModel();

private slots:
    void initTestCase();
    void init();
    void cleanup();
    void cleanupTestCase();

    void testAddEntryIncreasesByOne();
    void testUpdateEntryChangesProperties();
    void testRemoveEntryDecreasesByOne();
private:
    DataProvider    *_provider = nullptr;
    ProjectModel    *_projectModel = nullptr;
    TaskModel       *_taskModel = nullptr;
    EntryModel      *_model = nullptr;
    EntryProxyModel *_proxyModel = nullptr;

    void _initTestData() const;
};
