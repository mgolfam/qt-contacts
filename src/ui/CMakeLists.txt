# Presentation. Talks to the application layer only through ContactService.
add_library(contacts_ui STATIC
    ContactDialog.h ContactDialog.cpp
    ContactTableModel.h ContactTableModel.cpp
    MainWindow.h MainWindow.cpp
    Text.h
)

set_target_properties(contacts_ui PROPERTIES AUTOMOC ON)
target_include_directories(contacts_ui PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(contacts_ui PUBLIC contacts_application Qt6::Widgets)
contacts_set_warnings(contacts_ui)
