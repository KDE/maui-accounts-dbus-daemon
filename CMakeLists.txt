project(maui-accounts-dbus-daemon)

cmake_minimum_required(VERSION 3.10)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FeatureSummary)

find_package(ECM 1.7.0 REQUIRED NO_MODULE)
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake" ${ECM_MODULE_PATH})

find_package(
    Qt5 REQUIRED NO_MODULE

    COMPONENTS
        Core
        DBus
)
find_package(
    KF5 REQUIRED

    COMPONENTS
        Wallet
)

set(
    SOURCES

    src/main.cpp
    src/dbus-interfaces/RootDBusInterface.cpp
)


set(
    HEADERS

    src/dbus-interfaces/DBusInterface.hpp
    src/dbus-interfaces/RootDBusInterface.hpp
)

add_executable(
    ${PROJECT_NAME}

    ${SOURCES}
    ${HEADERS}
)
target_link_libraries(
    ${PROJECT_NAME}

    Qt5::Core
    Qt5::DBus
    KF5::Wallet
)

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
