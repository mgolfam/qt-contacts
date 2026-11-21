cmake_minimum_required(VERSION 3.16)

project(QtContacts
    VERSION 1.0.0
    DESCRIPTION "A small Qt 6 phone book that teaches Clean Architecture"
    LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    set(CMAKE_BUILD_TYPE Release CACHE STRING "Build type" FORCE)
endif()

option(CONTACTS_BUILD_TESTS "Build the unit tests" ON)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

include(GNUInstallDirs)
include(cmake/CompilerWarnings.cmake)

find_package(Qt6 6.2 REQUIRED COMPONENTS Core Gui Widgets)

add_subdirectory(src)

if(CONTACTS_BUILD_TESTS AND Qt6_VERSION VERSION_LESS 6.3)
    message(WARNING "Qt ${Qt6_VERSION} found: the tests need Qt 6.3+ and are skipped.")
elseif(CONTACTS_BUILD_TESTS)
    enable_testing()
    find_package(Qt6 REQUIRED COMPONENTS Test)
    add_subdirectory(tests)
endif()
