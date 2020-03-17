#ifndef DATABASECONFIGURATION_H
#define DATABASECONFIGURATION_H

#include <QApplication>
#include <QString>
#include <QSettings>

namespace DatabaseConfiguration
{
    const static QString DATABASE_TYPE_KEY = "DATABASE_TYPE";
    const static QString PATH_KEY          = "PATH";
    const static QString HOST_KEY          = "HOST";
    const static QString USERNAME_KEY      = "USERNAME";
    const static QString PASSWORD_KEY      = "PASSWORD";
}

#endif // DATABASECONFIGURATION_H
