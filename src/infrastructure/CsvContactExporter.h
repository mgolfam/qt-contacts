#pragma once

#include "application/ContactExporter.h"

#include <QString>

namespace contacts::infrastructure {

/// Adapter: exports contacts as CSV (RFC 4180) that spreadsheets can open.
/// UTF-8 encoded, CRLF line endings, a header row, fields quoted only when needed.
class CsvContactExporter final : public application::ContactExporter {
public:
    void exportTo(const std::string& filePath, const std::vector<domain::Contact>& contacts) override;

    /// The CSV text itself. Kept separate from the file I/O so it is easy to test.
    static QString toCsv(const std::vector<domain::Contact>& contacts);

    /// Quotes a field if it contains a comma, quote, line break or edge spaces;
    /// quotes inside are doubled: He said "hi" → "He said ""hi""".
    static QString escapeField(const QString& field);
};

}  // namespace contacts::infrastructure
