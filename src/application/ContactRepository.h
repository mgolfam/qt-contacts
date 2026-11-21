#pragma once

#include "domain/Contact.h"

#include <string>
#include <utility>
#include <vector>

namespace contacts::application {

/// Port: where the phone book is kept between runs.
///
/// The application layer owns this interface and depends only on it. Concrete
/// storage (a binary file, SQLite, a web API, …) lives in the infrastructure
/// layer and is plugged in by main.cpp — "dependency inversion".
class ContactRepository {
public:
    virtual ~ContactRepository() = default;

    /// Returns every stored contact; an empty list if nothing was saved yet.
    /// Throws StorageError if the data cannot be read.
    virtual std::vector<domain::Contact> load() = 0;

    /// Replaces the stored contacts. Throws StorageError on failure.
    virtual void save(const std::vector<domain::Contact>& contacts) = 0;

    /// A human-readable description of where the data lives, e.g. a file path.
    virtual std::string location() const = 0;
};

/// Adapter that keeps contacts in memory only. Handy in tests and demos.
class InMemoryContactRepository final : public ContactRepository {
public:
    InMemoryContactRepository() = default;
    explicit InMemoryContactRepository(std::vector<domain::Contact> initial) : contacts_(std::move(initial)) {}

    std::vector<domain::Contact> load() override { return contacts_; }
    void save(const std::vector<domain::Contact>& contacts) override { contacts_ = contacts; }
    std::string location() const override { return "memory"; }

private:
    std::vector<domain::Contact> contacts_;
};

}  // namespace contacts::application
