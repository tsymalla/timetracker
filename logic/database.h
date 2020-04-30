#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>
#include <cassert>
#include "databaseconfiguration.h"

using ENTITY_ID_TYPE = int;

class Database final: public QObject
{
Q_OBJECT
    friend class DataProvider;
private:
    void _init();
    void _refreshStorage();
    void _cleanupData();
    bool _isInitialized;

    const QString DEFAULT_DATABASE_FILE = "data.db";
    QSqlDatabase _db;

    DatabaseConfiguration::Settings _config;

    template<typename T>
    QSqlQuery _genericSelect(const QStringList& filter, const QVariantList& args) const
    {
        QString sql = "SELECT * FROM " + T::TYPE();

        if (!filter.empty())
        {
            assert(filter.size() == args.size());
            QStringList whereClause;
            for (int i = 0; i < filter.size(); ++i)
            {
                whereClause << filter[i] + " = :" + QString::number(i);
            }

            sql += " WHERE " + whereClause.join(", ");
        }

        sql += ";";

        return executeQuery(sql, args);
    }

    template<typename T>
    QSqlQuery _genericSelectAll() const
    {
        return _genericSelect<T>({}, {});
    }

    template<typename T>
    QSqlQuery _genericInsert(const QStringList& columns, const QVariantList& args)
    {
        const QString entity = T::TYPE();
        QString sql = "INSERT INTO " + entity + " (" + columns.join(',') + ") VALUES (";
        for (int i = 0; i < args.size(); ++i)
        {
            sql += ":" + QString::number(i + 1);
            if (i < args.size() - 1)
            {
                sql += ",";
            }
        }

        sql += ");";

        return executeQuery(sql, args);
    }

    template<typename T>
    QSqlQuery _genericUpdate(const ENTITY_ID_TYPE id, const QStringList& columns, QVariantList& args)
    {
        const QString entity = T::TYPE();
        QString sql = "UPDATE " + entity + " SET ";
        QStringList updateClause;
        for (int i = 0; i < args.size(); ++i)
        {
            updateClause << columns[i] + "=:" + QString::number(i + 1);
        }

        sql += updateClause.join(", ");
        sql += " WHERE ID = :" + QString::number(args.size() + 1);

        args << id;
        return executeQuery(sql, args);
    }

    template<typename T>
    bool _genericDelete(const ENTITY_ID_TYPE id)
    {
        const QString sql = "DELETE FROM " + T::TYPE() + " WHERE ID = :1";
        const QVariantList args({ QVariant(id) });
        auto result = executeQuery(sql, args).lastError().type();

        return result == QSqlError::NoError;
    }

public:
    Database(QObject *parent, const DatabaseConfiguration::Settings& settings);
    Database(const Database&) = delete;
    Database& operator=(const Database&) = delete;
    Database(Database&&) = delete;
    Database& operator=(Database&&) = delete;
    ~Database() final;

    void reset(const DatabaseConfiguration::Settings& settings);

    bool isInitialized() const;
    QSqlQuery executeQuery(const QString& sql, const QVariantList& bindArgs) const;
    QSqlQuery executeQuery(const QString& sql) const;
};

#endif // DATABASE_H
