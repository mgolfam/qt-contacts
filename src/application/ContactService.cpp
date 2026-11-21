#include "application/ContactService.h"

#include <utility>

namespace contacts::application {

ContactService::ContactService(std::unique_ptr<ContactRepository> repository, std::unique_ptr<ContactExporter> exporter)
    : repository_(std::move(repository)), exporter_(std::move(exporter))
{
}

std::optional<domain::Contact> ContactService::find(domain::ContactId id) const
{
    const domain::Contact* contact = book_.find(id);
    return contact ? std::optional<domain::Contact>(*contact) : std::nullopt;
}

domain::Contact ContactService::add(std::string_view name, std::string_view phone)
{
    domain::Contact contact = book_.add(name, phone);
    notify(true);
    return contact;
}

domain::Contact ContactService::update(domain::ContactId id, std::string_view name, std::string_view phone)
{
    const domain::Contact* current = book_.find(id);
    if (current && current->name() == domain::Contact::normalizeName(name) &&
        current->phone() == domain::Contact::normalizePhone(phone)) {
        return *current;  // Nothing changed, so the phone book is not "modified".
    }

    domain::Contact contact = book_.update(id, name, phone);
    notify(true);
    return contact;
}

void ContactService::remove(domain::ContactId id)
{
    book_.remove(id);
    notify(true);
}

void ContactService::load()
{
    // Build the new phone book completely before touching the current one: if the
    // repository throws, or the stored data breaks a rule, nothing is lost.
    domain::PhoneBook loaded(repository_->load());
    book_ = std::move(loaded);
    notify(false);
}

void ContactService::save()
{
    repository_->save(book_.contacts());
    notify(false);
}

void ContactService::exportTo(const std::string& filePath) const
{
    exporter_->exportTo(filePath, book_.contacts());
}

void ContactService::subscribe(ChangeListener listener)
{
    listeners_.push_back(std::move(listener));
}

void ContactService::notify(bool unsavedChanges)
{
    dirty_ = unsavedChanges;
    for (const ChangeListener& listener : listeners_) listener();
}

}  // namespace contacts::application
