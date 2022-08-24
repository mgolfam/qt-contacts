#ifndef CONTACT_H
#define CONTACT_H

#include <QByteArray>
#include <QDataStream>
#include <QString>

class Contact {
public:
    Contact();

    Contact(QString n, QString p);

    int getId();

    QString getName();

    QString getPhone();

    void setId(int i);

    void setName(QString n);

    void setPhone(QString p);

    QByteArray *serialize();

    void serialize(QDataStream &qds);

    QString getCsvRow();

    QString getUIListStr();

    static Contact *deserialize(QByteArray *data);

private:
    int id;
    QString name;
    QString phone;
};

#endif // CONTACT_H
