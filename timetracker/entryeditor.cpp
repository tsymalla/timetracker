#include "entryeditor.h"
#include "ui_entryeditor.h"

#include <QMessageBox>

const char* EntryEditor::INVALID_PROPERTY = "isInvalid";

EntryEditor::EntryEditor(QWidget *parent) :
                                            QWidget(parent),
                                            ui(new Ui::EntryEditor)
{
    ui->setupUi(this);
}

EntryEditor::~EntryEditor()
{
    delete ui;
}

void EntryEditor::initModels(ProjectModel *projectModel, TaskModel *taskModel, EntryModel* entryModel)
{
    assert(projectModel != nullptr);
    assert(taskModel != nullptr);
    assert(entryModel != nullptr);

    _projectModel = projectModel;
    _taskModel = taskModel;
    _entryModel = entryModel;

    ui->cboTask->setModel(_taskModel);
    ui->cboProject->setModel(_projectModel);
}

void EntryEditor::newEntry()
{
    on_btnNew_clicked();
}

void EntryEditor::setSelectedEntry(const Entry &selectedEntry, const QModelIndex &index)
{
    ui->dtFrom->setDateTime(selectedEntry.from);
    ui->dtUntil->setDateTime(selectedEntry.until);

    auto projectIndex = _projectModel->getIndex(selectedEntry.projectId);
    ui->cboProject->setCurrentIndex(projectIndex);

    auto taskIndex = _taskModel->getIndex(selectedEntry.taskId);
    ui->cboTask->setCurrentIndex(taskIndex);

    ui->txtContent->setText(selectedEntry.entryContent);
    _selectedEntry = selectedEntry;
    _selectedRowIndex = index;
    _isNewEntry = false;

    ui->btnDelete->setEnabled(true);
    _isNewEntry = false;
}

Entry EntryEditor::getSelectedEntry() const
{
    return _selectedEntry;
}

void EntryEditor::refreshModels() const
{
    _projectModel->refresh();
    _taskModel->refresh();
    _entryModel->refresh();
}

void EntryEditor::on_cboProject_currentIndexChanged(const QString &arg1)
{
    Q_UNUSED(arg1)
    ui->cboTask->clear();
    ui->cboTask->setCurrentIndex(-1);
    auto projectIndex = ui->cboProject->currentIndex();
    const auto& project = _projectModel->getRow(projectIndex);
    _taskModel->setProjectId(project.id);
}

void EntryEditor::on_btnNew_clicked()
{
    _isNewEntry = true;
    const auto currentDateTime = QDateTime::currentDateTime();

    ui->dtFrom->setDateTime(currentDateTime);
    ui->dtUntil->setDateTime(currentDateTime);
    ui->txtContent->clear();

    ui->btnDelete->setEnabled(false);

    // reset validation errors
    {
        ui->dtFrom->setProperty(INVALID_PROPERTY, {});
        _refreshStyle(ui->dtFrom);

        ui->dtUntil->setProperty(INVALID_PROPERTY, {});
        _refreshStyle(ui->dtUntil);

        ui->cboProject->setProperty(INVALID_PROPERTY, {});
        _refreshStyle(ui->cboProject);

        ui->cboTask->setProperty(INVALID_PROPERTY, {});
        _refreshStyle(ui->cboTask);

        ui->txtContent->setProperty(INVALID_PROPERTY, {});
        _refreshStyle(ui->txtContent);
    }

    emit newClicked();
}

void EntryEditor::on_btnSave_clicked()
{
    if (!_validateInput())
    {
        QMessageBox::warning(this, tr("Error"), tr("Please check your input data."));
        return;
    }

    Task t = _taskModel->getRow(ui->cboTask->currentIndex());

    Entry e;
    e.taskId            = t.id;
    e.entryContent      = ui->txtContent->toPlainText();
    e.from              = ui->dtFrom->dateTime();
    e.until             = ui->dtUntil->dateTime();

    if (_isNewEntry)
    {
        _entryModel->addRow(e);
        emit entryCreated();

        on_btnNew_clicked();
    }
    else
    {
        e.id = _selectedEntry.id;
        _entryModel->updateRow(e);
        emit entryUpdated();
    }
}

void EntryEditor::on_btnDelete_clicked()
{
    emit entryDeleted(_selectedRowIndex, _selectedEntry);
    on_btnNew_clicked();
}

bool EntryEditor::_validateInput()
{
    auto isValid = true;

    // valid date input
    if (ui->dtFrom->dateTime() >= ui->dtUntil->dateTime())
    {
        isValid = false;
        ui->dtFrom->setProperty(INVALID_PROPERTY, true);
        ui->dtUntil->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->dtFrom->setProperty(INVALID_PROPERTY, {});
        ui->dtUntil->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->dtFrom);
    this->_refreshStyle(ui->dtUntil);

    // valid project selection
    if (ui->cboProject->currentIndex() <= -1)
    {
        isValid = false;
        ui->cboProject->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->cboProject->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->cboProject);

    // valid task selection
    if (ui->cboTask->currentIndex() <= -1)
    {
        isValid = false;
        ui->cboTask->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->cboTask->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->cboTask);

    // prevent empty contents
    if (ui->txtContent->toPlainText().length() == 0)
    {
        isValid = false;
        ui->txtContent->setProperty(INVALID_PROPERTY, true);
    }
    else
    {
        ui->txtContent->setProperty(INVALID_PROPERTY, {});
    }

    this->_refreshStyle(ui->txtContent);

    return isValid;
}

void EntryEditor::_refreshStyle(QWidget *widget)
{
    widget->style()->unpolish(widget);
    widget->style()->polish(widget);
}
