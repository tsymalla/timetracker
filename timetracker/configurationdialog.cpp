#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include <QSettings>
#include <QLineEdit>
#include <QFileDialog>
#include <QMessageBox>
#include "databaseconfiguration.h"

ConfigurationDialog::ConfigurationDialog(QWidget *parent) :
                                                            QDialog(parent),
                                                            ui(new Ui::ConfigurationDialog)
{
    ui->setupUi(this);

    _restoreSettings();
}

ConfigurationDialog::~ConfigurationDialog()
{
    delete ui;
}

void ConfigurationDialog::on_databaseTypeComboBox_currentIndexChanged(const QString &arg1)
{
    _setFieldVisibility(arg1);
}

void ConfigurationDialog::_setFieldVisibility(const QString& databaseType)
{
    const auto type = databaseType.toUpper();
    if (type == "SQLITE")
    {
        ui->txtHost->setEnabled(false);
        ui->txtPort->setEnabled(false);
        ui->txtUsername->setEnabled(false);
        ui->txtPassword->setEnabled(false);
        ui->txtDatabaseName->setEnabled(false);

        ui->txtHost->clear();
        ui->txtPort->clear();
        ui->txtUsername->clear();
        ui->txtPassword->clear();
        ui->txtDatabaseName->clear();

        ui->txtDatabasePath->setEnabled(true);
        ui->btnSelectPath->setEnabled(true);
    }
    else
    {
        ui->txtHost->setEnabled(true);
        ui->txtPort->setEnabled(true);
        ui->txtUsername->setEnabled(true);
        ui->txtPassword->setEnabled(true);
        ui->txtDatabaseName->setEnabled(true);

        ui->txtDatabasePath->clear();
        ui->txtDatabasePath->setEnabled(false);
        ui->btnSelectPath->setEnabled(false);
    }
}

void ConfigurationDialog::_restoreSettings()
{
    QSettings settings;

    // toggle type combo box
    _setFieldVisibility(ui->databaseTypeComboBox->currentText());

    ui->databaseTypeComboBox->setCurrentText(_loadSetting(settings, DatabaseConfiguration::DATABASE_TYPE_KEY));

    ui->txtDatabasePath->setText(_loadSetting(settings, DatabaseConfiguration::PATH_KEY));
    ui->txtPort->setValue(_loadSetting(settings, DatabaseConfiguration::PORT_KEY).toInt());
    ui->txtHost->setText(_loadSetting(settings, DatabaseConfiguration::HOST_KEY));
    ui->txtDatabaseName->setText(_loadSetting(settings, DatabaseConfiguration::DATABASE_NAME_KEY));
    ui->txtUsername->setText(_loadSetting(settings, DatabaseConfiguration::USERNAME_KEY));
    ui->txtPassword->setText(_loadSetting(settings, DatabaseConfiguration::PASSWORD_KEY));
}

QString ConfigurationDialog::_loadSetting(const QSettings& settings, const QString &key, const QString &defaultValue)
{
    return settings.value(key, defaultValue).toString();
}

void ConfigurationDialog::_saveSetting(QSettings& settings, const QString &key, const QString &value)
{
    settings.setValue(key, value);
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    QSettings settings;

    _saveSetting(settings, DatabaseConfiguration::DATABASE_TYPE_KEY, ui->databaseTypeComboBox->currentText());
    _saveSetting(settings, DatabaseConfiguration::PATH_KEY, ui->txtDatabasePath->text());
    _saveSetting(settings, DatabaseConfiguration::PORT_KEY, ui->txtPort->text());
    _saveSetting(settings, DatabaseConfiguration::HOST_KEY, ui->txtHost->text());
    _saveSetting(settings, DatabaseConfiguration::DATABASE_NAME_KEY, ui->txtDatabaseName->text());
    _saveSetting(settings, DatabaseConfiguration::USERNAME_KEY, ui->txtUsername->text());
    _saveSetting(settings, DatabaseConfiguration::PASSWORD_KEY, ui->txtPassword->text());

    QMessageBox::warning(this, tr("Database configuration updated"), tr("The database configuration was updated.\nPlease restart the application in order for the changes to take effect."));
    this->close();
}

void ConfigurationDialog::on_btnSelectPath_clicked()
{
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Select database file"), QDir::homePath(), tr("Database files (*.db)"));
    ui->txtDatabasePath->setText(fileName);
}
