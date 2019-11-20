#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "dataprovider.h"

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

    void on_btnNew_clicked();

    void on_pushButton_clicked();

private:
    Ui::MainWindow *ui;

    DataProvider _provider;
    bool _isNewEntry = true;

    void _refreshEntries();
};
#endif // MAINWINDOW_H
