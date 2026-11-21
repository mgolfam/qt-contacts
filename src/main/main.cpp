// Composition root: the only place that knows every concrete type. It builds the
// adapters (infrastructure), hands them to the use cases (application) through
// their ports, and gives the result to the window (ui).
//
// Want to store contacts in SQLite instead? Write a SqliteContactRepository that
// implements application::ContactRepository and change one line here — nothing
// in domain/, application/ or ui/ has to change.

#include "application/ContactService.h"
#include "infrastructure/BinaryContactRepository.h"
#include "infrastructure/CsvContactExporter.h"
#include "ui/MainWindow.h"

#include <QApplication>
#include <QCommandLineParser>
#include <QMessageBox>

#include <memory>

using namespace contacts;

int main(int argc, char* argv[])
{
    QApplication app(argc, argv);
    QApplication::setOrganizationName(QStringLiteral("Golfam"));
    QApplication::setApplicationName(QStringLiteral("qt-contacts"));
    QApplication::setApplicationDisplayName(QStringLiteral("Contacts"));
    QApplication::setApplicationVersion(QStringLiteral(CONTACTS_VERSION));

    QCommandLineParser parser;
    parser.setApplicationDescription(QStringLiteral("A small phone book that teaches Clean Architecture."));
    parser.addHelpOption();
    parser.addVersionOption();
    const QCommandLineOption fileOption({QStringLiteral("f"), QStringLiteral("file")},
                                        QStringLiteral("Use <path> as the contacts file."), QStringLiteral("path"),
                                        infrastructure::BinaryContactRepository::defaultFilePath());
    parser.addOption(fileOption);
    parser.process(app);

    application::ContactService service(
        std::make_unique<infrastructure::BinaryContactRepository>(parser.value(fileOption)),
        std::make_unique<infrastructure::CsvContactExporter>());

    ui::MainWindow window(service);
    window.show();

    try {
        service.load();
    } catch (const std::exception& error) {
        QMessageBox::warning(&window, QStringLiteral("Could Not Load Contacts"),
                             QString::fromStdString(error.what()) +
                                 QStringLiteral("\n\nStarting with an empty list. Saving will replace the file."));
    }

    return QApplication::exec();
}
