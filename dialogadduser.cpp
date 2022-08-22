#include "dialogadduser.h"
#include "ui_dialogadduser.h"

DialogAddUser::DialogAddUser(QWidget *parent)
    : QDialog(parent), ui(new Ui::DialogAddUser) {
  ui->setupUi(this);
}

DialogAddUser::~DialogAddUser() { delete ui; }

QString DialogAddUser::name() { return ui->d_addname->text(); }

QString DialogAddUser::phone() { return ui->d_addphone->text(); }

void DialogAddUser::setName(QString name) { ui->d_addname->setText(name); }

void DialogAddUser::setPhone(QString phone) { ui->d_addphone->setText(phone); }
