#ifndef ENTRYEDITOR_H
#define ENTRYEDITOR_H

#include <QWidget>
#include "projectmodel.h"
#include "taskmodel.h"
#include "entrymodel.h"

namespace Ui {
class EntryEditor;
}

class EntryEditor : public QWidget
{
    Q_OBJECT

public:
    explicit EntryEditor(QWidget *parent = nullptr);
    ~EntryEditor();

    void initModels(ProjectModel* projectModel, TaskModel *taskModel, EntryModel *entryModel);
    void newEntry();
    void setSelectedEntry(const Entry& selectedEntry, const QModelIndex &index);
    Entry getSelectedEntry() const;
    void refreshModels() const;

signals:
    void newClicked();
    void entryCreated();
    void entryUpdated();
    void entryDeleted(const QModelIndex&, const Entry&);

private:
    Ui::EntryEditor *ui;
    const static char* INVALID_PROPERTY;

    ProjectModel                *_projectModel;
    TaskModel                   *_taskModel;
    EntryModel                  *_entryModel;
    bool                        _isNewEntry = true;
    QModelIndex                 _selectedRowIndex;
    Entry                       _selectedEntry;

    bool                        _validateInput();

    void                        _refreshStyle(QWidget* widget);

private slots:
    void on_cboProject_currentIndexChanged(const QString &);

    void on_btnNew_clicked();

    void on_btnSave_clicked();

    void on_btnDelete_clicked();

    void _onStartEndDateChanged();
};

#endif // ENTRYEDITOR_H
