#include "headers/phonebook.h"

#include <iostream>

#include <QDataStream>
#include <QFile>
#include <QTextStream>

#define MAGIC_NUMBER 0xAA55AA55

PhoneBook::PhoneBook() {
    //    this->contacts = new vector<Contact>();
}

int PhoneBook::addContact(Contact *c) {
    int myid = -1;
    this->mux.lock();
    if (c->getName().length() > 0 && c->getPhone().length() > 0) {
        this->_id++;
        myid = this->_id;
        c->setId(myid);
        this->contacts.push_back(c);
        this->size = this->contacts.size();
    }
    this->mux.unlock();
    return myid;
}

int PhoneBook::addContact(QString name, QString phone) {
    return this->addContact(new Contact(name, phone));
}

bool PhoneBook::removeContact(int id) {

    std::cout << "bool PhoneBook::removeContact(int " << id << ")" << std::endl;

    for (int i = 0; i < this->contacts.size(); i++) {
        std::cout << "traversing: " << contacts[i]->getId() << " "  << contacts[i]->getName().toStdString() << std::endl;
        if (this->contacts[i]->getId() == id) {

            std::cout << "removing: " << contacts[i]->getName().toStdString() << std::endl;

            this->contacts.erase(contacts.begin() + i);
            return true;
        }
    }
    return false;
}

std::vector<Contact *> PhoneBook::getContacts() { return this->contacts; }

bool PhoneBook::loadData(QString p) {
    QFile file(p);
    file.open(QIODevice::ReadOnly);

    QByteArray data = file.readAll();
    QDataStream a(&data, QIODevice::ReadOnly);

    quint32 magic;
    QString id;
    QString name;
    QString phone;

    a >> magic;

    while ((magic == quint32(MAGIC_NUMBER))) {
        a >> id;
        a >> name;
        a >> phone;

        std::cout << magic << " " << id.toStdString() << " " << name.toStdString() << " "
                  << phone.toStdString() << std::endl;

        addContact(name, phone);
        a >> magic;
    }

    file.close();
    std::cout << "loading contacts is finished." << std::endl;
}

bool PhoneBook::saveData(QString p) {
    QFile file(p);
    file.open(QIODevice::WriteOnly);
    QDataStream a(&file);
    for (auto c : this->contacts) {
        //    a << *c->serialize();
        c->serialize(a);
    }
    file.close();
}

void PhoneBook::exportCsv(QString p) {
    QFile file(p);
    if (file.open(QIODevice::ReadWrite)) {
        QTextStream stream(&file);
        for (int i = 0; i < this->size; ++i) {
            stream << contacts.at(i)->getCsvRow() << endl;
        }
    }
    file.close();
}
