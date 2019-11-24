#include "projecttaskadmindialog.h"
#include "ui_projecttaskadmindialog.h"

#include <QInputDialog>

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

void ProjectTaskAdminDialog::on_btnCreateProject_clicked()
{
    QString projectName = QInputDialog::getText(this, tr("New project name"), tr("Enter the new project name."));
    Project p;
    p.name = projectName;

    _projectModel->addRow(std::move(p));
    emit projectsChanged();
}

void ProjectTaskAdminDialog::on_btnDeleteProject_clicked()
{
}

void ProjectTaskAdminDialog::on_lstProjects_clicked(const QModelIndex &index)
{
    _selectedProject = _projectModel->getRow(index.row());
    ui->btnDeleteProject->setEnabled(true);
    _taskModel->setProjectId(_selectedProject.id);
}

void ProjectTaskAdminDialog::on_btnCreateTask_clicked()
{
    QString taskName = QInputDialog::getText(this, tr("New task name"), tr("Enter the new task name."));
    Task t;
    t.projectId = _selectedProject.id;
    t.name = taskName;

    _taskModel->addRow(std::move(t));
    emit tasksChanged(t.projectId);
}

void ProjectTaskAdminDialog::on_lstTasks_clicked(const QModelIndex &index)
{
    _selectedTask = _taskModel->getRow(index.row());
    ui->btnDeleteTask->setEnabled(true);
}
