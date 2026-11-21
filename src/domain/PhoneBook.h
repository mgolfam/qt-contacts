#pragma once

#include "domain/Contact.h"

#include <cstddef>
#include <string_view>
#include <vector>

namespace contacts::domain {

/// Aggregate: the whole collection of contacts and the rules that span more than
/// one contact, which a single Contact cannot check on its own:
///
///   * every contact has a unique id, handed out by the phone book;
///   * no two contacts share a phone number (compared by digits only).
///
/// All changes go through this class, so these rules cannot be bypassed.
/// Every method offers the strong exception guarantee: if it throws, nothing changed.
class PhoneBook {
public:
    PhoneBook() = default;

    /// Rebuilds a phone book from stored contacts, e.g. ones loaded from disk.
    /// Throws ValidationError if ids or phone numbers repeat.
    explicit PhoneBook(std::vector<Contact> contacts);

    /// Creates a contact with the next free id and returns a copy of it.
    Contact add(std::string_view name, std::string_view phone);

    /// Replaces the name and phone of contact `id` and returns the updated copy.
    /// Throws NotFoundError or ValidationError.
    Contact update(ContactId id, std::string_view name, std::string_view phone);

    /// Throws NotFoundError if there is no such contact.
    void remove(ContactId id);

    /// Returns nullptr if there is no such contact. The pointer is invalidated by the next change.
    const Contact* find(ContactId id) const noexcept;

    /// Contacts in the order they were added.
    const std::vector<Contact>& contacts() const noexcept { return contacts_; }
    std::size_t size() const noexcept { return contacts_.size(); }
    bool empty() const noexcept { return contacts_.empty(); }

private:
    /// Throws ValidationError if a contact other than `self` already has this number.
    void ensurePhoneIsFree(const Contact& candidate, ContactId self) const;

    std::vector<Contact> contacts_;
    ContactId nextId_ = 1;
};

}  // namespace contacts::domain
