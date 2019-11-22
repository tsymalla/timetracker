#include "mainwindow.h"
#include "ui_mainwindow.h"
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);

    if (!_provider.isInitialized())
    {
        QMessageBox::warning(this, tr("Error"), "Could not initialize database. Exiting.");
        QApplication::exit();
    }

    _entryModel = new EntryModel(this, _provider);
    ui->tblCurrentData->setModel(_entryModel);

    _projectModel = new ProjectModel(this, _provider);
    ui->lstProjects->setModel(_projectModel);
    ui->cboProject->setModel(_projectModel);

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
    auto projectIndex = ui->cboProject->currentIndex();
    const auto& project = _projectModel->getRow(projectIndex);
    const auto& t = _provider.getAllTasksByProject(project.id);
    for (const auto& task: t)
    {
        ui->cboTask->addItem(task.name, task.id);
    }
}

void MainWindow::on_tblCurrentData_clicked(const QModelIndex &index)
{
    _selectedEntry = _entryModel->getRow(index);
    ui->dtFrom->setDateTime(_selectedEntry.from);
    ui->dtUntil->setDateTime(_selectedEntry.until);
    _setComboItem(ui->cboProject, _selectedEntry.projectId);
    _setComboItem(ui->cboTask, _selectedEntry.taskId);
    ui->txtContent->setText(_selectedEntry.entryContent);
    _selectedRowIndex = index;

    ui->btnDelete->setEnabled(true);
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
    Entry e;
    e.taskId =          ui->cboTask->itemData(ui->cboTask->currentIndex()).toInt();
    e.entryContent =    ui->txtContent->toPlainText();
    e.from =            ui->dtFrom->dateTime();
    e.until =           ui->dtUntil->dateTime();

    _entryModel->addRow(std::move(e));
    on_btnNew_clicked();
}

void MainWindow::_setComboItem(QComboBox *item, int value)
{
    auto index = item->findData(value);
    if (index != -1)
    {
        item->setCurrentIndex(index);
    }
}

void MainWindow::on_btnCreateProject_clicked()
{
    QString projectName = QInputDialog::getText(this, tr("New project name"), tr("Enter the new project name."));
    Project p;
    p.name = projectName;

    _projectModel->addRow(std::move(p));
}

void MainWindow::on_lstProjects_clicked(const QModelIndex &index)
{
    _selectedProject = _projectModel->getRow(index.row());
    ui->btnDeleteProject->setEnabled(true);
}

void MainWindow::on_btnDeleteProject_clicked()
{
    //_projectModel->removeRow()
}
