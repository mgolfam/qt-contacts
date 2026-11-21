# Layers, from the inside out. Each layer may only depend on the ones listed
# above it — the "dependency rule" of Clean Architecture. CMake enforces it:
# a layer that is not linked simply cannot be #included.
#
#   domain          entities and business rules: Contact, PhoneBook      (pure C++17, no Qt)
#   application     use cases + ports: ContactService, ContactRepository (pure C++17, no Qt)
#   infrastructure  adapters for the ports: binary file, CSV export       (Qt Core)
#   ui              Qt Widgets presentation: table, dialog, main window   (Qt Widgets)
#   main            composition root: builds the adapters and wires them up
add_subdirectory(domain)
add_subdirectory(application)
add_subdirectory(infrastructure)
add_subdirectory(ui)
add_subdirectory(main)
