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

    _entryModel = new EntryModel(this, _provider);
    ui->tblCurrentData->setModel(_entryModel);

    _taskModel = new TaskModel(this, _provider);
    ui->cboTask->setModel(_taskModel);

    _projectTaskAdminDialog = new ProjectTaskAdminDialog(_provider, this);

    _projectModel = new ProjectModel(this, _provider);
    ui->cboProject->setModel(_projectModel);

    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::projectsChanged, this, &MainWindow::onProjectsChanged);
    connect(_projectTaskAdminDialog, &ProjectTaskAdminDialog::tasksChanged, this, &MainWindow::onTasksChanged);

    const auto currentDate = QDate::currentDate();
    ui->dtFilterStart->setDate(currentDate);
    ui->dtFilterEnd->setDate(currentDate);

    on_btnNew_clicked();
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
    on_btnNew_clicked();
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

        on_btnNew_clicked();
    }
    else
    {
        e.id = _selectedEntry.id;
        _entryModel->updateRow(std::move(e));
    }
}

void MainWindow::on_actionManage_projects_and_tasks_triggered()
{
    _projectTaskAdminDialog->show();
}

void MainWindow::onProjectsChanged()
{
    _projectModel->refresh();
    _entryModel->refresh();
}

void MainWindow::onTasksChanged(ENTITY_ID_TYPE projectId)
{
    if (projectId == _selectedEntry.projectId)
    {
        _taskModel->refresh();
        _entryModel->refresh();
    }
}
