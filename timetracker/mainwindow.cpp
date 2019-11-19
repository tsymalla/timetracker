#include "mainwindow.h"
#include "ui_mainwindow.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
      , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
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

