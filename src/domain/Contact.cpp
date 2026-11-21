#include "domain/Contact.h"

#include "domain/Errors.h"

namespace contacts::domain {

namespace {

bool isSpace(char c)
{
    return c == ' ' || c == '\t' || c == '\n' || c == '\r' || c == '\f' || c == '\v';
}

bool isDigit(char c)
{
    return c >= '0' && c <= '9';
}

bool isPhoneSeparator(char c)
{
    return c == ' ' || c == '-' || c == '.' || c == '(' || c == ')';
}

std::string_view trim(std::string_view text)
{
    while (!text.empty() && isSpace(text.front())) text.remove_prefix(1);
    while (!text.empty() && isSpace(text.back())) text.remove_suffix(1);
    return text;
}

/// UTF-8 continuation bytes look like 10xxxxxx; every other byte starts a character.
std::size_t countCharacters(std::string_view utf8)
{
    std::size_t count = 0;
    for (const char c : utf8) {
        if ((static_cast<unsigned char>(c) & 0xC0) != 0x80) ++count;
    }
    return count;
}

}  // namespace

Contact::Contact(ContactId id, std::string_view name, std::string_view phone)
    : id_(id), name_(normalizeName(name)), phone_(normalizePhone(phone))
{
    if (id_ <= 0) throw ValidationError("A contact id must be a positive number.");
}

void Contact::rename(std::string_view name)
{
    name_ = normalizeName(name);
}

void Contact::changePhone(std::string_view phone)
{
    phone_ = normalizePhone(phone);
}

std::string Contact::normalizeName(std::string_view raw)
{
    std::string name;
    bool pendingSpace = false;
    for (const char c : trim(raw)) {
        if (isSpace(c)) {
            pendingSpace = true;
            continue;
        }
        if (pendingSpace) name += ' ';
        pendingSpace = false;
        name += c;
    }

    if (name.empty()) throw ValidationError("Name is required.");
    if (countCharacters(name) > kMaxNameLength) {
        throw ValidationError("Name is too long (at most " + std::to_string(kMaxNameLength) + " characters).");
    }
    return name;
}

std::string Contact::normalizePhone(std::string_view raw)
{
    const std::string_view phone = trim(raw);
    if (phone.empty()) throw ValidationError("Phone number is required.");

    std::size_t digits = 0;
    for (std::size_t i = 0; i < phone.size(); ++i) {
        const char c = phone[i];
        if (isDigit(c)) {
            ++digits;
        } else if (c == '+') {
            if (i != 0) throw ValidationError("'+' is only allowed at the start of a phone number.");
        } else if (!isPhoneSeparator(c)) {
            throw ValidationError("A phone number may only contain digits, spaces and + - . ( )");
        }
    }

    if (digits < kMinPhoneDigits || digits > kMaxPhoneDigits) {
        throw ValidationError("A phone number needs between " + std::to_string(kMinPhoneDigits) + " and " +
                              std::to_string(kMaxPhoneDigits) + " digits.");
    }
    return std::string(phone);
}

std::string Contact::digitsOf(std::string_view phone)
{
    std::string digits;
    for (const char c : phone) {
        if (isDigit(c)) digits += c;
    }
    return digits;
}

}  // namespace contacts::domain
