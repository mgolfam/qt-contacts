#include "infrastructure/CsvContactExporter.h"

#include <QFile>
#include <QTemporaryDir>
#include <QTest>

using namespace contacts;
using infrastructure::CsvContactExporter;

class TestCsvContactExporter : public QObject {
    Q_OBJECT

private slots:
    void escapeField_data()
    {
        QTest::addColumn<QString>("field");
        QTest::addColumn<QString>("expected");
        QTest::newRow("plain") << "Ada" << "Ada";
        QTest::newRow("comma") << "Lovelace, Ada" << "\"Lovelace, Ada\"";
        QTest::newRow("quotes") << "Ada \"Countess\"" << "\"Ada \"\"Countess\"\"\"";
        QTest::newRow("newline") << "a\nb" << "\"a\nb\"";
        QTest::newRow("edge spaces") << " a " << "\" a \"";
    }
    void escapeField()
    {
        QFETCH(QString, field);
        QFETCH(QString, expected);
        QCOMPARE(CsvContactExporter::escapeField(field), expected);
    }

    void writesHeaderAndRows()
    {
        const std::vector<domain::Contact> contacts{domain::Contact(1, "Lovelace, Ada", "+44 20 7946 0000"),
                                                    domain::Contact(2, "Alan", "555 0101")};
        QCOMPARE(CsvContactExporter::toCsv(contacts), QStringLiteral("id,name,phone\r\n"
                                                                     "1,\"Lovelace, Ada\",+44 20 7946 0000\r\n"
                                                                     "2,Alan,555 0101\r\n"));
    }

    void exportWritesUtf8File()
    {
        QTemporaryDir dir;
        QVERIFY(dir.isValid());
        const QString path = dir.filePath(QStringLiteral("contacts.csv"));

        CsvContactExporter().exportTo(path.toStdString(), {domain::Contact(1, "Zoë", "555 0100")});

        QFile file(path);
        QVERIFY(file.open(QIODevice::ReadOnly));
        QCOMPARE(QString::fromUtf8(file.readAll()), QStringLiteral("id,name,phone\r\n1,Zoë,555 0100\r\n"));
    }
};

QTEST_GUILESS_MAIN(TestCsvContactExporter)
#include "tst_csv_contact_exporter.moc"
