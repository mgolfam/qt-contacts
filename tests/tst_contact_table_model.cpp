// UI test without opening a window: models are plain QObjects.
#include "ui/ContactTableModel.h"

#include <QAbstractItemModelTester>
#include <QTest>

using namespace contacts;
using ui::ContactTableModel;

class TestContactTableModel : public QObject {
    Q_OBJECT

private slots:
    void showsContactsInColumns()
    {
        ContactTableModel model;
        // Checks, after every change, that the model follows Qt's model rules.
        QAbstractItemModelTester tester(&model, QAbstractItemModelTester::FailureReportingMode::QtTest);

        model.setContacts({domain::Contact(4, "Ada", "555 0100"), domain::Contact(9, "Alan", "555 0101")});

        QCOMPARE(model.rowCount(), 2);
        QCOMPARE(model.columnCount(), int(ContactTableModel::ColumnCount));
        QCOMPARE(model.index(1, ContactTableModel::NameColumn).data().toString(), QStringLiteral("Alan"));
        QCOMPARE(model.index(1, ContactTableModel::PhoneColumn).data().toString(), QStringLiteral("555 0101"));
        QCOMPARE(model.index(0, ContactTableModel::PhoneColumn).data(ContactTableModel::IdRole).value<qint64>(), 4);
        QCOMPARE(model.headerData(ContactTableModel::NameColumn, Qt::Horizontal).toString(), QStringLiteral("Name"));
    }

    void findsRowsById()
    {
        ContactTableModel model;
        model.setContacts({domain::Contact(4, "Ada", "555 0100"), domain::Contact(9, "Alan", "555 0101")});
        QCOMPARE(model.indexOf(9).row(), 1);
        QVERIFY(!model.indexOf(42).isValid());
    }
};

QTEST_GUILESS_MAIN(TestContactTableModel)
#include "tst_contact_table_model.moc"
