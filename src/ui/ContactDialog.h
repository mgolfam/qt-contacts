#pragma once

#include <QDialog>

#include <functional>
#include <optional>

class QDialogButtonBox;
class QLabel;
class QLineEdit;

namespace contacts::ui {

/// Form for adding or editing one contact.
///
/// The dialog contains no business rules. When the user presses OK it calls the
/// SubmitHandler, which runs the real use case. If that returns an error message
/// the dialog stays open and shows it, so the user can fix the input in place.
class ContactDialog final : public QDialog {
    Q_OBJECT

public:
    /// Returns std::nullopt on success, or a message explaining what is wrong.
    using SubmitHandler = std::function<std::optional<QString>(const QString& name, const QString& phone)>;

    ContactDialog(const QString& title, SubmitHandler submit, QWidget* parent = nullptr);

    void setValues(const QString& name, const QString& phone);

    void accept() override;

private:
    void updateOkButton();

    SubmitHandler submit_;
    QLineEdit* name_ = nullptr;
    QLineEdit* phone_ = nullptr;
    QLabel* error_ = nullptr;
    QDialogButtonBox* buttons_ = nullptr;
};

}  // namespace contacts::ui
