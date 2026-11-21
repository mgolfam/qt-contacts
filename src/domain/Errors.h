#pragma once

#include <stdexcept>

namespace contacts::domain {

/// Base class for every business rule the domain enforces.
class DomainError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

/// The input breaks a rule, e.g. an empty name or a phone number that is already taken.
/// Messages are written for end users, so the UI can show them as they are.
class ValidationError : public DomainError {
public:
    using DomainError::DomainError;
};

/// An operation referred to a contact id that does not exist.
class NotFoundError : public DomainError {
public:
    using DomainError::DomainError;
};

}  // namespace contacts::domain
