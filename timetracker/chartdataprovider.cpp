#include "chartdataprovider.h"

ChartDataProvider::ChartDataProvider(QObject *parent): QObject(parent)
{

}

/**
 * @brief ChartDataProvider::getGroupedData
 * @param entryData
 * @return Project and tasks assigned their total duration
 */
ChartDataProvider::MAP_TYPE ChartDataProvider::getGroupedData(const QVector<Entry> &entryData)
{
    _groupedData.clear();
    _totalTimeSpent = 0;

    // gather total data
    for (const auto& entry: entryData)
    {
        const auto duration = entry.duration.toTime_t();
        if (duration <= 0)
        {
            continue;
        }

        const QString row = entry.projectName + ": " + entry.taskName;
        _groupedData[row].first += duration;
        _totalTimeSpent += static_cast<qreal>(duration);
    }

    auto it = _groupedData.begin();
    while (it != _groupedData.end())
    {
        it.value().second = (static_cast<qreal>(it.value().first) / _totalTimeSpent) * 100.0;
        ++it;
    }

    return _groupedData;
}

qreal ChartDataProvider::getTotalTimeSpent() const
{
    return _totalTimeSpent;
}
