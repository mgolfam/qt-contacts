# Innermost layer: no Qt, no I/O — only the standard library.
add_library(contacts_domain STATIC
    Contact.h Contact.cpp
    Errors.h
    PhoneBook.h PhoneBook.cpp
)

target_include_directories(contacts_domain PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_compile_features(contacts_domain PUBLIC cxx_std_17)
contacts_set_warnings(contacts_domain)
