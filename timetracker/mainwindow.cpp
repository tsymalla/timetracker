#include "csvwriter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    _provider = new DataProvider(this);

    if (!_provider->isInitialized())
    {
        QMessageBox::warning(this, tr("Error"), "Could not initialize database. Exiting.");
        QApplication::exit();
    }

    // dialogs
    _projectTaskAdminDialog = new ProjectTaskAdminDialog(_provider, this);
    _aboutDialog = new AboutDialog(this);

    // models and their connection
    _entryModel = new EntryModel(this, _provider);
    ui->tblCurrentData->setModel(_entryModel);

    _taskModel = new TaskModel(this, _provider);
    ui->cboTask->setModel(_taskModel);

    _projectModel = new ProjectModel(this, _provider);
    ui->cboProject->setModel(_projectModel);

    // receive events from admin dialog when the data has changed
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::projectsChanged, this, &MainWindow::onProjectsChanged);
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::tasksChanged, this, &MainWindow::onTasksChanged);

    // initialize chart
    _chart = new QChart();
    QFont chartFont;
    chartFont.setPointSize(15);
    chartFont.setBold(true);

    _chart->setTitleFont(std::move(chartFont));
    ui->chrtView->setChart(_chart);

    _chartData = new QPieSeries();
    _chart->addSeries(_chartData);

    _chartDataProvider = new ChartDataProvider(this);

    on_btnNew_clicked();
    on_btnFilterToday_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_cboProject_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->cboTask->clear();
    ui->cboTask->setCurrentIndex(-1);
    auto projectIndex = ui->cboProject->currentIndex();
    const auto& project = _projectModel->getRow(projectIndex);
    _taskModel->setProjectId(project.id);
}

void MainWindow::on_tblCurrentData_clicked(const QModelIndex &index)
{
    _selectedEntry = _entryModel->getRow(index);
    ui->dtFrom->setDateTime(_selectedEntry.from);
    ui->dtUntil->setDateTime(_selectedEntry.until);

    auto projectIndex = _projectModel->getIndex(_selectedEntry.projectId);
    ui->cboProject->setCurrentIndex(projectIndex);

    auto taskIndex = _taskModel->getIndex(_selectedEntry.taskId);
    ui->cboTask->setCurrentIndex(taskIndex);

    ui->txtContent->setText(_selectedEntry.entryContent);
    _selectedRowIndex = index;
    _isNewEntry = false;

    ui->btnDelete->setEnabled(true);
    _isNewEntry = false;
}

void MainWindow::on_btnNew_clicked()
{
    _isNewEntry = true;
    const auto currentDateTime = QDateTime::currentDateTime();

    ui->dtFrom->setDateTime(currentDateTime);
    ui->dtUntil->setDateTime(currentDateTime);
    ui->txtContent->clear();

    ui->btnDelete->setEnabled(false);
    ui->tblCurrentData->clearSelection();
    ui->tblCurrentData->resizeRowsToContents();
}

void MainWindow::on_btnDelete_clicked()
{
    _entryModel->removeRow(_selectedRowIndex, std::move(_selectedEntry));
    statusBar()->showMessage(tr("Deleted entry."));
    on_btnNew_clicked();

    _updateChart();
}

void MainWindow::on_btnSave_clicked()
{
    Task t = _taskModel->getRow(ui->cboTask->currentIndex());

    Entry e;
    e.taskId            = t.id;
    e.entryContent      = ui->txtContent->toPlainText();
    e.from              = ui->dtFrom->dateTime();
    e.until             = ui->dtUntil->dateTime();

    if (_isNewEntry)
    {
        _entryModel->addRow(std::move(e));
        statusBar()->showMessage(tr("Created entry."));

        on_btnNew_clicked();
    }
    else
    {
        e.id = _selectedEntry.id;
        _entryModel->updateRow(std::move(e));
        statusBar()->showMessage(tr("Updated entry."));
    }

    _updateChart();
}

void MainWindow::on_actionManage_projects_and_tasks_triggered()
{
    _projectTaskAdminDialog->show();
}

void MainWindow::onProjectsChanged()
{
    _refreshData();
}

void MainWindow::onTasksChanged(ENTITY_ID_TYPE projectId)
{
    if (projectId == _selectedEntry.projectId)
    {
        _refreshData();
    }
}

void MainWindow::_refreshData()
{
    _projectModel->refresh();
    _entryModel->refresh();

    statusBar()->showMessage(tr("Projects and tasks refreshed."));
}

