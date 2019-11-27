#include "projecttaskadmindialog.h"
#include "ui_projecttaskadmindialog.h"

#include <QInputDialog>
#include <QMessageBox>

ProjectTaskAdminDialog::ProjectTaskAdminDialog(DataProvider* provider, QWidget *parent) :
                                                                  QDialog(parent),
                                                                  ui(new Ui::ProjectTaskAdminDialog),
                                                                  _provider(provider)
{
    ui->setupUi(this);

    _taskModel = new TaskModel(this, _provider);
    ui->lstTasks->setModel(_taskModel);

    _projectModel = new ProjectModel(this, _provider);
    ui->lstProjects->setModel(_projectModel);
}

ProjectTaskAdminDialog::~ProjectTaskAdminDialog()
{
    delete ui;
}

void ProjectTaskAdminDialog::on_lstProjects_clicked(const QModelIndex &index)
{
    _selectedProject = _projectModel->getRow(index.row());
    ui->btnUpdateProject->setEnabled(true);
    ui->btnDeleteProject->setEnabled(true);
    _taskModel->setProjectId(_selectedProject.id);
}

void ProjectTaskAdminDialog::on_btnCreateProject_clicked()
{
    bool isConfirmed = false;

    QString projectName = QInputDialog::getText(this, tr("New project name"), tr("Enter the new project name."), QLineEdit::Normal, "", &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Project p;
    p.name = projectName;

    _projectModel->addRow(std::move(p));
    emit projectsChanged();
}

void ProjectTaskAdminDialog::on_btnUpdateProject_clicked()
{
    bool isConfirmed = false;
    const auto projectName = QInputDialog::getText(this, tr("New project name"), tr("Enter the new project name."), QLineEdit::Normal, _selectedProject.name, &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Project newProject;
    newProject.id       = _selectedProject.id;
    newProject.name     = projectName;

    _projectModel->updateRow(std::move(newProject));
    emit projectsChanged();
}

void ProjectTaskAdminDialog::on_btnDeleteProject_clicked()
{
    if (QMessageBox::question(this, tr("Delete project"), tr("Do you really want to delete the project?"), QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
    {
        return;
    }

    const auto idx = _projectModel->getIndex(_selectedProject.id);
    _projectModel->removeRow(idx, _selectedProject);

    emit projectsChanged();
    emit tasksChanged(_selectedProject.id);
    ui->btnUpdateProject->setEnabled(false);
    ui->btnDeleteProject->setEnabled(false);
}

void ProjectTaskAdminDialog::on_lstTasks_clicked(const QModelIndex &index)
{
    _selectedTask = _taskModel->getRow(index.row());
    ui->btnUpdateTask->setEnabled(true);
    ui->btnDeleteTask->setEnabled(true);
}

void ProjectTaskAdminDialog::on_btnCreateTask_clicked()
{
    bool isConfirmed = false;
    const auto taskName = QInputDialog::getText(this, tr("New task name"), tr("Enter the new task name."), QLineEdit::Normal, "", &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Task t;
    t.projectId = _selectedProject.id;
    t.name = taskName;

    _taskModel->addRow(std::move(t));
    emit tasksChanged(t.projectId);
}

void ProjectTaskAdminDialog::on_btnUpdateTask_clicked()
{
    bool isConfirmed =      false;
    const auto taskName =   QInputDialog::getText(this, tr("New task name"), tr("Enter the new task name."), QLineEdit::Normal, _selectedTask.name, &isConfirmed);

    if (!isConfirmed)
    {
        return;
    }

    Task newTask;
    newTask         = _selectedTask;
    newTask.name    = taskName;

    _taskModel->updateRow(std::move(newTask));
    emit tasksChanged(newTask.projectId);
}

void ProjectTaskAdminDialog::on_btnDeleteTask_clicked()
{
    if (QMessageBox::question(this, tr("Delete task"), tr("Do you really want to delete the task?"), QMessageBox::Yes | QMessageBox::Cancel) == QMessageBox::Cancel)
    {
        return;
    }

    const auto idx = _taskModel->getIndex(_selectedTask.id);
    _taskModel->removeRow(idx, _selectedTask);

    emit tasksChanged(_selectedProject.id);
    ui->btnUpdateTask->setEnabled(false);
    ui->btnDeleteTask->setEnabled(false);
}
