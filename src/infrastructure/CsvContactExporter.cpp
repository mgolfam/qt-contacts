#include "infrastructure/CsvContactExporter.h"

#include "application/Errors.h"

#include <QSaveFile>

namespace contacts::infrastructure {

namespace {

const QString kLineEnd = QStringLiteral("\r\n");

}  // namespace

void CsvContactExporter::exportTo(const std::string& filePath, const std::vector<domain::Contact>& contacts)
{
    const QString path = QString::fromStdString(filePath);
    QSaveFile file(path);
    if (!file.open(QIODevice::WriteOnly)) {
        throw application::StorageError(
            QStringLiteral("Cannot write %1: %2").arg(path, file.errorString()).toStdString());
    }

    file.write(toCsv(contacts).toUtf8());
    if (!file.commit()) {
        throw application::StorageError(
            QStringLiteral("Cannot write %1: %2").arg(path, file.errorString()).toStdString());
    }
}

QString CsvContactExporter::toCsv(const std::vector<domain::Contact>& contacts)
{
    QString csv = QStringLiteral("id,name,phone") + kLineEnd;
    for (const domain::Contact& contact : contacts) {
        csv += QString::number(contact.id()) + u',' + escapeField(QString::fromStdString(contact.name())) + u',' +
               escapeField(QString::fromStdString(contact.phone())) + kLineEnd;
    }
    return csv;
}

QString CsvContactExporter::escapeField(const QString& field)
{
    const bool needsQuotes = field.contains(u',') || field.contains(u'"') || field.contains(u'\n') ||
                             field.contains(u'\r') || field.startsWith(u' ') || field.endsWith(u' ');
    if (!needsQuotes) return field;

    QString quoted = field;
    quoted.replace(u'"', QStringLiteral("\"\""));
    return u'"' + quoted + u'"';
}

}  // namespace contacts::infrastructure
