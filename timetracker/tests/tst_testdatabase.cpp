#include <QtTest>
#include <QCoreApplication>

// add necessary includes here

class TestDatabase : public QObject
{
    Q_OBJECT

public:
    TestDatabase();
    ~TestDatabase();

private slots:
    void initTestCase();
    void cleanupTestCase();
    void test_case1();

};

TestDatabase::TestDatabase()
{

}

TestDatabase::~TestDatabase()
{

}

void TestDatabase::initTestCase()
{

}

void TestDatabase::cleanupTestCase()
{

}

void TestDatabase::test_case1()
{

}

QTEST_MAIN(TestDatabase)

#include "tst_testdatabase.moc"
