#include "ui/MainWindow.h"

#include "application/Errors.h"
#include "domain/Errors.h"
#include "ui/ContactDialog.h"
#include "ui/ContactTableModel.h"
#include "ui/Text.h"

#include <QAction>
#include <QApplication>
#include <QCloseEvent>
#include <QFileDialog>
#include <QHeaderView>
#include <QLabel>
#include <QLineEdit>
#include <QMenuBar>
#include <QMessageBox>
#include <QSortFilterProxyModel>
#include <QStandardPaths>
#include <QStatusBar>
#include <QStyle>
#include <QTableView>
#include <QToolBar>

namespace contacts::ui {

namespace {

/// Freedesktop theme icon on Linux, Qt's built-in icon everywhere else.
QIcon themedIcon(const QString& name, QStyle::StandardPixmap fallback, const QStyle* style)
{
    return QIcon::fromTheme(name, style->standardIcon(fallback));
}

}  // namespace

MainWindow::MainWindow(application::ContactService& service, QWidget* parent) : QMainWindow(parent), service_(service)
{
    // "[*]" is where Qt shows a "*" while setWindowModified(true).
    setWindowTitle(tr("Contacts[*]"));
    resize(640, 480);

    createActions();
    createMenus();
    createToolBar();
    createCentralWidget();

    countLabel_ = new QLabel(this);
    statusBar()->addPermanentWidget(countLabel_);

    // Observer: the service calls us back after every change, whoever caused it.
    service_.subscribe([this] { onContactsChanged(); });
    onContactsChanged();
}

// ---- Construction ------------------------------------------------------------

void MainWindow::createActions()
{
    const QStyle* s = style();

    addAction_ =
        new QAction(themedIcon(QStringLiteral("list-add"), QStyle::SP_FileDialogNewFolder, s), tr("&Add…"), this);
    addAction_->setShortcut(QKeySequence::New);
    addAction_->setStatusTip(tr("Add a new contact"));
    connect(addAction_, &QAction::triggered, this, &MainWindow::addContact);

    editAction_ = new QAction(themedIcon(QStringLiteral("document-edit"), QStyle::SP_FileDialogDetailedView, s),
                              tr("&Edit…"), this);
    editAction_->setShortcut(QKeySequence(Qt::CTRL | Qt::Key_E));
    editAction_->setStatusTip(tr("Edit the selected contact"));
    connect(editAction_, &QAction::triggered, this, &MainWindow::editSelectedContact);

    removeAction_ =
        new QAction(themedIcon(QStringLiteral("list-remove"), QStyle::SP_TrashIcon, s), tr("&Remove"), this);
    removeAction_->setShortcut(QKeySequence::Delete);
    removeAction_->setStatusTip(tr("Remove the selected contact"));
    connect(removeAction_, &QAction::triggered, this, &MainWindow::removeSelectedContact);

    saveAction_ =
        new QAction(themedIcon(QStringLiteral("document-save"), QStyle::SP_DialogSaveButton, s), tr("&Save"), this);
    saveAction_->setShortcut(QKeySequence::Save);
    saveAction_->setStatusTip(tr("Save the contacts to disk"));
    connect(saveAction_, &QAction::triggered, this, &MainWindow::saveContacts);

    reloadAction_ = new QAction(themedIcon(QStringLiteral("view-refresh"), QStyle::SP_BrowserReload, s),
                                tr("&Reload from Disk"), this);
    reloadAction_->setShortcut(QKeySequence::Refresh);
    reloadAction_->setStatusTip(tr("Discard unsaved changes and load the saved contacts"));
    connect(reloadAction_, &QAction::triggered, this, &MainWindow::reloadContacts);

    exportAction_ =
        new QAction(themedIcon(QStringLiteral("document-export"), QStyle::SP_FileIcon, s), tr("E&xport as CSV…"), this);
    exportAction_->setStatusTip(tr("Write the contacts to a CSV file for spreadsheets"));
    connect(exportAction_, &QAction::triggered, this, &MainWindow::exportContacts);

    findAction_ = new QAction(tr("&Find"), this);
    findAction_->setShortcut(QKeySequence::Find);
    connect(findAction_, &QAction::triggered, this, [this] {
        search_->setFocus();
        search_->selectAll();
    });

    quitAction_ = new QAction(tr("&Quit"), this);
    quitAction_->setShortcut(QKeySequence::Quit);
    connect(quitAction_, &QAction::triggered, this, &QWidget::close);

    aboutAction_ = new QAction(tr("&About"), this);
    connect(aboutAction_, &QAction::triggered, this, [this] {
        QMessageBox::about(this, tr("About Contacts"),
                           tr("<b>Contacts %1</b><p>A small Qt phone book built to show Clean Architecture: "
                              "domain → application → infrastructure → ui.</p>")
                               .arg(QApplication::applicationVersion()));
    });
}

void MainWindow::createMenus()
{
    QMenu* file = menuBar()->addMenu(tr("&File"));
    file->addAction(saveAction_);
    file->addAction(reloadAction_);
    file->addSeparator();
    file->addAction(exportAction_);
    file->addSeparator();
    file->addAction(quitAction_);

    QMenu* contact = menuBar()->addMenu(tr("&Contact"));
    contact->addAction(addAction_);
    contact->addAction(editAction_);
    contact->addAction(removeAction_);
    contact->addSeparator();
    contact->addAction(findAction_);

    QMenu* help = menuBar()->addMenu(tr("&Help"));
    help->addAction(aboutAction_);
}

void MainWindow::createToolBar()
{
    QToolBar* toolBar = addToolBar(tr("Main"));
    toolBar->setObjectName(QStringLiteral("mainToolBar"));
    toolBar->setMovable(false);
    toolBar->setToolButtonStyle(Qt::ToolButtonTextBesideIcon);
    toolBar->addAction(addAction_);
    toolBar->addAction(editAction_);
    toolBar->addAction(removeAction_);
    toolBar->addSeparator();
    toolBar->addAction(saveAction_);

    auto* spacer = new QWidget(toolBar);
    spacer->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
    toolBar->addWidget(spacer);

    search_ = new QLineEdit(toolBar);
    search_->setPlaceholderText(tr("Search…"));
    search_->setToolTip(tr("Filter by name or phone (Ctrl+F)"));
    search_->setClearButtonEnabled(true);
    search_->setMaximumWidth(240);
    toolBar->addWidget(search_);
}

void MainWindow::createCentralWidget()
{
    model_ = new ContactTableModel(this);

    // The proxy sits between model and view: it sorts and filters without
    // touching the model, so the model stays simple.
    proxy_ = new QSortFilterProxyModel(this);
    proxy_->setSourceModel(model_);
    proxy_->setFilterCaseSensitivity(Qt::CaseInsensitive);
    proxy_->setFilterKeyColumn(-1);  // Search every column.
    proxy_->setSortCaseSensitivity(Qt::CaseInsensitive);
    proxy_->setSortLocaleAware(true);
    connect(search_, &QLineEdit::textChanged, proxy_, &QSortFilterProxyModel::setFilterFixedString);

    table_ = new QTableView(this);
    table_->setModel(proxy_);
    table_->setSelectionBehavior(QAbstractItemView::SelectRows);
    table_->setSelectionMode(QAbstractItemView::SingleSelection);
    table_->setEditTriggers(QAbstractItemView::NoEditTriggers);
    table_->setSortingEnabled(true);
    table_->sortByColumn(ContactTableModel::NameColumn, Qt::AscendingOrder);
    table_->setAlternatingRowColors(true);
    table_->verticalHeader()->hide();
    table_->horizontalHeader()->setSectionResizeMode(ContactTableModel::NameColumn, QHeaderView::Stretch);
    table_->horizontalHeader()->setSectionResizeMode(ContactTableModel::PhoneColumn, QHeaderView::ResizeToContents);
    setCentralWidget(table_);

    connect(table_, &QTableView::doubleClicked, this, &MainWindow::editSelectedContact);
    connect(table_->selectionModel(), &QItemSelectionModel::selectionChanged, this, &MainWindow::updateActions);
    connect(proxy_, &QAbstractItemModel::modelReset, this, &MainWindow::updateStatus);
    connect(proxy_, &QAbstractItemModel::rowsInserted, this, &MainWindow::updateStatus);
    connect(proxy_, &QAbstractItemModel::rowsRemoved, this, &MainWindow::updateStatus);
}

// ---- User intents ------------------------------------------------------------

void MainWindow::addContact()
{
    std::optional<domain::ContactId> added;
    ContactDialog dialog(
        tr("Add Contact"),
        [&](const QString& name, const QString& phone) -> std::optional<QString> {
            try {
                added = service_.add(toStd(name), toStd(phone)).id();
                return std::nullopt;
            } catch (const domain::DomainError& error) {
                return QString::fromStdString(error.what());
            }
        },
        this);

    if (dialog.exec() == QDialog::Accepted && added) {
        search_->clear();  // Make sure the new contact is visible.
        selectContact(*added);
        statusBar()->showMessage(tr("Contact added."), 3000);
    }
}

void MainWindow::editSelectedContact()
{
    const std::optional<domain::ContactId> id = selectedId();
    if (!id) return;
    const std::optional<domain::Contact> contact = service_.find(*id);
    if (!contact) return;

    ContactDialog dialog(
        tr("Edit Contact"),
        [&](const QString& name, const QString& phone) -> std::optional<QString> {
            try {
                service_.update(*id, toStd(name), toStd(phone));
                return std::nullopt;
            } catch (const domain::DomainError& error) {
                return QString::fromStdString(error.what());
            }
        },
        this);
    dialog.setValues(toQt(contact->name()), toQt(contact->phone()));

    if (dialog.exec() == QDialog::Accepted) selectContact(*id);
}

void MainWindow::removeSelectedContact()
{
    const std::optional<domain::ContactId> id = selectedId();
    if (!id) return;
    const std::optional<domain::Contact> contact = service_.find(*id);
    if (!contact) return;

    const auto answer = QMessageBox::question(this, tr("Remove Contact"),
                                              tr("Remove \"%1\" from your contacts?").arg(toQt(contact->name())));
    if (answer != QMessageBox::Yes) return;

    try {
        service_.remove(*id);
        statusBar()->showMessage(tr("Contact removed."), 3000);
    } catch (const domain::DomainError& error) {
        showError(tr("Remove Contact"), error);
    }
}

bool MainWindow::saveContacts()
{
    try {
        service_.save();
        statusBar()->showMessage(tr("Saved to %1").arg(toQt(service_.storageLocation())), 3000);
        return true;
    } catch (const application::StorageError& error) {
        showError(tr("Save Failed"), error);
        return false;
    }
}

void MainWindow::reloadContacts()
{
    if (service_.hasUnsavedChanges()) {
        const auto answer = QMessageBox::question(this, tr("Reload from Disk"),
                                                  tr("Discard your unsaved changes and reload the saved contacts?"));
        if (answer != QMessageBox::Yes) return;
    }

    try {
        service_.load();
        statusBar()->showMessage(tr("Reloaded."), 3000);
    } catch (const std::exception& error) {  // StorageError or invalid data (DomainError)
        showError(tr("Reload Failed"), error);
    }
}

void MainWindow::exportContacts()
{
    const QString folder = QStandardPaths::writableLocation(QStandardPaths::DocumentsLocation);
    const QString path = QFileDialog::getSaveFileName(
        this, tr("Export as CSV"), folder + QStringLiteral("/contacts.csv"), tr("CSV files (*.csv);;All files (*)"));
    if (path.isEmpty()) return;  // Cancelled.

    try {
        service_.exportTo(toStd(path));
        statusBar()->showMessage(tr("Exported %n contact(s) to %1", nullptr, int(service_.contacts().size())).arg(path),
                                 5000);
    } catch (const application::StorageError& error) {
        showError(tr("Export Failed"), error);
    }
}

// ---- Keeping the view in sync ------------------------------------------------

void MainWindow::onContactsChanged()
{
    model_->setContacts(service_.contacts());
    setWindowModified(service_.hasUnsavedChanges());
    updateActions();
    updateStatus();
}

void MainWindow::updateActions()
{
    const bool hasSelection = selectedId().has_value();
    editAction_->setEnabled(hasSelection);
    removeAction_->setEnabled(hasSelection);
    saveAction_->setEnabled(service_.hasUnsavedChanges());
    exportAction_->setEnabled(!service_.contacts().empty());
}

void MainWindow::updateStatus()
{
    const int total = model_->rowCount();
    const int shown = proxy_->rowCount();
    countLabel_->setText(shown == total ? tr("%n contact(s)", nullptr, total)
                                        : tr("%1 of %n contact(s)", nullptr, total).arg(shown));
}

std::optional<domain::ContactId> MainWindow::selectedId() const
{
    const QModelIndexList rows = table_->selectionModel()->selectedRows();
    if (rows.isEmpty()) return std::nullopt;
    return rows.first().data(ContactTableModel::IdRole).value<qint64>();
}

void MainWindow::selectContact(domain::ContactId id)
{
    const QModelIndex index = proxy_->mapFromSource(model_->indexOf(id));
    if (!index.isValid()) return;
    table_->selectionModel()->select(index, QItemSelectionModel::ClearAndSelect | QItemSelectionModel::Rows);
    table_->setCurrentIndex(index);
    table_->scrollTo(index);
}

// ---- Closing -----------------------------------------------------------------

void MainWindow::closeEvent(QCloseEvent* event)
{
    if (maybeSave()) {
        event->accept();
    } else {
        event->ignore();
    }
}

bool MainWindow::maybeSave()
{
    if (!service_.hasUnsavedChanges()) return true;

    const auto answer = QMessageBox::warning(this, tr("Unsaved Changes"), tr("Save your changes before closing?"),
                                             QMessageBox::Save | QMessageBox::Discard | QMessageBox::Cancel);
    if (answer == QMessageBox::Save) return saveContacts();
    return answer == QMessageBox::Discard;
}

void MainWindow::showError(const QString& title, const std::exception& error)
{
    QMessageBox::critical(this, title, QString::fromStdString(error.what()));
}

}  // namespace contacts::ui
