#include "phonebook.h"

#include <iostream>

#include <QDataStream>
#include <QFile>
#include <QTextStream>

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

std::vector<Contact *> PhoneBook::getContacts() { return this->contacts; }

bool PhoneBook::loadData(QString p) {
  QFile file(p);
  file.open(QIODevice::ReadOnly);

  QByteArray data = file.readAll();
  QDataStream a(&data, QIODevice::ReadOnly);
  quint32 magic;
  int id;
  QString name;
  QString phone;

  a >> magic;
  a >> id;
  a >> name;
  a >> phone;

  std::cout << magic << " " << id << " " << name.toStdString() << " "
            << phone.toStdString() << std::endl;

  file.close();
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
