#include "csvwriter.h"
#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>
#include "databaseconfiguration.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

#ifdef Q_OS_MAC
    // maximize window on mac
    setWindowState(Qt::WindowMaximized);
#endif

    this->setWindowIcon(QIcon(":/default/resources/clock-2.ico"));

    _initDatabase();

    // initialize chart
    _chart = new QChart();
    QFont chartFont;
    chartFont.setPointSize(15);
    chartFont.setBold(true);

    _chart->setTitleFont(chartFont);
    _chart->legend()->setAlignment(Qt::AlignRight);
    ui->chrtView->setChart(_chart);

    _chartData = new QPieSeries();
    _chart->addSeries(_chartData);

    _chartDataProvider = new ChartDataProvider(this);

    // dialogs
    _configurationDialog = new ConfigurationDialog(this);
    _aboutDialog = new AboutDialog(this);

    // models and their connection
    _projectModel = new ProjectModel(this, _provider);
    _taskModel = new TaskModel(this, _provider);
    _entryModel = new EntryModel(this, _provider);
    _entryProxyModel = new EntryProxyModel(this);
    _entryProxyModel->setSourceModel(_entryModel);
    ui->tblCurrentData->setModel(_entryProxyModel);

    _projectTreeModel = new ProjectTreeModel(this, _provider);
    ui->trvProject->setModel(_projectTreeModel);

    ui->entryEditor->initModels(_projectModel, _taskModel, _entryModel);

    _connectFilters();

    // receive events from editor
    connect(ui->entryEditor, &EntryEditor::newClicked, this, &MainWindow::newEntry);
    connect(ui->entryEditor, &EntryEditor::entryCreated, this, &MainWindow::createdEntry);
    connect(ui->entryEditor, &EntryEditor::entryUpdated, this, &MainWindow::updatedEntry);
    connect(ui->entryEditor, &EntryEditor::entryDeleted, this, &MainWindow::deletedEntry);

    connect(_configurationDialog, &ConfigurationDialog::databaseConfigChanged, this, &MainWindow::_initDatabase);

    ui->entryEditor->newEntry();
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

void MainWindow::on_tblCurrentData_clicked(const QModelIndex &index)
{
    ui->entryEditor->setSelectedEntry(_entryModel->getRow(_entryProxyModel->getMappedIndex(index)), index);
}

void MainWindow::onProjectsChanged()
{
}

void MainWindow::onTasksChanged(ENTITY_ID_TYPE projectId)
{
    if (projectId == ui->entryEditor->getSelectedEntry().projectId)
    {
        _refreshData();
    }
}

void MainWindow::_initDatabase()
{
    QSettings _databaseConfig;
    DatabaseConfiguration::Settings settings;

    settings.type           = _databaseConfig.value(DatabaseConfiguration::DATABASE_TYPE_KEY, "SQLite").toString();
    settings.path           = _databaseConfig.value(DatabaseConfiguration::PATH_KEY, "").toString();
    settings.port           = _databaseConfig.value(DatabaseConfiguration::PORT_KEY, DatabaseConfiguration::DEFAULT_PORT).toInt();
    settings.host           = _databaseConfig.value(DatabaseConfiguration::HOST_KEY, "").toString();
    settings.databaseName   = _databaseConfig.value(DatabaseConfiguration::DATABASE_NAME_KEY, "").toString();
    settings.username       = _databaseConfig.value(DatabaseConfiguration::USERNAME_KEY, "").toString();
    settings.password       = _databaseConfig.value(DatabaseConfiguration::PASSWORD_KEY, "").toString();

    if (_provider == nullptr)
    {
        _provider = new DataProvider(this, settings);
    }
    else
    {
        _provider->reset(settings);
    }

    if (!_provider->isInitialized())
    {
        QMessageBox::warning(this, tr("Error"), "Could not initialize database. Creating a new local database file.");
        // TODO
    }
}

