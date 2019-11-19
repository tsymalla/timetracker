#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>

class Database final
{
private:
    void _init();
    void _refreshStorage();
    bool _isInitialized;
    QSqlDatabase _db;
    const QString DATABASE_NAME = "data.db";

public:
    Database();
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;
    ~Database();

    bool isInitialized() const;
    QSqlQuery executeQuery(const QString& sql, const QVariantList& bindArgs) const;
    QSqlQuery executeQuery(const QString& sql) const;
};

#endif // DATABASE_H
