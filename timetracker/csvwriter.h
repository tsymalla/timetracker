#ifndef CSVWRITER_H
#define CSVWRITER_H

#include "entrymodel.h"

#include <QObject>

class CSVWriter : public QObject
{
    Q_OBJECT
public:
    explicit CSVWriter(QObject *parent = nullptr);

    static bool toCSV(const EntryModel* model, const QString& filePath);

signals:

public slots:
};

#endif // CSVWRITER_H
