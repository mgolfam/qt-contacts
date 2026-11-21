#pragma once

#include "application/ContactExporter.h"
#include "application/ContactRepository.h"
#include "domain/PhoneBook.h"

#include <functional>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace contacts::application {

/// The application's use cases — everything a user can do with their contacts.
///
/// The UI talks only to this class. It never touches PhoneBook, files or CSV
/// directly, so the same service could sit behind a CLI, a web API or a test.
///
/// Errors are reported with exceptions:
///   domain::ValidationError / domain::NotFoundError  — the request broke a rule
///   application::StorageError                         — reading or writing failed
class ContactService {
public:
    using ChangeListener = std::function<void()>;

    ContactService(std::unique_ptr<ContactRepository> repository, std::unique_ptr<ContactExporter> exporter);

    // ---- Queries (never change state) --------------------------------------
    const std::vector<domain::Contact>& contacts() const noexcept { return book_.contacts(); }
    std::optional<domain::Contact> find(domain::ContactId id) const;
    bool hasUnsavedChanges() const noexcept { return dirty_; }
    std::string storageLocation() const { return repository_->location(); }

    // ---- Commands ----------------------------------------------------------
    domain::Contact add(std::string_view name, std::string_view phone);
    domain::Contact update(domain::ContactId id, std::string_view name, std::string_view phone);
    void remove(domain::ContactId id);

    /// Replaces the in-memory contacts with the stored ones, discarding unsaved
    /// changes. If loading fails, the current contacts are kept.
    void load();
    void save();
    void exportTo(const std::string& filePath) const;

    /// Registers a callback that runs after every change to the contacts or to
    /// hasUnsavedChanges(). This is how the UI stays in sync (Observer pattern)
    /// without the application layer knowing anything about the UI.
    void subscribe(ChangeListener listener);

private:
    /// Updates the unsaved-changes flag and tells every listener.
    void notify(bool unsavedChanges);

    std::unique_ptr<ContactRepository> repository_;
    std::unique_ptr<ContactExporter> exporter_;
    domain::PhoneBook book_;
    bool dirty_ = false;
    std::vector<ChangeListener> listeners_;
};

}  // namespace contacts::application
