# Adapters that implement the application's ports with real I/O.
add_library(contacts_infrastructure STATIC
    BinaryContactRepository.h BinaryContactRepository.cpp
    CsvContactExporter.h CsvContactExporter.cpp
)

target_include_directories(contacts_infrastructure PUBLIC ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(contacts_infrastructure PUBLIC contacts_application Qt6::Core)
contacts_set_warnings(contacts_infrastructure)
