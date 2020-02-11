#include "csvwriter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    this->setWindowIcon(QIcon(":/default/resources/clock-2.ico"));

    _provider = new DataProvider(this);

    if (!_provider->isInitialized())
    {
        QMessageBox::warning(this, tr("Error"), "Could not initialize database. Exiting.");
        QApplication::exit();
    }

    // initialize chart
    _chart = new QChart();
    QFont chartFont;
    chartFont.setPointSize(15);
    chartFont.setBold(true);

    _chart->setTitleFont(chartFont);
    ui->chrtView->setChart(_chart);

    _chartData = new QPieSeries();
    _chart->addSeries(_chartData);

    _chartDataProvider = new ChartDataProvider(this);

    // dialogs
    _projectTaskAdminDialog = new ProjectTaskAdminDialog(_provider, this);
    _aboutDialog = new AboutDialog(this);

    // models and their connection
    _entryModel = new EntryModel(this, _provider);
    _entryProxyModel = new EntryProxyModel(this);
    _entryProxyModel->setSourceModel(_entryModel);
    ui->tblCurrentData->setModel(_entryProxyModel);

    _taskModel = new TaskModel(this, _provider);
    ui->cboTask->setModel(_taskModel);

    _taskFilterModel = new TaskModel(this, _provider);
    ui->cboFilterTask->setModel(_taskFilterModel);

    _projectModel = new ProjectModel(this, _provider);
    ui->cboProject->setModel(_projectModel);
    ui->cboFilterProject->setModel(_projectModel);
    ui->cboFilterProject->setCurrentIndex(-1);

    _connectFilters();

    // receive events from admin dialog when the data has changed
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::projectsChanged, this, &MainWindow::onProjectsChanged);
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::tasksChanged, this, &MainWindow::onTasksChanged);
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::projectsChanged, this, &MainWindow::onProjectsFilterChanged);
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::tasksChanged, this, &MainWindow::onTasksFilterChanged);

    on_btnNew_clicked();
    on_btnFilterToday_clicked();
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::_connectFilters()
{
    connect(this, &MainWindow::updateStartDateFilter, _entryProxyModel, &EntryProxyModel::setStartDate);
    connect(this, &MainWindow::updateEndDateFilter, _entryProxyModel, &EntryProxyModel::setEndDate);
    connect(this, &MainWindow::updateProjectIdFilter, _entryProxyModel, &EntryProxyModel::setProjectId);
    connect(this, &MainWindow::updateTaskIdFilter, _entryProxyModel, &EntryProxyModel::setTaskId);
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
    _entryModel->removeRow(_selectedRowIndex, _selectedEntry);
    statusBar()->showMessage(tr("Deleted entry."));
    on_btnNew_clicked();

    _updateChart();
}

void MainWindow::on_btnSave_clicked()
{
    if (!_validateInput())
    {
        QMessageBox::warning(this, tr("Error"), tr("Please check your input data."));
        return;
    }

    Task t = _taskModel->getRow(ui->cboTask->currentIndex());

    Entry e;
    e.taskId            = t.id;
    e.entryContent      = ui->txtContent->toPlainText();
    e.from              = ui->dtFrom->dateTime();
    e.until             = ui->dtUntil->dateTime();

    if (_isNewEntry)
    {
        _entryModel->addRow(e);
        statusBar()->showMessage(tr("Created entry."));

        on_btnNew_clicked();
    }
    else
    {
        e.id = _selectedEntry.id;
        _entryModel->updateRow(e);
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

void MainWindow::onProjectsFilterChanged()
{
    _refreshData();
}

void MainWindow::onTasksFilterChanged(ENTITY_ID_TYPE projectId)
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

    emit updateStartDateFilter(start);
    emit updateEndDateFilter(end);

    _updateChart();

    statusBar()->showMessage(tr("Filters refreshed."));
}

void MainWindow::_updateChart()
{
    _entryProxyModel->refresh();
    _chartData->clear();
    const auto& groupedData = _chartDataProvider->getGroupedData(_entryProxyModel->getRows());
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
                            .arg(it.key(), EntryModel::getDurationString(durationDT), QString::number(value.second, 'f', 2)));

        ++it;
    }

    const auto timeSpent = QDateTime::fromTime_t(_chartDataProvider->getTotalTimeSpent());

    const auto durationString = EntryModel::getDurationString(timeSpent);
    _chart->setTitle(QString("Report from %1 until %2<br>Total time spent: %3")
                     .arg(ui->dtFilterStart->text(), ui->dtFilterEnd->text(), durationString));

    QString totalTime = durationString;
    totalTime += " (" + EntryModel::getDurationString(timeSpent, true) + ")";
    ui->lblTotalTime->setText(totalTime);
}

