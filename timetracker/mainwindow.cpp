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

    const auto& p = _provider.getAllProjects();
    for (const auto& project: p)
    {
        ui->cboProject->addItem(project.name, project.id);
    }

    const auto currentDate = QDate::currentDate();
    const auto currentTime = QDateTime::currentDateTime().time();

    ui->dtFrom->setDate(currentDate);
    ui->dtUntil->setDate(currentDate);
    ui->tmFrom->setTime(currentTime);
    ui->tmUntil->setTime(currentTime);
    ui->dtFilterStart->setDate(currentDate);
    ui->dtFilterEnd->setDate(currentDate);
}

MainWindow::~MainWindow()
{
    delete ui;
}

