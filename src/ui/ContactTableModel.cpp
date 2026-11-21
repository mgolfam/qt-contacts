#include "ui/ContactTableModel.h"

#include "ui/Text.h"

#include <utility>

namespace contacts::ui {

ContactTableModel::ContactTableModel(QObject* parent) : QAbstractTableModel(parent) {}

void ContactTableModel::setContacts(std::vector<domain::Contact> contacts)
{
    beginResetModel();
    contacts_ = std::move(contacts);
    endResetModel();
}

QModelIndex ContactTableModel::indexOf(domain::ContactId id, int column) const
{
    for (std::size_t row = 0; row < contacts_.size(); ++row) {
        if (contacts_[row].id() == id) return index(static_cast<int>(row), column);
    }
    return {};
}

int ContactTableModel::rowCount(const QModelIndex& parent) const
{
    // Table models have no children: only the invisible root has rows.
    return parent.isValid() ? 0 : static_cast<int>(contacts_.size());
}

int ContactTableModel::columnCount(const QModelIndex& parent) const
{
    return parent.isValid() ? 0 : ColumnCount;
}

QVariant ContactTableModel::data(const QModelIndex& index, int role) const
{
    if (!checkIndex(index, CheckIndexOption::IndexIsValid | CheckIndexOption::ParentIsInvalid)) return {};

    const domain::Contact& contact = contacts_[static_cast<std::size_t>(index.row())];
    switch (role) {
        case Qt::DisplayRole:
        case Qt::ToolTipRole:
            return index.column() == NameColumn ? toQt(contact.name()) : toQt(contact.phone());
        case IdRole:
            return QVariant::fromValue<qint64>(contact.id());
        default:
            return {};
    }
}

QVariant ContactTableModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) return {};
    switch (section) {
        case NameColumn:
            return tr("Name");
        case PhoneColumn:
            return tr("Phone");
        default:
            return {};
    }
}

}  // namespace contacts::ui
