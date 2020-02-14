#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QComboBox>
#include <QtCharts/QChartView>
#include <QtCharts/QPieSeries>
#include "dataprovider.h"
#include "chartdataprovider.h"
#include "projectmodel.h"
#include "taskmodel.h"
#include "entrymodel.h"
#include "entryproxymodel.h"
#include "projecttaskadmindialog.h"
#include "aboutdialog.h"

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
    void on_cboProject_currentIndexChanged(const QString &);

    void on_tblCurrentData_clicked(const QModelIndex &index);

    void on_btnNew_clicked();

    void on_btnDelete_clicked();

    void on_btnSave_clicked();

    void on_actionManage_projects_and_tasks_triggered();

    void onProjectsChanged();

    void onTasksChanged(ENTITY_ID_TYPE projectId);

    void onProjectsFilterChanged();

    void onTasksFilterChanged(ENTITY_ID_TYPE projectId);

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

    void on_cboFilterProject_currentIndexChanged(const QString &arg1);

    void on_cboFilterTask_currentIndexChanged(const QString &arg1);

    void on_btnResetProjectFilter_clicked();

    void on_btnResetTaskFilter_clicked();

    void on_actionExport_current_view_triggered();

signals:
    void updateStartDateFilter(const QDate& dt);
    void updateEndDateFilter(const QDate& dt);
    void updateProjectIdFilter(ENTITY_ID_TYPE projectId);
    void updateTaskIdFilter(ENTITY_ID_TYPE taskId);

private:
    Ui::MainWindow              *ui;
    ProjectTaskAdminDialog      *_projectTaskAdminDialog;
    AboutDialog                 *_aboutDialog;
    QChart                      *_chart;
    QPieSeries                  *_chartData;

    DataProvider                *_provider;
    ChartDataProvider           *_chartDataProvider;
    ProjectModel                *_projectModel;
    TaskModel                   *_taskModel;
    TaskModel                   *_taskFilterModel;
    EntryModel                  *_entryModel;
    EntryProxyModel             *_entryProxyModel;
    Entry                       _selectedEntry;
    QModelIndex                 _selectedRowIndex;
    bool                        _isNewEntry = true;

    void                        _refreshData();

    void                        _connectFilters();
    void                        _resetFilters(const QDate& start, const QDate& end);

    void                        _updateChart();
    bool                        _validateInput();

    void                        _refreshStyle(QWidget* widget);
};
#endif // MAINWINDOW_H
