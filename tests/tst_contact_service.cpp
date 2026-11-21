// Application tests: the use cases run against in-memory fakes of the ports,
// so they need no files and no UI. This is what the ports make possible.
#include "application/ContactService.h"
#include "application/Errors.h"
#include "domain/Errors.h"

#include <QTest>

#include <memory>
#include <string>
#include <vector>

using namespace contacts;
using application::ContactService;
using application::InMemoryContactRepository;

namespace {

/// Test double that records what it was asked to export.
class RecordingExporter final : public application::ContactExporter {
public:
    void exportTo(const std::string& filePath, const std::vector<domain::Contact>& contacts) override
    {
        lastPath = filePath;
        lastContacts = contacts;
    }
    std::string lastPath;
    std::vector<domain::Contact> lastContacts;
};

/// Test double whose storage always fails.
class BrokenRepository final : public application::ContactRepository {
public:
    std::vector<domain::Contact> load() override { throw application::StorageError("disk on fire"); }
    void save(const std::vector<domain::Contact>&) override { throw application::StorageError("disk on fire"); }
    std::string location() const override { return "nowhere"; }
};

}  // namespace

class TestContactService : public QObject {
    Q_OBJECT

private slots:
    void changesMarkTheBookUnsavedAndNotify()
    {
        ContactService service(std::make_unique<InMemoryContactRepository>(), std::make_unique<RecordingExporter>());
        int notifications = 0;
        service.subscribe([&] { ++notifications; });

        QVERIFY(!service.hasUnsavedChanges());
        const domain::Contact ada = service.add("Ada", "555 0100");
        QVERIFY(service.hasUnsavedChanges());
        service.update(ada.id(), "Ada Lovelace", "555 0100");
        service.remove(ada.id());
        QCOMPARE(notifications, 3);
    }

    void saveThenLoadRoundTrips()
    {
        auto repository = std::make_unique<InMemoryContactRepository>();
        InMemoryContactRepository* storage = repository.get();
        ContactService service(std::move(repository), std::make_unique<RecordingExporter>());

        service.add("Ada", "555 0100");
        service.save();
        QVERIFY(!service.hasUnsavedChanges());
        QCOMPARE(storage->load().size(), std::size_t(1));

        service.add("Alan", "555 0101");  // Not saved…
        service.load();                   // …so reloading discards it.
        QCOMPARE(service.contacts().size(), std::size_t(1));
        QVERIFY(!service.hasUnsavedChanges());
    }

    void updateWithoutRealChangesIsNotAModification()
    {
        ContactService service(std::make_unique<InMemoryContactRepository>(), std::make_unique<RecordingExporter>());
        const domain::Contact ada = service.add("Ada", "555 0100");
        service.save();

        service.update(ada.id(), " Ada ", "555 0100");
        QVERIFY(!service.hasUnsavedChanges());
    }

    void invalidInputIsRejectedWithoutSideEffects()
    {
        ContactService service(std::make_unique<InMemoryContactRepository>(), std::make_unique<RecordingExporter>());
        int notifications = 0;
        service.subscribe([&] { ++notifications; });

        QVERIFY_THROWS_EXCEPTION(domain::ValidationError, service.add("", "555 0100"));
        QVERIFY_THROWS_EXCEPTION(domain::NotFoundError, service.remove(99));
        QVERIFY(service.contacts().empty());
        QVERIFY(!service.hasUnsavedChanges());
        QCOMPARE(notifications, 0);
    }

    void failedLoadKeepsCurrentContacts()
    {
        ContactService service(std::make_unique<BrokenRepository>(), std::make_unique<RecordingExporter>());
        service.add("Ada", "555 0100");

        QVERIFY_THROWS_EXCEPTION(application::StorageError, service.load());
        QVERIFY_THROWS_EXCEPTION(application::StorageError, service.save());
        QCOMPARE(service.contacts().size(), std::size_t(1));
        QVERIFY(service.hasUnsavedChanges());
    }

    void exportPassesContactsToTheExporter()
    {
        auto exporter = std::make_unique<RecordingExporter>();
        RecordingExporter* recorder = exporter.get();
        ContactService service(std::make_unique<InMemoryContactRepository>(), std::move(exporter));
        service.add("Ada", "555 0100");

        service.exportTo("out.csv");
        QCOMPARE(recorder->lastPath, std::string("out.csv"));
        QCOMPARE(recorder->lastContacts, service.contacts());
    }
};

QTEST_APPLESS_MAIN(TestContactService)
#include "tst_contact_service.moc"
