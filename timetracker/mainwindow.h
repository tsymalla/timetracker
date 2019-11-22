#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QComboBox>
#include <QMainWindow>
#include "dataprovider.h"
#include "projectmodel.h"
#include "taskmodel.h"
#include "entrymodel.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_cboProject_currentIndexChanged(const QString &);

    void on_tblCurrentData_clicked(const QModelIndex &index);

    void on_btnNew_clicked();

    void on_btnDelete_clicked();

    void on_btnSave_clicked();

    void on_btnCreateProject_clicked();

    void on_lstProjects_clicked(const QModelIndex &index);

    void on_btnDeleteProject_clicked();

private:
    Ui::MainWindow      *ui;

    DataProvider        _provider;
    ProjectModel*       _projectModel;
    TaskModel*          _taskModel;
    EntryModel*         _entryModel;
    Project             _selectedProject;
    Entry               _selectedEntry;
    QModelIndex         _selectedRowIndex;
    bool                _isNewEntry = true;

    void                _setComboItem(QComboBox* item, int value);
};
#endif // MAINWINDOW_H
