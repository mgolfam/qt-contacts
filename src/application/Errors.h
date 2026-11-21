#pragma once

#include <stdexcept>

namespace contacts::application {

/// Thrown by repository and exporter adapters when reading or writing fails
/// (missing permissions, disk full, corrupt file, …). The message is user-facing.
class StorageError : public std::runtime_error {
public:
    using std::runtime_error::runtime_error;
};

}  // namespace contacts::application
