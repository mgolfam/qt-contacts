#pragma once

#include <QString>

#include <string>

namespace contacts::ui {

// The inner layers use UTF-8 std::string; Qt widgets use QString.
// The UI layer is the boundary, so the conversion lives here.
inline QString toQt(const std::string& text)
{
    return QString::fromStdString(text);
}
inline std::string toStd(const QString& text)
{
    return text.toStdString();
}

}  // namespace contacts::ui
