/********************************************************************************
** Form generated from reading UI file 'dialogadduser.ui'
**
** Created by: Qt User Interface Compiler version 5.9.7
**
** WARNING! All changes made in this file will be lost when recompiling UI file!
********************************************************************************/

#ifndef UI_DIALOGADDUSER_H
#define UI_DIALOGADDUSER_H

#include <QtCore/QVariant>
#include <QtWidgets/QAction>
#include <QtWidgets/QApplication>
#include <QtWidgets/QButtonGroup>
#include <QtWidgets/QDialog>
#include <QtWidgets/QDialogButtonBox>
#include <QtWidgets/QHBoxLayout>
#include <QtWidgets/QHeaderView>
#include <QtWidgets/QLabel>
#include <QtWidgets/QLineEdit>
#include <QtWidgets/QWidget>

QT_BEGIN_NAMESPACE

class Ui_DialogAddUser
{
public:
    QDialogButtonBox *buttonBox;
    QWidget *horizontalLayoutWidget_2;
    QHBoxLayout *horizontalLayout_2;
    QLabel *label_2;
    QLineEdit *d_addphone;
    QWidget *layoutWidget;
    QHBoxLayout *horizontalLayout;
    QLabel *label;
    QLineEdit *d_addname;

    void setupUi(QDialog *DialogAddUser)
    {
        if (DialogAddUser->objectName().isEmpty())
            DialogAddUser->setObjectName(QStringLiteral("DialogAddUser"));
        DialogAddUser->resize(400, 300);
        buttonBox = new QDialogButtonBox(DialogAddUser);
        buttonBox->setObjectName(QStringLiteral("buttonBox"));
        buttonBox->setGeometry(QRect(30, 240, 341, 32));
        buttonBox->setOrientation(Qt::Horizontal);
        buttonBox->setStandardButtons(QDialogButtonBox::Cancel|QDialogButtonBox::Ok);
        horizontalLayoutWidget_2 = new QWidget(DialogAddUser);
        horizontalLayoutWidget_2->setObjectName(QStringLiteral("horizontalLayoutWidget_2"));
        horizontalLayoutWidget_2->setGeometry(QRect(40, 110, 319, 139));
        horizontalLayout_2 = new QHBoxLayout(horizontalLayoutWidget_2);
        horizontalLayout_2->setObjectName(QStringLiteral("horizontalLayout_2"));
        horizontalLayout_2->setContentsMargins(0, 0, 0, 0);
        label_2 = new QLabel(horizontalLayoutWidget_2);
        label_2->setObjectName(QStringLiteral("label_2"));

        horizontalLayout_2->addWidget(label_2);

        d_addphone = new QLineEdit(horizontalLayoutWidget_2);
        d_addphone->setObjectName(QStringLiteral("d_addphone"));

        horizontalLayout_2->addWidget(d_addphone);

        layoutWidget = new QWidget(DialogAddUser);
        layoutWidget->setObjectName(QStringLiteral("layoutWidget"));
        layoutWidget->setGeometry(QRect(40, 10, 319, 139));
        horizontalLayout = new QHBoxLayout(layoutWidget);
        horizontalLayout->setObjectName(QStringLiteral("horizontalLayout"));
        horizontalLayout->setContentsMargins(0, 0, 0, 0);
        label = new QLabel(layoutWidget);
        label->setObjectName(QStringLiteral("label"));

        horizontalLayout->addWidget(label);

        d_addname = new QLineEdit(layoutWidget);
        d_addname->setObjectName(QStringLiteral("d_addname"));

        horizontalLayout->addWidget(d_addname);

        QWidget::setTabOrder(d_addname, d_addphone);
        QWidget::setTabOrder(d_addphone, d_addphone);

        retranslateUi(DialogAddUser);
        QObject::connect(buttonBox, SIGNAL(accepted()), DialogAddUser, SLOT(accept()));
        QObject::connect(buttonBox, SIGNAL(rejected()), DialogAddUser, SLOT(reject()));

        QMetaObject::connectSlotsByName(DialogAddUser);
    } // setupUi

    void retranslateUi(QDialog *DialogAddUser)
    {
        DialogAddUser->setWindowTitle(QApplication::translate("DialogAddUser", "Dialog", Q_NULLPTR));
        label_2->setText(QApplication::translate("DialogAddUser", "Phone", Q_NULLPTR));
        label->setText(QApplication::translate("DialogAddUser", "Name", Q_NULLPTR));
    } // retranslateUi

};

namespace Ui {
    class DialogAddUser: public Ui_DialogAddUser {};
} // namespace Ui

QT_END_NAMESPACE

#endif // UI_DIALOGADDUSER_H
