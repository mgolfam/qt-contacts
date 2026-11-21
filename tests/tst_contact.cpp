// Domain tests: no Qt besides the test framework, no files, no UI.
#include "domain/Contact.h"
#include "domain/Errors.h"

#include <QTest>

#include <string>

using contacts::domain::Contact;
using contacts::domain::ValidationError;

class TestContact : public QObject {
    Q_OBJECT

private slots:
    void storesNormalizedValues()
    {
        const Contact contact(7, "  Ada \t Lovelace ", "  +44 20 7946 0000 ");
        QCOMPARE(contact.id(), 7);
        QCOMPARE(contact.name(), std::string("Ada Lovelace"));
        QCOMPARE(contact.phone(), std::string("+44 20 7946 0000"));
    }

    void phoneKeyKeepsDigitsOnly()
    {
        QCOMPARE(Contact(1, "A", "+1 (555) 010-9999").phoneKey(), std::string("15550109999"));
    }

    void rejectsInvalidNames_data()
    {
        QTest::addColumn<QString>("name");
        QTest::newRow("empty") << "";
        QTest::newRow("only spaces") << "   \t ";
        QTest::newRow("too long") << QString(int(Contact::kMaxNameLength) + 1, u'x');
    }
    void rejectsInvalidNames()
    {
        QFETCH(QString, name);
        QVERIFY_THROWS_EXCEPTION(ValidationError, Contact(1, name.toStdString(), "555 0100"));
    }

    void countsNameLengthInCharactersNotBytes()
    {
        // 100 × "é" is 200 bytes of UTF-8 but only 100 characters: allowed.
        const QString name(int(Contact::kMaxNameLength), QChar(0x00E9));
        QCOMPARE(Contact(1, name.toStdString(), "555 0100").name(), name.toStdString());
    }

    void rejectsInvalidPhones_data()
    {
        QTest::addColumn<QString>("phone");
        QTest::newRow("empty") << "";
        QTest::newRow("letters") << "555-CALL-NOW";
        QTest::newRow("plus in the middle") << "555+0100";
        QTest::newRow("too few digits") << "12";
        QTest::newRow("too many digits") << "1234567890123456";
    }
    void rejectsInvalidPhones()
    {
        QFETCH(QString, phone);
        QVERIFY_THROWS_EXCEPTION(ValidationError, Contact(1, "Ada", phone.toStdString()));
    }

    void acceptsCommonPhoneFormats_data()
    {
        QTest::addColumn<QString>("phone");
        QTest::newRow("plain") << "5550100";
        QTest::newRow("international") << "+1 (555) 010-9999";
        QTest::newRow("dots") << "555.010.9999";
    }
    void acceptsCommonPhoneFormats()
    {
        QFETCH(QString, phone);
        QCOMPARE(Contact(1, "Ada", phone.toStdString()).phone(), phone.toStdString());
    }

    void rejectsNonPositiveIds() { QVERIFY_THROWS_EXCEPTION(ValidationError, Contact(0, "Ada", "555 0100")); }

    void failedSetterLeavesContactUnchanged()
    {
        Contact contact(1, "Ada", "555 0100");
        QVERIFY_THROWS_EXCEPTION(ValidationError, contact.rename(""));
        QVERIFY_THROWS_EXCEPTION(ValidationError, contact.changePhone("abc"));
        QCOMPARE(contact, Contact(1, "Ada", "555 0100"));
    }
};

QTEST_APPLESS_MAIN(TestContact)
#include "tst_contact.moc"
