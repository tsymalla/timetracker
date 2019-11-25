#ifndef PROJECTTASKADMINDIALOG_H
#define PROJECTTASKADMINDIALOG_H

#include "dataprovider.h"
#include "projectmodel.h"
#include "taskmodel.h"

#include <QDialog>

namespace Ui {
class ProjectTaskAdminDialog;
}

class ProjectTaskAdminDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ProjectTaskAdminDialog(DataProvider* provider, QWidget *parent = nullptr);
    ~ProjectTaskAdminDialog();

signals:
    void projectsChanged();
    void tasksChanged(ENTITY_ID_TYPE projectId);

private slots:
    void on_btnCreateProject_clicked();

    void on_btnDeleteProject_clicked();

    void on_lstProjects_clicked(const QModelIndex &index);

    void on_btnCreateTask_clicked();

    void on_lstTasks_clicked(const QModelIndex &index);

    void on_btnUpdateProject_clicked();

    void on_btnUpdateTask_clicked();

private:
    Ui::ProjectTaskAdminDialog  *ui;
    DataProvider                *_provider;
    ProjectModel                *_projectModel;
    TaskModel                   *_taskModel;
    Project                     _selectedProject;
    Task                        _selectedTask;
};

#endif // PROJECTTASKADMINDIALOG_H
