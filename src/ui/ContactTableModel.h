#pragma once

#include "domain/Contact.h"

#include <QAbstractTableModel>

#include <vector>

namespace contacts::ui {

/// Adapts a list of contacts to Qt's model/view framework, so a QTableView can
/// show it (and a QSortFilterProxyModel can sort and search it).
///
/// The model holds its own copy of the rows. That keeps Qt's rule that views
/// must be told *before* the data under them changes: setContacts() swaps the
/// copy inside beginResetModel() / endResetModel().
class ContactTableModel final : public QAbstractTableModel {
    Q_OBJECT

public:
    enum Column { NameColumn, PhoneColumn, ColumnCount };

    /// data(index, IdRole) returns the contact's id (a qint64) for any column.
    static constexpr int IdRole = Qt::UserRole + 1;

    explicit ContactTableModel(QObject* parent = nullptr);

    void setContacts(std::vector<domain::Contact> contacts);

    /// Row of the contact with this id, or an invalid index if there is none.
    QModelIndex indexOf(domain::ContactId id, int column = NameColumn) const;

    int rowCount(const QModelIndex& parent = {}) const override;
    int columnCount(const QModelIndex& parent = {}) const override;
    QVariant data(const QModelIndex& index, int role = Qt::DisplayRole) const override;
    QVariant headerData(int section, Qt::Orientation orientation, int role = Qt::DisplayRole) const override;

private:
    std::vector<domain::Contact> contacts_;
};

}  // namespace contacts::ui
