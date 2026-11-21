#include "ui/ContactDialog.h"

#include <QDialogButtonBox>
#include <QFormLayout>
#include <QLabel>
#include <QLineEdit>
#include <QPushButton>
#include <QVBoxLayout>

#include <utility>

namespace contacts::ui {

ContactDialog::ContactDialog(const QString& title, SubmitHandler submit, QWidget* parent)
    : QDialog(parent), submit_(std::move(submit))
{
    setWindowTitle(title);
    setMinimumWidth(360);

    name_ = new QLineEdit(this);
    name_->setPlaceholderText(tr("e.g. Ada Lovelace"));
    phone_ = new QLineEdit(this);
    phone_->setPlaceholderText(tr("e.g. +1 (555) 010-9999"));

    error_ = new QLabel(this);
    error_->setWordWrap(true);
    error_->setStyleSheet(QStringLiteral("color: #c62828;"));
    error_->hide();

    buttons_ = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel, this);

    auto* form = new QFormLayout;
    form->addRow(tr("&Name:"), name_);
    form->addRow(tr("&Phone:"), phone_);

    auto* layout = new QVBoxLayout(this);
    layout->addLayout(form);
    layout->addWidget(error_);
    layout->addWidget(buttons_);

    connect(buttons_, &QDialogButtonBox::accepted, this, &ContactDialog::accept);
    connect(buttons_, &QDialogButtonBox::rejected, this, &ContactDialog::reject);
    connect(name_, &QLineEdit::textChanged, this, &ContactDialog::updateOkButton);
    connect(phone_, &QLineEdit::textChanged, this, &ContactDialog::updateOkButton);

    updateOkButton();
}

void ContactDialog::setValues(const QString& name, const QString& phone)
{
    name_->setText(name);
    phone_->setText(phone);
}

void ContactDialog::accept()
{
    if (const std::optional<QString> error = submit_(name_->text(), phone_->text())) {
        error_->setText(*error);
        error_->show();
        return;  // Stay open so the user can correct the input.
    }
    QDialog::accept();
}

void ContactDialog::updateOkButton()
{
    // A quick hint only; the real validation happens in the domain layer.
    const bool filled = !name_->text().trimmed().isEmpty() && !phone_->text().trimmed().isEmpty();
    buttons_->button(QDialogButtonBox::Ok)->setEnabled(filled);
    error_->hide();
}

}  // namespace contacts::ui
