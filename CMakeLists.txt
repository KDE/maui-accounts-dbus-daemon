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
        Network
)
find_package(
    libdavclient REQUIRED
)

set(
    SOURCES

    src/main.cpp
    src/EncryptionHelper.cpp
    src/dbus-interfaces/RootDBusInterface.cpp
)


set(
    HEADERS

    src/EncryptionHelper.hpp
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
    Qt5::Network

    libdavclient.so
)

install(
    TARGETS ${PROJECT_NAME}
    DESTINATION bin
)
install(
    FILES dbus-org.maui-accounts-dbus-daemon.service
    DESTINATION /etc/systemd/system/
)
install(
    FILES org.mauikit.accounts.conf
    DESTINATION /etc/dbus-1/system.d/
)

### CPack Configurations

include(InstallRequiredSystemLibraries)

set(CPACK_GENERATOR "DEB")
set(CPACK_DEBIAN_PACKAGE_SHLIBDEPS ON)
set(CPACK_DEBIAN_PACKAGE_MAINTAINER "Anupam Basak")
set(CPACK_PACKAGE_DESCRIPTION_SUMMARY "MAUI Accounts DBus Daemon")
set(CPACK_PACKAGE_VENDOR "Anupam Basak")
set(CPACK_PACKAGE_CONTACT "Anupam Basak <anupam.basak27@gmail.com>")
#set(CPACK_PACKAGE_DESCRIPTION_FILE "${CMAKE_CURRENT_SOURCE_DIR}/ReadMe.txt")
#set(CPACK_RESOURCE_FILE_LICENSE "${CMAKE_CURRENT_SOURCE_DIR}/Copyright.txt")
set(CPACK_PACKAGE_VERSION_MAJOR ${PROJECT_VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${PROJECT_VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${PROJECT_VERSION_PATCH})
set(CPACK_PACKAGE_INSTALL_DIRECTORY "CMake ${CMake_VERSION_MAJOR}.${CMake_VERSION_MINOR}")
set(CPACK_SOURCE_STRIP_FILES "")
set(CPACK_DEBIAN_PACKAGE_CONTROL_EXTRA "${CMAKE_CURRENT_SOURCE_DIR}/cpack-package-control-extra-scripts/preinst;${CMAKE_CURRENT_SOURCE_DIR}/cpack-package-control-extra-scripts/postinst;${CMAKE_CURRENT_SOURCE_DIR}/cpack-package-control-extra-scripts/prerm;${CMAKE_CURRENT_SOURCE_DIR}/cpack-package-control-extra-scripts/postrm")

include(CPack)

### END CPack Configurations

feature_summary(WHAT ALL FATAL_ON_MISSING_REQUIRED_PACKAGES)
