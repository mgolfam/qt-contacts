# Use cases and the ports (interfaces) they need. Still no Qt.
add_library(contacts_application STATIC
    ContactExporter.h
    ContactRepository.h
    ContactService.h ContactService.cpp
    Errors.h
)

target_include_directories(contacts_application PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(contacts_application PUBLIC contacts_domain)
contacts_set_warnings(contacts_application)
