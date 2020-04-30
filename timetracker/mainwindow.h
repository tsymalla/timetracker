#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSettings>
#include <QComboBox>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include "dataprovider.h"
#include "chartdataprovider.h"
#include "projectmodel.h"
#include "taskmodel.h"
#include "entrymodel.h"
#include "entryproxymodel.h"
#include "configurationdialog.h"
#include "aboutdialog.h"
#include "projecttreemodel.h"
#include "entryeditor.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

QT_CHARTS_USE_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:

    void on_actionAbout_triggered();

    void on_actionExit_triggered();

    void on_btnFilterYesterday_clicked();

    void on_btnFilterToday_clicked();

    void on_btnFilterTomorrow_clicked();

    void on_btnFilterWeek_clicked();

    void on_btnFilterMonth_clicked();

    void on_btnFilterYear_clicked();

    void on_dtFilterStart_userDateChanged(const QDate &date);

    void on_dtFilterEnd_userDateChanged(const QDate &date);

    void on_actionExport_current_view_triggered();

    void on_trvProject_clicked(const QModelIndex &index);

    void refreshTree();

    void newEntry();

    void createdEntry();

    void updatedEntry();

    void deletedEntry(const QModelIndex& selectedRowIndex, const Entry& selectedEntry);

    void on_actionDatabase_configuration_triggered();

    void on_trvProject_customContextMenuRequested(const QPoint &pos);

    void on_actionCreate_project_triggered();

    void on_actionDelete_project_triggered();

    void on_actionRename_project_triggered();

    void on_actionCreate_task_triggered();

    void on_actionRename_task_triggered();

    void on_actionDelete_task_triggered();

    void onTblSelectionChanged(const QModelIndex &current, const QModelIndex &previous);

signals:
    void updateStartDateFilter(const QDate& dt);
    void updateEndDateFilter(const QDate& dt);
    void updateProjectIdFilter(ENTITY_ID_TYPE projectId);
    void updateTaskIdFilter(ENTITY_ID_TYPE taskId);

private:
    Ui::MainWindow              *ui;
    QSettings                   _databaseConfig;

    ConfigurationDialog         *_configurationDialog;
    AboutDialog                 *_aboutDialog;
    QChart                      *_chart;
    QPieSeries                  *_chartData;

    DataProvider                *_provider = nullptr;
    ChartDataProvider           *_chartDataProvider;
    ProjectModel                *_projectModel;
    TaskModel                   *_taskModel;
    EntryModel                  *_entryModel;
    EntryProxyModel             *_entryProxyModel;
    ProjectTreeModel            *_projectTreeModel;
    ENTITY_ID_TYPE              _selectedId = 0;

    void                        _initDatabase();
    void                        _refreshData();

    void                        _connectFilters();
    void                        _resetFilters(const QDate& start, const QDate& end);

    void                        _updateChart();
};
#endif // MAINWINDOW_H