void MainWindow::_resetFilters(const QDate& start, const QDate& end)
{
    ui->dtFilterStart->setDate(start);
    ui->dtFilterEnd->setDate(end);

    _entryModel->setDateFilter(start, end);
    _updateChart();

    statusBar()->showMessage(tr("Filters refreshed."));
}

void MainWindow::_updateChart()
{
    _chartData->clear();
    const auto& groupedData = _chartDataProvider->getGroupedData(_entryModel->getRows());
    auto it = groupedData.constBegin();

    while (it != groupedData.constEnd())
    {
        const auto& value = it.value();
        auto slice = _chartData->append(it.key(), value.second);
        slice->setLabelVisible(true);
        slice->setLabelPosition(QPieSlice::LabelInsideHorizontal);
        slice->setLabelColor(Qt::white);

        const auto durationDT = QDateTime::fromTime_t(value.first);

        slice->setLabel(QString("%1<br />%2 (%3%)")
                            .arg(it.key())
                            .arg(EntryModel::getDurationString(durationDT))
                            .arg(QString::number(value.second, 'f', 2)));

        ++it;
    }

    const auto durationString = EntryModel::getDurationString(QDateTime::fromTime_t(_chartDataProvider->getTotalTimeSpent()));
    _chart->setTitle(QString("Report from %1 until %2<br>Total time spent: %3")
                         .arg(ui->dtFilterStart->text())
                         .arg(ui->dtFilterEnd->text())
                         .arg(durationString));
}

void MainWindow::on_actionCreate_new_database_file_triggered()
{
}

void MainWindow::on_actionExit_triggered()
{
    QApplication::exit();
}

void MainWindow::on_actionAbout_triggered()
{
    _aboutDialog->show();
}

void MainWindow::on_btnFilterYesterday_clicked()
{
    const auto yesterday = ui->dtFilterStart->date().addDays(-1);

    _resetFilters(yesterday, yesterday);
}

void MainWindow::on_btnFilterToday_clicked()
{
    const auto today = QDate::currentDate();

    _resetFilters(today, today);
}

void MainWindow::on_btnFilterTomorrow_clicked()
{
    const auto tomorrow = ui->dtFilterStart->date().addDays(1);

    _resetFilters(tomorrow, tomorrow);
}

void MainWindow::on_btnFilterWeek_clicked()
{
    const auto today        = QDate::currentDate();

    // get first day of week
    const auto dayOfWeek    = today.dayOfWeek();
    const auto startOfWeek  = today.addDays(-dayOfWeek + 1);
    const auto endOfWeek    = startOfWeek.addDays(6);

    _resetFilters(startOfWeek, endOfWeek);
}

void MainWindow::on_btnFilterMonth_clicked()
{
    const auto today            = QDate::currentDate();

    // get first day of month
    const auto monthNumber      = today.month();
    const auto year             = today.year();
    const auto startDateString  = QString::number(year) + "-" + QString::number(monthNumber) + "-01";
    const auto startOfMonth     = QDate::fromString(startDateString, DATE_FORMAT);

    const auto isYearOverlap    = (monthNumber + 1 > 12);
    const auto nextMonth        = (isYearOverlap) ? 1 : (monthNumber + 1);
    const auto nextYear         = (isYearOverlap) ? year + 1 : year;

    const auto endDateString    = QString::number(nextYear) + "-" + QString::number(nextMonth) + "-01";
    const auto endOfMonth       = QDate::fromString(endDateString, DATE_FORMAT).addDays(-1);

    _resetFilters(startOfMonth, endOfMonth);
}

void MainWindow::on_btnFilterYear_clicked()
{
    const auto today = QDate::currentDate();

    // get first day of month
    const auto year             = today.year();
    const auto startDateString  = QString::number(year) + "-01-01";
    const auto startOfYear      = QDate::fromString(startDateString, DATE_FORMAT);

    const auto endDateString    = QString::number(year + 1) + "-01-01";
    const auto endOfYear        = QDate::fromString(endDateString, DATE_FORMAT).addDays(-1);

    _resetFilters(startOfYear, endOfYear);
}

void MainWindow::on_dtFilterStart_userDateChanged(const QDate &date)
{
    _resetFilters(date, ui->dtFilterEnd->date());
}

void MainWindow::on_dtFilterEnd_userDateChanged(const QDate &date)
{
    _resetFilters(ui->dtFilterStart->date(), date);
}

void MainWindow::on_btnExport_clicked()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export data to CSV"), "", tr("CSV files (*.csv)"));

    if (CSVWriter::toCSV(_entryModel, filePath))
    {
        QMessageBox::information(this, tr("Success"), tr("Data successfully exported to %1.").arg(filePath));
    }
}