void MainWindow::_refreshData()
{
    ui->entryEditor->refreshModels();
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
    _chart->setTitle(QString("Report: %1 - %2<br>Time spent total: %3")
                     .arg(ui->dtFilterStart->text(), ui->dtFilterEnd->text(), durationString));

    QString totalTime = durationString;
    totalTime += " (" + EntryModel::getDurationString(timeSpent, true) + ")";
    ui->lblTotalTime->setText(totalTime);
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

void MainWindow::on_actionExport_current_view_triggered()
{
    QString filePath = QFileDialog::getSaveFileName(this, tr("Export data to CSV"), "", tr("CSV files (*.csv)"));

    if (CSVWriter::toCSV(_entryProxyModel, filePath))
    {
        QMessageBox::information(this, tr("Success"), tr("Data successfully exported to %1.").arg(filePath));
    }
}

void MainWindow::on_trvProject_clicked(const QModelIndex &index)
{
    const auto node = _projectTreeModel->getNode(index);
    if (node == nullptr)
    {
        return;
    }

    const auto type = static_cast<TreeItem::TYPE>(node->data(2).toUInt());
    const auto id = node->data(1);

    if (type == TreeItem::TYPE::OTHER)
    {
        // reset filters
        emit updateProjectIdFilter(0);
        emit updateTaskIdFilter(0);
    }
    else if (type == TreeItem::TYPE::PROJECT)
    {
        emit updateProjectIdFilter(id.toInt());
        emit updateTaskIdFilter(0);
    }
    else if (type == TreeItem::TYPE::TASK)
    {
        const auto parent = node->parent();
        if (parent == nullptr)
        {
            return;
        }

        const auto projectId = parent->data(1);
        emit updateProjectIdFilter(projectId.toInt());
        emit updateTaskIdFilter(id.toInt());
    }

    _updateChart();
}

void MainWindow::refreshTree()
{
    ui->trvProject->clearSelection();
    _projectTreeModel->refreshTree();
}

void MainWindow::newEntry()
{
    ui->tblCurrentData->clearSelection();
    ui->tblCurrentData->resizeRowsToContents();
}

void MainWindow::createdEntry()
{
    statusBar()->showMessage(tr("Created entry."));

    _updateChart();
}

void MainWindow::updatedEntry()
{
    statusBar()->showMessage(tr("Updated entry."));

    _updateChart();
}

void MainWindow::deletedEntry(const QModelIndex& selectedRowIndex, const Entry& selectedEntry)
{
    _entryModel->removeRow(_entryProxyModel->getMappedIndex(selectedRowIndex), selectedEntry);

    statusBar()->showMessage(tr("Deleted entry."));

    _updateChart();
}

void MainWindow::on_actionDatabase_configuration_triggered()
{
    _configurationDialog->show();
}

void MainWindow::on_trvProject_customContextMenuRequested(const QPoint &pos)
{
    QMenu contextMenu;
    QModelIndex index = ui->trvProject->indexAt(pos);
    const auto node = _projectTreeModel->getNode(index);

    if (node == nullptr)
    {
        contextMenu.addAction(ui->actionCreate_project);
    }
    else if (index.isValid())
    {
        if (node->isOtherNode())
        {
            contextMenu.addAction(ui->actionCreate_project);
        }
        else
        {
            const auto nodeData = node->_getInternalData();
            _selectedId = nodeData.id;

            if (node->isProjectNode())
            {
                contextMenu.addAction(ui->actionCreate_task);
                contextMenu.addAction(ui->actionRename_project);
                contextMenu.addAction(ui->actionDelete_project);
            }

            if (node->isTaskNode())
            {
                _taskModel->setProjectId(node->parent()->_getInternalData().id);

                contextMenu.addAction(ui->actionRename_task);
                contextMenu.addAction(ui->actionDelete_task);
            }
        }
    }

    contextMenu.exec(ui->trvProject->mapToGlobal(pos));
}

void MainWindow::on_actionCreate_project_triggered()
{
    bool isConfirmed    = false;

    QString projectName = QInputDialog::getText(this, tr("New project name"), tr("Enter the new project name."), QLineEdit::Normal, "", &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Project p;
    p.name              = projectName;

    _projectModel->addRow(p);
    _refreshData();
    refreshTree();
}


void MainWindow::on_actionRename_project_triggered()
{
    const auto selectedProject = _projectModel->getRow(_projectModel->getIndex(_selectedId));
    bool isConfirmed        = false;
    const auto projectName  = QInputDialog::getText(this, tr("New project name"), tr("Enter the new project name."), QLineEdit::Normal, selectedProject.name, &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Project newProject;
    newProject.id           = selectedProject.id;
    newProject.name         = projectName;

    _projectModel->updateRow(newProject);
    _refreshData();
    refreshTree();
}

void MainWindow::on_actionDelete_project_triggered()
{
    if (QMessageBox::question(this, tr("Delete project"), tr("Do you really want to delete the project?"), QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
    {
        return;
    }

    const auto idx = _projectModel->getIndex(_selectedId);
    const auto project = _projectModel->getRow(idx);

    _projectModel->removeRow(idx, project);
    _refreshData();
    refreshTree();
}

void MainWindow::on_actionCreate_task_triggered()
{
    bool isConfirmed    = false;
    const auto taskName = QInputDialog::getText(this, tr("New task name"), tr("Enter the new task name."), QLineEdit::Normal, "", &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Task t;
    t.projectId         = _selectedId;
    t.name              = taskName;

    _taskModel->addRow(t);
    _refreshData();
    refreshTree();
}

void MainWindow::on_actionRename_task_triggered()
{
    const auto selectedIndex = _taskModel->getIndex(_selectedId);
    const auto selectedTask = _taskModel->getRow(selectedIndex);

    bool isConfirmed    = false;
    const auto taskName = QInputDialog::getText(this, tr("New task name"), tr("Enter the new task name."), QLineEdit::Normal, selectedTask.name, &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Task newTask;
    newTask             = selectedTask;
    newTask.name        = taskName;

    _taskModel->updateRow(newTask);

    _refreshData();
    refreshTree();
}

void MainWindow::on_actionDelete_task_triggered()
{
    if (QMessageBox::question(this, tr("Delete task"), tr("Do you really want to delete the task?"), QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
    {
        return;
    }

    const auto idx = _taskModel->getIndex(_selectedId);
    const auto task = _taskModel->getRow(idx);
    _taskModel->removeRow(idx, task);

    _refreshData();
    refreshTree();
}
