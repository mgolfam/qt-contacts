#include "contact.h"

#include <iostream>

#include <sstream>
#include <string>

quint32 MAGIC_NUMBER = 0XAA55AA55;

Contact::Contact() {}

Contact::Contact(QString n, QString p) {
  this->name = n;
  this->phone = p;
}

int Contact::getId() { return this->id; }

QString Contact::getName() { return this->name; }

QString Contact::getPhone() { return this->phone; }

void Contact::setId(int i) { this->id = i; }

void Contact::setName(QString n) { this->name = n; }

void Contact::setPhone(QString p) { this->phone = p; }

QString Contact::getUIListStr() {
  return QString::number(id) + " " + this->getName() + " " + this->getPhone();
}

QByteArray *Contact::serialize() {
  QByteArray *bytes = new QByteArray();
  bytes->append(MAGIC_NUMBER);
  bytes->append(this->getId());
  bytes->append(this->getName());
  bytes->append(this->getPhone());
  return bytes;
}

void Contact::serialize(QDataStream &qds) {
  qds << MAGIC_NUMBER;
  qds << this->getId();
  qds << this->getName();
  qds << this->getPhone();
}

Contact *Contact::deserialize(QByteArray *data) { return nullptr; }

QString Contact::getCsvRow() {
  std::stringstream ss;
  ss << this->getId();
  ss << ",";
  ss << '"';
  ss << this->getName().toStdString();
  ss << '"';
  ss << ",";
  ss << '"';
  ss << this->getPhone().toStdString();
  ss << '"';
  std::string s;
  ss >> s;
  return QString::fromStdString(s);
}
