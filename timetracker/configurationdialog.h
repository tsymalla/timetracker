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
    explicit ConfigurationDialog(QWidget *parent = nullptr);
    ~ConfigurationDialog();

private slots:
    void on_databaseTypeComboBox_currentIndexChanged(const QString &arg1);

    void on_buttonBox_accepted();

    void on_btnSelectPath_clicked();

private:
    Ui::ConfigurationDialog *ui;
    QSettings       *_settings;

    void _setFieldVisibility(const QString& databaseType);
    void _restoreSettings();

    QString _loadSetting(const QSettings& settings, const QString& key, const QString& defaultValue = "");
    void _loadSetting(const QSettings& settings, const QString& key, QLineEdit* field);

    void _saveSetting(QSettings& settings, const QString& key, const QString& value = "");
    void _saveSetting(QSettings& settings, const QString& key, QLineEdit* field);
};

#endif // CONFIGURATIONDIALOG_H
