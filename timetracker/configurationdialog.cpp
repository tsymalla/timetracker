#include "configurationdialog.h"
#include "ui_configurationdialog.h"

#include <QSettings>
#include <QLineEdit>
#include <QMessageBox>

ConfigurationDialog::ConfigurationDialog(QSettings *settings, QWidget *parent) :
                                                            QDialog(parent),
                                                            ui(new Ui::ConfigurationDialog),
                                                            _settings(settings)
{
    ui->setupUi(this);
    assert(settings != nullptr);

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
    ui->databaseTypeComboBox->setCurrentText(_loadSetting(DATABASE_TYPE_KEY));

    _loadSetting(PATH_KEY, ui->txtDatabasePath);
    _loadSetting(HOST_KEY, ui->txtHost);
    _loadSetting(USERNAME_KEY, ui->txtUsername);
    _loadSetting(PASSWORD_KEY, ui->txtPassword);

    // toggle type combo box
    _setFieldVisibility(ui->databaseTypeComboBox->currentText());
}

QString ConfigurationDialog::_loadSetting(const QString &key, const QString &defaultValue)
{
    return _settings->value(key, defaultValue).toString();
}

void ConfigurationDialog::_loadSetting(const QString &key, QLineEdit* field)
{
    const auto value = _loadSetting(key);
    field->setText(value);
}

void ConfigurationDialog::_saveSetting(const QString &key, const QString &value)
{
    _settings->setValue(key, value);
}

void ConfigurationDialog::_saveSetting(const QString &key, QLineEdit *field)
{
    const auto value = field->text();
    _saveSetting(key, value);
}

void ConfigurationDialog::on_buttonBox_accepted()
{
    _saveSetting(DATABASE_TYPE_KEY, ui->databaseTypeComboBox->currentText());
    _saveSetting(PATH_KEY, ui->txtDatabasePath);
    _saveSetting(HOST_KEY, ui->txtHost);
    _saveSetting(USERNAME_KEY, ui->txtUsername);
    _saveSetting(PASSWORD_KEY, ui->txtPassword);

    QMessageBox::warning(this, tr("Database configuration updated"), tr("The database configuration was updated.\nPlease restart the application in order for the changes to take effect."));
    this->close();
}
