#ifndef PHONEBOOK_H
#define PHONEBOOK_H

#include <QString>
#include <headers/contact.h>
#include <mutex>
#include <vector>

class PhoneBook {
public:
    PhoneBook();

    int addContact(Contact *c);

    int addContact(QString name, QString phone);

    bool editContact(int i, QString name, QString phone);

    bool removeContact(int i);

    std::vector<Contact *> getContacts();

    QString getUIListStr();

    bool loadData(QString p);

    bool saveData(QString p);

    void exportCsv(QString p);

private:
    int _id;
    std::mutex mux;
    int size = 0;
    std::vector<Contact *> contacts;
};

#endif // PHONEBOOK_H
