#ifndef CHARTDATAPROVIDER_H
#define CHARTDATAPROVIDER_H

#include <QMap>
#include <QObject>
#include "dataprovider.h"

class ChartDataProvider: public QObject
{
Q_OBJECT
private:
    // Project / task, total time, percentage time
    using MAP_TYPE = QMap<QString, QPair<quint64, qreal>>;
    MAP_TYPE _groupedData;
    qreal _totalTimeSpent;
public:
    ChartDataProvider(QObject *parent);

    MAP_TYPE& getGroupedData(const QVector<Entry>& entryData);
    qreal getTotalTimeSpent() const;
};

#endif // CHARTDATAPROVIDER_H
