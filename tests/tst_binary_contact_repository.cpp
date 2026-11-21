// Infrastructure tests: real files, in a temporary folder that is deleted afterwards.
#include "application/Errors.h"
#include "infrastructure/BinaryContactRepository.h"

#include <QDataStream>
#include <QFile>
#include <QTemporaryDir>
#include <QTest>

using namespace contacts;
using infrastructure::BinaryContactRepository;

class TestBinaryContactRepository : public QObject {
    Q_OBJECT

private slots:
    void init() { QVERIFY(dir_.isValid()); }

    void missingFileLoadsAsEmpty()
    {
        BinaryContactRepository repository(dir_.filePath(QStringLiteral("none.dat")));
        QVERIFY(repository.load().empty());
    }

    void saveThenLoadRoundTripsUnicode()
    {
        const std::vector<domain::Contact> contacts{
            domain::Contact(1, "Ada Lovelace", "+44 20 7946 0000"),
            domain::Contact(5, "مریم میرزاخانی", "+98 21 1234 5678"),
        };
        BinaryContactRepository repository(dir_.filePath(QStringLiteral("nested/folder/contacts.dat")));
        repository.save(contacts);
        QCOMPARE(repository.load(), contacts);
    }

    void rejectsFilesThatAreNotContactFiles()
    {
        const QString path = writeFile(QByteArrayLiteral("hello, world"));
        QVERIFY_THROWS_EXCEPTION(application::StorageError, BinaryContactRepository(path).load());
    }

    void rejectsFilesFromNewerVersions()
    {
        const QString path = writeFile(header(BinaryContactRepository::kFormatVersion + 1, 0));
        QVERIFY_THROWS_EXCEPTION(application::StorageError, BinaryContactRepository(path).load());
    }

    void rejectsTruncatedFiles()
    {
        const QString path = writeFile(header(BinaryContactRepository::kFormatVersion, 3));  // Promises 3, has 0.
        QVERIFY_THROWS_EXCEPTION(application::StorageError, BinaryContactRepository(path).load());
    }

    void rejectsInvalidContactsOnDisk()
    {
        QByteArray bytes = header(BinaryContactRepository::kFormatVersion, 1);
        QDataStream out(&bytes, QIODevice::Append);
        out.setVersion(QDataStream::Qt_6_0);
        out << qint64(1) << QString() << QStringLiteral("555 0100");  // Empty name.
        QVERIFY_THROWS_EXCEPTION(application::StorageError, BinaryContactRepository(writeFile(bytes)).load());
    }

private:
    static QByteArray header(quint16 version, quint32 count)
    {
        QByteArray bytes;
        QDataStream out(&bytes, QIODevice::WriteOnly);
        out.setVersion(QDataStream::Qt_6_0);
        out << BinaryContactRepository::kMagic << version << count;
        return bytes;
    }

    QString writeFile(const QByteArray& bytes)
    {
        const QString path = dir_.filePath(QStringLiteral("test.dat"));
        QFile file(path);
        if (!file.open(QIODevice::WriteOnly) || file.write(bytes) != bytes.size()) qFatal("cannot write test file");
        return path;
    }

    QTemporaryDir dir_;
};

QTEST_GUILESS_MAIN(TestBinaryContactRepository)
#include "tst_binary_contact_repository.moc"
