#include "dataprovider.h"
#include <QVariant>

DataProvider::DataProvider()
{

}

bool DataProvider::isInitialized() const
{
    return _db.isInitialized();
}

QVector<Project> &DataProvider::getAllProjects()
{
    _projects.clear();
    auto result = _db.executeQuery("SELECT * FROM PROJECT");
    while (result.next())
    {
        Project p{result.value(0).toInt(), result.value(1).toString()};
        _projects.push_back(std::move(p));
    }

    return _projects;
}
