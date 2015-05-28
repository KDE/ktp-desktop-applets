project(ktp-desktop-applets)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

set(KTP_DESKTOP_APPLETS_VERSION "15.04.2")

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

find_package(Qt5 REQUIRED COMPONENTS Qml)
find_package(KF5 REQUIRED COMPONENTS Plasma WindowSystem)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

add_subdirectory(chat)
add_subdirectory(contactlist)
add_subdirectory(person)
