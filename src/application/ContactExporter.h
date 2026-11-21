#pragma once

#include "domain/Contact.h"

#include <string>
#include <vector>

namespace contacts::application {

/// Port: writes contacts to a file another program can open (CSV, vCard, …).
class ContactExporter {
public:
    virtual ~ContactExporter() = default;

    /// `filePath` is UTF-8. Throws StorageError on failure.
    virtual void exportTo(const std::string& filePath, const std::vector<domain::Contact>& contacts) = 0;
};

}  // namespace contacts::application
