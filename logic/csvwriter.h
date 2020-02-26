#ifndef CSVWRITER_H
#define CSVWRITER_H

#include "entryproxymodel.h"

#include <QObject>

class CSVWriter : public QObject
{
    Q_OBJECT
public:
    explicit CSVWriter(QObject *parent = nullptr);

    static bool toCSV(const EntryProxyModel* model, const QString& filePath);
};

#endif // CSVWRITER_H
