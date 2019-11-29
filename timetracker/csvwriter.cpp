#include "csvwriter.h"

#include <QFile>
#include <QTextStream>

CSVWriter::CSVWriter(QObject *parent) : QObject(parent)
{
}

bool CSVWriter::toCSV(const EntryModel* model, const QString &filePath)
{
    const QString CSV_SEPARATOR = ";";

    if (filePath.isEmpty())
    {
        return false;
    }

    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly | QIODevice::Truncate | QIODevice::Text))
    {
        return false;
    }

    QTextStream stream(&file);

    QStringList headerData;
    // write headers
    for (int i = 0; i < model->columnCount(QModelIndex()); ++i)
    {
        headerData << model->headerData(i, Qt::Horizontal, Qt::DisplayRole).toString();
    }

    stream << headerData.join(CSV_SEPARATOR);
    stream << "\n";

    for (int i = 0; i < model->rowCount(QModelIndex()); ++i)
    {
        QStringList rowData;

        for (int j = 0; j < model->columnCount(QModelIndex()); ++j)
        {
            const auto index = model->index(i, j);
            rowData << "\"" + model->data(index, Qt::DisplayRole).toString() + "\"";
        }

        stream << rowData.join(CSV_SEPARATOR);
        stream << "\n";
    }

    file.close();

    return true;
}
