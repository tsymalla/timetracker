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

    void on_actionAbout_triggered();

    void on_actionCreate_new_database_file_triggered();

    void on_actionExit_triggered();

    void on_btnFilterYesterday_clicked();

    void on_btnFilterToday_clicked();

    void on_btnFilterTomorrow_clicked();

    void on_btnFilterWeek_clicked();

    void on_btnFilterMonth_clicked();

    void on_btnFilterYear_clicked();

    void on_dtFilterStart_userDateChanged(const QDate &date);

    void on_dtFilterEnd_userDateChanged(const QDate &date);

    void on_btnExport_clicked();

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
    EntryModel                  *_entryModel;
    Entry                       _selectedEntry;
    QModelIndex                 _selectedRowIndex;
    bool                        _isNewEntry = true;

    void                        _refreshData();
    void                        _resetFilters(const QDate& start, const QDate& end);

    void                        _updateChart();
};
#endif // MAINWINDOW_H
