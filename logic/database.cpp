#include "database.h"
#include <QDebug>
#include <QDir>

Database::Database(QObject *parent, const DatabaseConfiguration::Settings& settings): QObject(parent), _isInitialized(false)
{
    _config = settings;

    _init();
}

void Database::_init()
{
    const QString driver = _config.type.toUpper() == "SQLITE" ? "QSQLITE" : "QMYSQL3";
    if (!QSqlDatabase::isDriverAvailable(driver))
    {
        qWarning() << "SQLite not available.";
        return;
    }

    _db = QSqlDatabase::addDatabase(driver);

    auto hasDatabase    = true;
    if (driver == "QSQLITE")
    {
        const QString path  = _config.path.isEmpty() ? (QDir::currentPath() + QDir::separator() + DEFAULT_DATABASE_FILE) : _config.path;
        if (!QFile::exists(path))
        {
            QFile file;
            file.open(QIODevice::WriteOnly);
            file.close();
            hasDatabase     = false;
        }

        _db.setDatabaseName(path);
    }
    else
    {
        _db.setConnectOptions("MYSQL_OPT_RECONNECT=1");
        _db.setHostName(_config.host);
        _db.setPort(_config.port);
        _db.setDatabaseName(_config.databaseName);
        _db.setUserName(_config.username);
        _db.setPassword(_config.password);
    }

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
    executeQuery("CREATE TABLE TASK (ID INTEGER PRIMARY KEY, PROJECT_ID INTEGER, NAME TEXT, FOREIGN KEY (PROJECT_ID) REFERENCES PROJECT(ID) ON DELETE CASCADE)");
    executeQuery("CREATE TABLE ENTRY (ID INTEGER PRIMARY KEY, TASK_ID INTEGER, ENTRY_CONTENT TEXT, TS_FROM INTEGER, TS_UNTIL INTEGER, FOREIGN KEY (TASK_ID) REFERENCES TASK(ID) ON DELETE CASCADE)");
}

void Database::_cleanupData()
{
    executeQuery("DELETE FROM ENTRY;");
    executeQuery("DELETE FROM TASK");
    executeQuery("DELETE FROM PROJECT");
}

QSqlQuery Database::executeQuery(const QString &sql, const QVariantList &bindArgs) const
{
    QSqlQuery query;
    query.prepare(sql);

    for (int i = 0; i < bindArgs.size(); ++i)
    {
        query.bindValue(i, bindArgs[i]);
    }

    query.exec();

    if (query.lastError().type() != QSqlError::NoError)
    {
        qWarning() << query.lastError().text();
        qWarning() << query.executedQuery();
    }

    return query;
}

QSqlQuery Database::executeQuery(const QString &sql) const
{
    return executeQuery(sql, {});
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
