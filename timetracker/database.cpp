#include "database.h"
#include <QDebug>
#include <QDir>

void Database::_init()
{
    const QString driver("QSQLITE");
    if (!QSqlDatabase::isDriverAvailable(driver))
    {
        qWarning() << "SQLite not available.";
        return;
    }

    _db = QSqlDatabase::addDatabase(driver);

    const QString path = QDir::currentPath() + QDir::separator() + DATABASE_NAME;
    auto hasDatabase = true;
    if (!QFile::exists(path))
    {
        QFile file;
        file.open(QIODevice::WriteOnly);
        file.close();
        hasDatabase = false;
    }

    _db.setDatabaseName(path);

    if (!_db.open())
    {
        qWarning() << "Could not open database: " << _db.lastError().text();
        return;
    }

    if (!hasDatabase)
    {
        _refreshStorage();
    }

    _isInitialized = true;
}

void Database::_refreshStorage()
{
    executeQuery("CREATE TABLE PROJECT (ID INTEGER PRIMARY KEY, NAME TEXT)");
    executeQuery("CREATE TABLE TASK (ID INTEGER PRIMARY KEY, PROJECT_ID INTEGER, NAME TEXT, FOREIGN KEY (PROJECT_ID) REFERENCES PROJECT(ID))");
    executeQuery("CREATE TABLE ENTRY (ID INTEGER PRIMARY KEY, TASK_ID INTEGER, CONTENT TEXT, FROM DATETIME, UNTIL DATETIME, FOREIGN KEY (TASK_ID) REFERENCES TASK(ID))");
    executeQuery("INSERT INTO PROJECT(NAME) VALUES('test')");
    executeQuery("INSERT INTO PROJECT(NAME) VALUES('test2')");
    executeQuery("INSERT INTO TASK(PROJECT_ID, NAME) VALUES(1, 'testaufgabe')");
    executeQuery("INSERT INTO TASK(PROJECT_ID, NAME) VALUES(1, 'testaufgabe 2')");
    executeQuery("INSERT INTO TASK(PROJECT_ID, NAME) VALUES(2, 'testaufgabe 3')");
}

QSqlQuery Database::executeQuery(const QString &sql, const QVariantList &bindArgs) const
{
    QSqlQuery query;
    query.prepare(sql);

    assert(query.boundValues().size() == bindArgs.size());

    for (int i = 0; i < bindArgs.size(); ++i)
    {
        query.bindValue(i, bindArgs[i]);
    }

    query.exec();

    return query;
}

QSqlQuery Database::executeQuery(const QString &sql) const
{
    return executeQuery(sql, {});
}

Database::Database(): _isInitialized(false)
{
    _init();
}

Database::~Database()
{
    if (_db.isOpen())
    {
        _db.close();
    }
}

bool Database::isInitialized() const
{
    return _isInitialized;
}
