# One executable per test file. Each links only the layer it tests, which shows
# that the inner layers really are usable without the outer ones.
function(contacts_add_test name)
    add_executable(${name} ${name}.cpp)
    set_target_properties(${name} PROPERTIES
        AUTOMOC ON
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/tests)
    target_link_libraries(${name} PRIVATE ${ARGN} Qt6::Test)
    contacts_set_warnings(${name})
    add_test(NAME ${name} COMMAND ${name})
    # Lets the tests run on CI machines without a display.
    set_tests_properties(${name} PROPERTIES ENVIRONMENT "QT_QPA_PLATFORM=offscreen")
endfunction()

contacts_add_test(tst_contact contacts_domain)
contacts_add_test(tst_phone_book contacts_domain)
contacts_add_test(tst_contact_service contacts_application)
contacts_add_test(tst_binary_contact_repository contacts_infrastructure)
contacts_add_test(tst_csv_contact_exporter contacts_infrastructure)
