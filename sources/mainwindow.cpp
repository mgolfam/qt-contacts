#include "headers/mainwindow.h"
#include "headers/contact.h"
#include "headers/dialogadduser.h"
#include "headers/phonebook.h"
#include "headers/ui_mainwindow.h"

#include <iostream>
#include <string>

#include <QFileDialog>
#include <QList>
#include <QListView>
#include <QListWidgetItem>
#include <QMessageBox>
#include <QString>
#include <QStringList>
#include <QTableView>
#include <QTreeView>

PhoneBook *pbook = new PhoneBook();
QString DB_PATH = "db.dat";

MainWindow::MainWindow(QWidget *parent)
        : QMainWindow(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    this->setCentralWidget(ui->listWidget);
}

MainWindow::~MainWindow() { delete ui; }

void MainWindow::on_actionAdd_triggered() {
    std::cout << "Adding new Contact." << std::endl;
    DialogAddUser diag;

    auto res = diag.exec();

    if (res == QDialog::Accepted) {
        int id = pbook->addContact(diag.name(), diag.phone());
        std::cout << id << std::endl;
        ui->listWidget->addItem(QString::number(id) + " " + diag.name() + " " +
                                diag.phone());
    }
}

void MainWindow::on_actionEdit_triggered() {
    const QString &s = ui->listWidget->currentItem()->text();
    std::cout << s.toStdString() << std::endl;
}

void MainWindow::on_actionExport_triggered() {
    QString path = QFileDialog::getSaveFileName(this, "Save File");
    QFile file(path);

    if (!file.open(QIODevice::WriteOnly)) {
        QMessageBox::critical(this, "Error", file.errorString());
        return;
    }
    file.close();

    ui->statusBar->showMessage(path);
    pbook->exportCsv(path);
}

void MainWindow::on_actionSave_triggered() { pbook->saveData(DB_PATH); }

void MainWindow::delListUI() {
    std::cout << "void MainWindow::delListUI() called." << std::endl;
    QList<QListWidgetItem *> items = ui->listWidget->selectedItems();

//            foreach (QListWidgetItem *item, items) {
//            delete ui->listWidget->takeItem(ui->listWidget->row(item));
//        }

    ui->listWidget->clear();
}

void MainWindow::flushInMemoryContacts() {
    std::cout << "void MainWindow::flushInMemoryContacts() called." << std::endl;
    pbook->getContacts().clear();
}

void MainWindow::loadListUI() {
    std::cout << "void MainWindow::loadListUI() called." << std::endl;
    for (auto c : pbook->getContacts()) {
        std::cout << c->getName().toStdString() << std::endl;
        ui->listWidget->addItem(QString::number(c->getId()) + " " + c->getName() + " " +
                                c->getPhone());
    }
}

void MainWindow::refreshUi() {
    std::cout << "void MainWindow::refreshUi() called." << std::endl;
    delListUI();

    loadListUI();
}

void MainWindow::on_actionLoad_triggered() {
    flushInMemoryContacts();
    pbook->loadData(DB_PATH);
    this->refreshUi();
}

void MainWindow::on_actionRemove_triggered() {
    const QString &s = ui->listWidget->currentItem()->text();
    QStringList lst = s.split(" ");
    std::cout << lst.at(0).toStdString() << std::endl;

    pbook->removeContact(std::stoi(lst.at(0).toStdString()));
    refreshUi();
//    std::cout << s.toStdString() << std::endl;
}
