#ifndef DATABASECONFIGURATION_H
#define DATABASECONFIGURATION_H

#include <QString>
#include <QSettings>

namespace DatabaseConfiguration
{
    const static QString DATABASE_TYPE_KEY = "DATABASE_TYPE";
    const static QString PATH_KEY          = "PATH";
    const static QString PORT_KEY          = "PORT";
    const static QString HOST_KEY          = "HOST";
    const static QString DATABASE_NAME_KEY = "DATABASE_NAME";
    const static QString USERNAME_KEY      = "USERNAME";
    const static QString PASSWORD_KEY      = "PASSWORD";

    const static int DEFAULT_PORT = 3306;

    struct Settings
    {
        QString type;
        QString path;
        int     port;
        QString host;
        QString databaseName;
        QString username;
        QString password;
    };
}

#endif // DATABASECONFIGURATION_H
