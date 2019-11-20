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
        ui->cboProject->addItem(project.name, QVariant(project.id));
    }

    const auto currentDate = QDate::currentDate();
    ui->dtFilterStart->setDate(currentDate);
    ui->dtFilterEnd->setDate(currentDate);

    on_btnNew_clicked();
    _refreshEntries();
}

MainWindow::~MainWindow()
{
    delete ui;
}


void MainWindow::on_cboProject_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->cboTask->clear();

    const auto& t = _provider.getAllTasksByProject(ui->cboProject->itemData(ui->cboProject->currentIndex()).toInt());
    for (const auto& task: t)
    {
        ui->cboTask->addItem(task.name, task.id);
    }
}

void MainWindow::on_btnNew_clicked()
{
    _isNewEntry = true;
    const auto currentDateTime = QDateTime::currentDateTime();

    ui->dtFrom->setDateTime(currentDateTime);
    ui->dtUntil->setDateTime(currentDateTime);
    ui->txtContent->clear();
}

void MainWindow::on_pushButton_clicked()
{
    _provider.addEntry(ui->cboTask->itemData(ui->cboTask->currentIndex()).toInt(), ui->txtContent->toPlainText(), ui->dtFrom->dateTime(), ui->dtUntil->dateTime());
    on_btnNew_clicked();
    _refreshEntries();
}

void MainWindow::_refreshEntries()
{
    const auto& entries = _provider.getAllEntries();

    ui->tblCurrentData->clear();
    ui->tblCurrentData->setRowCount(entries.count());
    ui->tblCurrentData->setHorizontalHeaderLabels(QStringList() << "Task" << "From" << "Until" << "Text");
    for (int i = 0; i < entries.count(); ++i)
    {
        const auto& entry = entries[i];
        ui->tblCurrentData->setItem(i, 0, new QTableWidgetItem(entry.taskId));
        ui->tblCurrentData->setItem(i, 1, new QTableWidgetItem(entry.from.toString(Qt::DateFormat::LocalDate)));
        ui->tblCurrentData->setItem(i, 2, new QTableWidgetItem(entry.until.toString(Qt::DateFormat::LocalDate)));
        ui->tblCurrentData->setItem(i, 3, new QTableWidgetItem(entry.entryContent));
    }
}
