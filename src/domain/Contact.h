#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>

namespace contacts::domain {

using ContactId = std::int64_t;

/// Entity: one person in the phone book.
///
/// A Contact is *always valid*: the constructor and every setter validate their
/// input and throw ValidationError otherwise. Code that holds a Contact never has
/// to re-check it. Text is stored as UTF-8 std::string — this layer knows nothing
/// about Qt, files or screens.
class Contact {
public:
    static constexpr std::size_t kMaxNameLength = 100;  ///< In characters (Unicode code points).
    static constexpr std::size_t kMinPhoneDigits = 3;
    static constexpr std::size_t kMaxPhoneDigits = 15;  ///< The longest number E.164 allows.

    /// Throws ValidationError if `id` is not positive or `name` / `phone` break a rule.
    Contact(ContactId id, std::string_view name, std::string_view phone);

    ContactId id() const noexcept { return id_; }
    const std::string& name() const noexcept { return name_; }
    const std::string& phone() const noexcept { return phone_; }

    void rename(std::string_view name);
    void changePhone(std::string_view phone);

    /// The phone number reduced to its digits: "+1 (555) 010-9999" → "15550109999".
    /// Two contacts with the same key have the same number, however it is written.
    std::string phoneKey() const { return digitsOf(phone_); }

    /// Trims and collapses inner whitespace ("  Ada   Lovelace " → "Ada Lovelace").
    /// Throws ValidationError if the result is empty or too long.
    static std::string normalizeName(std::string_view raw);

    /// Trims, then checks the characters (digits, spaces, + - . ( ) with '+' only
    /// first) and the digit count. Throws ValidationError on failure.
    static std::string normalizePhone(std::string_view raw);

    static std::string digitsOf(std::string_view phone);

    friend bool operator==(const Contact& a, const Contact& b)
    {
        return a.id_ == b.id_ && a.name_ == b.name_ && a.phone_ == b.phone_;
    }
    friend bool operator!=(const Contact& a, const Contact& b) { return !(a == b); }

private:
    ContactId id_;
    std::string name_;
    std::string phone_;
};

}  // namespace contacts::domain