bool MainWindow::_validateInput()
{
    auto isValid = true;
    const static char* INVALID_PROPERTY = "isInvalid";

    // valid date input
    if (ui->dtFrom->dateTime() >= ui->dtUntil->dateTime())
    {
        isValid = false;
        ui->dtFrom->setProperty(INVALID_PROPERTY, true);
        ui->dtUntil->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->dtFrom->setProperty(INVALID_PROPERTY, {});
        ui->dtUntil->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->dtFrom);
    this->_refreshStyle(ui->dtUntil);

    // valid project selection
    if (ui->cboProject->currentIndex() <= -1)
    {
        isValid = false;
        ui->cboProject->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->cboProject->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->cboProject);

    // valid task selection
    if (ui->cboTask->currentIndex() <= -1)
    {
        isValid = false;
        ui->cboTask->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->cboTask->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->cboTask);

    // prevent empty contents
    if (ui->txtContent->toPlainText().length() == 0)
    {
        isValid = false;
        ui->txtContent->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->txtContent->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->txtContent);

    return isValid;
}

void MainWindow::_refreshStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
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
    const auto startDateString  = QString::number(year) + "-" + QString::number(monthNumber).rightJustified(2, '0') + "-01";
    const auto startOfMonth     = QDate::fromString(startDateString, DATE_FORMAT);

    const auto isYearOverlap    = (monthNumber + 1 > 12);
    const auto nextMonth        = (isYearOverlap) ? 1 : (monthNumber + 1);
    const auto nextYear         = (isYearOverlap) ? year + 1 : year;

    const auto endDateString    = QString::number(nextYear) + "-" + QString::number(nextMonth).rightJustified(2, '0') + "-01";
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

    if (CSVWriter::toCSV(_entryProxyModel, filePath))
    {
        QMessageBox::information(this, tr("Success"), tr("Data successfully exported to %1.").arg(filePath));
    }
}

void MainWindow::on_cboFilterProject_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->cboFilterTask->clear();
    ui->cboFilterTask->setCurrentIndex(-1);
    auto projectIndex = ui->cboFilterProject->currentIndex();

    if (projectIndex <= -1)
    {
        return;
    }

    const auto& project = _projectModel->getRow(projectIndex);
    _taskFilterModel->setProjectId(project.id);

    emit updateProjectIdFilter(project.id);
    emit updateTaskIdFilter(0);

    _updateChart();
}

void MainWindow::on_cboFilterTask_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    const auto index = ui->cboFilterTask->currentIndex();

    if (index <= -1)
    {
        return;
    }

    Task t = _taskModel->getRow(index);
    emit updateTaskIdFilter(t.id);
    _updateChart();
}

void MainWindow::on_btnResetProjectFilter_clicked()
{
    ui->cboFilterProject->setCurrentIndex(-1);
    ui->cboTask->clear();

    emit updateProjectIdFilter(0);
    emit updateTaskIdFilter(0);

    _updateChart();
}

void MainWindow::on_btnResetTaskFilter_clicked()
{
    ui->cboFilterTask->setCurrentIndex(-1);
    emit updateTaskIdFilter(0);
    _updateChart();
}
