#include "domain/PhoneBook.h"

#include "domain/Errors.h"

#include <algorithm>
#include <string>
#include <utility>

namespace contacts::domain {

namespace {

constexpr ContactId kNoContact = 0;  // Ids start at 1, so 0 never matches a real contact.

std::string notFound(ContactId id)
{
    return "There is no contact with id " + std::to_string(id) + ".";
}

}  // namespace

PhoneBook::PhoneBook(std::vector<Contact> contacts)
{
    contacts_.reserve(contacts.size());
    for (Contact& contact : contacts) {
        if (find(contact.id()) != nullptr) {
            throw ValidationError("Contact id " + std::to_string(contact.id()) + " appears twice.");
        }
        ensurePhoneIsFree(contact, kNoContact);
        nextId_ = std::max(nextId_, contact.id() + 1);
        contacts_.push_back(std::move(contact));
    }
}

Contact PhoneBook::add(std::string_view name, std::string_view phone)
{
    Contact contact(nextId_, name, phone);  // validates name and phone
    ensurePhoneIsFree(contact, kNoContact);

    contacts_.push_back(contact);
    ++nextId_;
    return contact;
}

Contact PhoneBook::update(ContactId id, std::string_view name, std::string_view phone)
{
    const auto it = std::find_if(contacts_.begin(), contacts_.end(), [id](const Contact& c) { return c.id() == id; });
    if (it == contacts_.end()) throw NotFoundError(notFound(id));

    // Build the new version on the side first; only commit once every check passed.
    Contact updated(id, name, phone);
    ensurePhoneIsFree(updated, id);

    *it = updated;
    return updated;
}

void PhoneBook::remove(ContactId id)
{
    const auto it = std::find_if(contacts_.begin(), contacts_.end(), [id](const Contact& c) { return c.id() == id; });
    if (it == contacts_.end()) throw NotFoundError(notFound(id));
    contacts_.erase(it);
}

const Contact* PhoneBook::find(ContactId id) const noexcept
{
    const auto it = std::find_if(contacts_.begin(), contacts_.end(), [id](const Contact& c) { return c.id() == id; });
    return it == contacts_.end() ? nullptr : &*it;
}

void PhoneBook::ensurePhoneIsFree(const Contact& candidate, ContactId self) const
{
    const std::string key = candidate.phoneKey();
    for (const Contact& other : contacts_) {
        if (other.id() != self && other.phoneKey() == key) {
            throw ValidationError("This phone number already belongs to \"" + other.name() + "\".");
        }
    }
}

}  // namespace contacts::domain
