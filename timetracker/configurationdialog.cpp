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
        ui->hostLabel->setVisible(false);
        ui->txtHost->setVisible(false);
        ui->usernameLabel->setVisible(false);
        ui->txtUsername->setVisible(false);
        ui->passswordLabel->setVisible(false);
        ui->txtPassword->setVisible(false);

        ui->pathLabel->setVisible(true);
        ui->txtDatabasePath->setVisible(true);
        ui->btnSelectPath->setVisible(true);
    }
    else
    {
        ui->hostLabel->setVisible(true);
        ui->txtHost->setVisible(true);
        ui->usernameLabel->setVisible(true);
        ui->txtUsername->setVisible(true);
        ui->passswordLabel->setVisible(true);
        ui->txtPassword->setVisible(true);

        ui->pathLabel->setVisible(false);
        ui->txtDatabasePath->setVisible(false);
        ui->btnSelectPath->setVisible(false);
    }
}

void ConfigurationDialog::_restoreSettings()
{
    QSettings settings;

    ui->databaseTypeComboBox->setCurrentText(_loadSetting(settings, DatabaseConfiguration::DATABASE_TYPE_KEY));

    _loadSetting(settings, DatabaseConfiguration::PATH_KEY, ui->txtDatabasePath);
    _loadSetting(settings, DatabaseConfiguration::HOST_KEY, ui->txtHost);
    _loadSetting(settings, DatabaseConfiguration::USERNAME_KEY, ui->txtUsername);
    _loadSetting(settings, DatabaseConfiguration::PASSWORD_KEY, ui->txtPassword);

    // toggle type combo box
    _setFieldVisibility(ui->databaseTypeComboBox->currentText());
}

QString ConfigurationDialog::_loadSetting(const QSettings& settings, const QString &key, const QString &defaultValue)
{
    return settings.value(key, defaultValue).toString();
}

void ConfigurationDialog::_loadSetting(const QSettings& settings, const QString &key, QLineEdit* field)
{
    const auto value = _loadSetting(settings, key);
    field->setText(value);
}

void ConfigurationDialog::_saveSetting(QSettings& settings, const QString &key, const QString &value)
{
    settings.setValue(key, value);
}

void ConfigurationDialog::_saveSetting(QSettings& settings, const QString &key, QLineEdit *field)
{
    const auto value = field->text();
    _saveSetting(settings, key, value);
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    QSettings settings;

    _saveSetting(settings, DatabaseConfiguration::DATABASE_TYPE_KEY, ui->databaseTypeComboBox->currentText());
    _saveSetting(settings, DatabaseConfiguration::PATH_KEY, ui->txtDatabasePath);
    _saveSetting(settings, DatabaseConfiguration::HOST_KEY, ui->txtHost);
    _saveSetting(settings, DatabaseConfiguration::USERNAME_KEY, ui->txtUsername);
    _saveSetting(settings, DatabaseConfiguration::PASSWORD_KEY, ui->txtPassword);

    QMessageBox::warning(this, tr("Database configuration updated"), tr("The database configuration was updated.\nPlease restart the application in order for the changes to take effect."));
    this->close();
}

void ConfigurationDialog::on_btnSelectPath_clicked()
{
    const auto fileName = QFileDialog::getOpenFileName(this, tr("Select database file"), QDir::homePath(), tr("Database files (*.db)"));
    ui->txtDatabasePath->setText(fileName);
}
