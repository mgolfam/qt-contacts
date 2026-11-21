#pragma once

#include "application/ContactService.h"

#include <QMainWindow>

#include <optional>

class QAction;
class QLabel;
class QLineEdit;
class QSortFilterProxyModel;
class QTableView;

namespace contacts::ui {

class ContactTableModel;

/// Shows the contacts and turns user intents (clicks, shortcuts) into calls on
/// ContactService. Holds no business logic: it never decides whether a contact
/// is valid, it only displays what the service says.
class MainWindow final : public QMainWindow {
    Q_OBJECT

public:
    explicit MainWindow(application::ContactService& service, QWidget* parent = nullptr);

protected:
    void closeEvent(QCloseEvent* event) override;

private:
    void createActions();
    void createMenus();
    void createToolBar();
    void createCentralWidget();

    // User intents
    void addContact();
    void editSelectedContact();
    void removeSelectedContact();
    bool saveContacts();
    void reloadContacts();
    void exportContacts();

    /// Called by the service after every change.
    void onContactsChanged();
    void updateActions();
    void updateStatus();

    std::optional<domain::ContactId> selectedId() const;
    void selectContact(domain::ContactId id);

    /// Asks what to do with unsaved changes. Returns false if the user cancelled.
    bool maybeSave();
    void showError(const QString& title, const std::exception& error);

    application::ContactService& service_;

    ContactTableModel* model_ = nullptr;
    QSortFilterProxyModel* proxy_ = nullptr;
    QTableView* table_ = nullptr;
    QLineEdit* search_ = nullptr;
    QLabel* countLabel_ = nullptr;

    QAction* addAction_ = nullptr;
    QAction* editAction_ = nullptr;
    QAction* removeAction_ = nullptr;
    QAction* saveAction_ = nullptr;
    QAction* reloadAction_ = nullptr;
    QAction* exportAction_ = nullptr;
    QAction* findAction_ = nullptr;
    QAction* quitAction_ = nullptr;
    QAction* aboutAction_ = nullptr;
};

}  // namespace contacts::ui
