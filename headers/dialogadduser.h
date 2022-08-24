#ifndef DIALOGADDUSER_H
#define DIALOGADDUSER_H

#include <QDialog>

namespace Ui {
    class DialogAddUser;
}

class DialogAddUser : public QDialog {
Q_OBJECT

public:
    explicit DialogAddUser(QWidget *parent = nullptr);

    ~DialogAddUser();

    QString name();

    QString phone();

    void setName(QString name);

    void setPhone(QString phone);

private:
    Ui::DialogAddUser *ui;
};

#endif // DIALOGADDUSER_H
