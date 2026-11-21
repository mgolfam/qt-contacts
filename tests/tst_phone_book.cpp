#include "domain/Errors.h"
#include "domain/PhoneBook.h"

#include <QTest>

#include <string>
#include <vector>

using namespace contacts::domain;

class TestPhoneBook : public QObject {
    Q_OBJECT

private slots:
    void assignsIncreasingIds()
    {
        PhoneBook book;
        QCOMPARE(book.add("Ada", "555 0100").id(), 1);
        QCOMPARE(book.add("Alan", "555 0101").id(), 2);
        QCOMPARE(book.size(), std::size_t(2));
    }

    void rejectsDuplicatePhoneNumbersWrittenDifferently()
    {
        PhoneBook book;
        book.add("Ada", "+1 555 010 9999");
        QVERIFY_THROWS_EXCEPTION(ValidationError, book.add("Alan", "1-555-010-9999"));
        QCOMPARE(book.size(), std::size_t(1));
    }

    void updateChangesOnlyThatContact()
    {
        PhoneBook book;
        const Contact ada = book.add("Ada", "555 0100");
        book.add("Alan", "555 0101");

        book.update(ada.id(), "Ada Lovelace", "555 0199");
        QCOMPARE(book.find(ada.id())->name(), std::string("Ada Lovelace"));
        QCOMPARE(book.find(ada.id())->phone(), std::string("555 0199"));
        QCOMPARE(book.find(2)->name(), std::string("Alan"));
    }

    void updateMayKeepItsOwnPhoneNumber()
    {
        PhoneBook book;
        const Contact ada = book.add("Ada", "555 0100");
        book.update(ada.id(), "Ada L.", "555-0100");  // Same digits as before: not a duplicate.
        QCOMPARE(book.find(ada.id())->name(), std::string("Ada L."));
    }

    void failedUpdateChangesNothing()
    {
        PhoneBook book;
        const Contact ada = book.add("Ada", "555 0100");
        book.add("Alan", "555 0101");

        QVERIFY_THROWS_EXCEPTION(ValidationError, book.update(ada.id(), "Ada", "555 0101"));
        QVERIFY_THROWS_EXCEPTION(ValidationError, book.update(ada.id(), "", "555 0100"));
        QCOMPARE(*book.find(ada.id()), ada);
    }

    void unknownIdsAreReported()
    {
        PhoneBook book;
        QVERIFY(book.find(42) == nullptr);
        QVERIFY_THROWS_EXCEPTION(NotFoundError, book.update(42, "Ada", "555 0100"));
        QVERIFY_THROWS_EXCEPTION(NotFoundError, book.remove(42));
    }

    void removeDeletesTheContact()
    {
        PhoneBook book;
        const Contact ada = book.add("Ada", "555 0100");
        book.remove(ada.id());
        QVERIFY(book.empty());
        QVERIFY(book.find(ada.id()) == nullptr);
    }

    void restoredBookContinuesAfterTheHighestId()
    {
        PhoneBook book({Contact(3, "Ada", "555 0100"), Contact(9, "Alan", "555 0101")});
        QCOMPARE(book.add("Grace", "555 0102").id(), 10);
    }

    void restoringRejectsBrokenData()
    {
        QVERIFY_THROWS_EXCEPTION(ValidationError,
                                 PhoneBook({Contact(1, "Ada", "555 0100"), Contact(1, "Alan", "555 0101")}));
        QVERIFY_THROWS_EXCEPTION(ValidationError,
                                 PhoneBook({Contact(1, "Ada", "555 0100"), Contact(2, "Alan", "555-0100")}));
    }
};

QTEST_APPLESS_MAIN(TestPhoneBook)
#include "tst_phone_book.moc"
