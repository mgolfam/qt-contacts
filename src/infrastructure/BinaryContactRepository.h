#pragma once

#include "application/ContactRepository.h"

#include <QString>
#include <QtGlobal>

namespace contacts::infrastructure {

/// Adapter: stores the phone book in a small binary file using QDataStream.
///
/// File layout (QDataStream writes big-endian, so the file is portable):
///
///   quint32  magic     0xAA55AA55 — "this is a contacts file"
///   quint16  version   kFormatVersion — lets future versions change the layout
///   quint32  count
///   count ×  { qint64 id; QString name; QString phone }
///
/// Saving goes through QSaveFile: the data is written to a temporary file and
/// renamed over the old one only when complete, so a crash never leaves a
/// half-written phone book behind.
class BinaryContactRepository final : public application::ContactRepository {
public:
    static constexpr quint32 kMagic = 0xAA55AA55;
    static constexpr quint16 kFormatVersion = 1;

    explicit BinaryContactRepository(QString filePath);

    /// e.g. ~/.local/share/<Organization>/<Application>/contacts.dat on Linux.
    static QString defaultFilePath();

    std::vector<domain::Contact> load() override;
    void save(const std::vector<domain::Contact>& contacts) override;
    std::string location() const override { return filePath_.toStdString(); }

private:
    QString filePath_;
};

}  // namespace contacts::infrastructure
