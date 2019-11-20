#ifndef DATABASE_H
#define DATABASE_H

#include <QSqlDatabase>
#include <QSqlDriver>
#include <QSqlError>
#include <QSqlQuery>
#include <QVariant>

using ENTITY_ID_TYPE = int;

class Database final
{
    friend class DataProvider;
private:
    void _init();
    void _refreshStorage();
    bool _isInitialized;
    QSqlDatabase _db;
    const QString DATABASE_NAME = "data.db";

    template<typename T>
    QSqlQuery _genericSelect(const QStringList& filter, const QVariantList& args)
    {
        QString sql = "SELECT * FROM " + T::TYPE();

        if (filter.size() > 0)
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
    QSqlQuery _genericSelectAll()
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
    bool _genericDelete(const ENTITY_ID_TYPE id)
    {
        const QString sql = "DELETE FROM " + T::TYPE() + " WHERE ID = :1";
        const QVariantList args({ QVariant(id) });
        auto result = executeQuery(sql, args).lastError().type();

        return result == QSqlError::NoError;
    }

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
