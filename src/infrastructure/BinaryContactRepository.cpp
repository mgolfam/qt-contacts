#include "infrastructure/BinaryContactRepository.h"

#include "application/Errors.h"
#include "domain/Errors.h"

#include <QDataStream>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QSaveFile>
#include <QStandardPaths>

#include <utility>

namespace contacts::infrastructure {

namespace {

/// Pin the stream format so files stay readable when Qt changes its defaults.
constexpr QDataStream::Version kStreamVersion = QDataStream::Qt_6_0;

/// Protects against allocating huge amounts of memory for a corrupt count.
constexpr quint32 kMaxContacts = 1'000'000;

[[noreturn]] void fail(const QString& message)
{
    throw application::StorageError(message.toStdString());
}

}  // namespace

BinaryContactRepository::BinaryContactRepository(QString filePath) : filePath_(std::move(filePath)) {}

QString BinaryContactRepository::defaultFilePath()
{
    return QStandardPaths::writableLocation(QStandardPaths::AppDataLocation) + QStringLiteral("/contacts.dat");
}

std::vector<domain::Contact> BinaryContactRepository::load()
{
    QFile file(filePath_);
    if (!file.exists()) return {};  // First run: nothing saved yet.
    if (!file.open(QIODevice::ReadOnly)) fail(QStringLiteral("Cannot open %1: %2").arg(filePath_, file.errorString()));

    QDataStream in(&file);
    in.setVersion(kStreamVersion);

    quint32 magic = 0;
    quint16 version = 0;
    quint32 count = 0;
    in >> magic >> version >> count;

    if (in.status() != QDataStream::Ok || magic != kMagic) {
        fail(QStringLiteral("%1 is not a contacts file.").arg(filePath_));
    }
    if (version > kFormatVersion) {
        fail(QStringLiteral("%1 was saved by a newer version of this app (format %2).").arg(filePath_).arg(version));
    }
    if (count > kMaxContacts) fail(QStringLiteral("%1 is corrupt.").arg(filePath_));

    std::vector<domain::Contact> contacts;
    contacts.reserve(count);
    for (quint32 i = 0; i < count; ++i) {
        qint64 id = 0;
        QString name;
        QString phone;
        in >> id >> name >> phone;
        if (in.status() != QDataStream::Ok) fail(QStringLiteral("%1 is truncated or corrupt.").arg(filePath_));

        // Data on disk may have been edited by hand or written by a buggy version,
        // so it goes through the same validation as user input.
        try {
            contacts.emplace_back(id, name.toStdString(), phone.toStdString());
        } catch (const domain::DomainError& error) {
            fail(QStringLiteral("%1 contains an invalid contact: %2")
                     .arg(filePath_, QString::fromStdString(error.what())));
        }
    }
    return contacts;
}

void BinaryContactRepository::save(const std::vector<domain::Contact>& contacts)
{
    const QString directory = QFileInfo(filePath_).absolutePath();
    if (!QDir().mkpath(directory)) fail(QStringLiteral("Cannot create the folder %1.").arg(directory));

    QSaveFile file(filePath_);
    if (!file.open(QIODevice::WriteOnly))
        fail(QStringLiteral("Cannot write %1: %2").arg(filePath_, file.errorString()));

    QDataStream out(&file);
    out.setVersion(kStreamVersion);
    out << kMagic << kFormatVersion << static_cast<quint32>(contacts.size());
    for (const domain::Contact& contact : contacts) {
        out << static_cast<qint64>(contact.id()) << QString::fromStdString(contact.name())
            << QString::fromStdString(contact.phone());
    }

    if (out.status() != QDataStream::Ok || !file.commit()) {
        fail(QStringLiteral("Cannot write %1: %2").arg(filePath_, file.errorString()));
    }
}

}  // namespace contacts::infrastructure
