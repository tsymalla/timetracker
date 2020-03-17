#ifndef CONFIGURATIONDIALOG_H
#define CONFIGURATIONDIALOG_H

#include <QDialog>

namespace Ui {
class ConfigurationDialog;
}

class QSettings;
class QLineEdit;

class ConfigurationDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ConfigurationDialog(QSettings *settings, QWidget *parent = nullptr);
    ~ConfigurationDialog();

private slots:
    void on_databaseTypeComboBox_currentIndexChanged(const QString &arg1);

    void on_buttonBox_accepted();

private:
    Ui::ConfigurationDialog *ui;
    QSettings       *_settings;

    void _setFieldVisibility(const QString& databaseType);
    void _restoreSettings();
    QString _loadSetting(const QString& key, const QString& defaultValue = "");
    void _loadSetting(const QString& key, QLineEdit* field);

    void _saveSetting(const QString& key, const QString& value = "");
    void _saveSetting(const QString& key, QLineEdit* field);

    const QString DATABASE_TYPE_KEY = "DATABASE_TYPE";
    const QString PATH_KEY          = "PATH";
    const QString HOST_KEY          = "HOST";
    const QString USERNAME_KEY      = "USERNAME";
    const QString PASSWORD_KEY      = "PASSWORD";
};

#endif // CONFIGURATIONDIALOG_H
