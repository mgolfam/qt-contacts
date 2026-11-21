add_executable(qt-contacts WIN32 MACOSX_BUNDLE
    main.cpp
)

set_target_properties(qt-contacts PROPERTIES
    MACOSX_BUNDLE_BUNDLE_NAME "Contacts"
    MACOSX_BUNDLE_GUI_IDENTIFIER "com.golfam.qtcontacts"
    MACOSX_BUNDLE_BUNDLE_VERSION ${PROJECT_VERSION}
    MACOSX_BUNDLE_SHORT_VERSION_STRING ${PROJECT_VERSION}
)

# The only target that sees every layer: it picks the concrete adapters.
target_link_libraries(qt-contacts PRIVATE contacts_ui contacts_infrastructure)
target_compile_definitions(qt-contacts PRIVATE CONTACTS_VERSION="${PROJECT_VERSION}")
contacts_set_warnings(qt-contacts)

install(TARGETS qt-contacts
    BUNDLE DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})
