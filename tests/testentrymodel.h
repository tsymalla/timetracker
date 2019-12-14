#include <QObject>
#include <QtTest>
#include "dataprovider.h"
#include "entrymodel.h"

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
    EntryModel      *_model = nullptr;
};
