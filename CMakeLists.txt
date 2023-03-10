cmake_minimum_required(VERSION 3.16)

# KDE Application Version, managed by release script
set (RELEASE_SERVICE_VERSION_MAJOR "23")
set (RELEASE_SERVICE_VERSION_MINOR "03")
set (RELEASE_SERVICE_VERSION_MICRO "80")

set(KTP_DESKTOP_APPLETS_VERSION "${RELEASE_SERVICE_VERSION_MAJOR}.${RELEASE_SERVICE_VERSION_MINOR}.${RELEASE_SERVICE_VERSION_MICRO}")

project(ktp-desktop-applets VERSION ${KTP_DESKTOP_APPLETS_VERSION})

find_package(ECM 1.6.0 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH})

find_package(Qt5 REQUIRED COMPONENTS Qml DBus)
find_package(KF5 REQUIRED COMPONENTS I18n Plasma WindowSystem)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)

add_subdirectory(chat)
add_subdirectory(contactlist)
add_subdirectory(person)

ki18n_install(po)
