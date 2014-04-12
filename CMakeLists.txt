project(ktp-desktop-applets)

cmake_minimum_required(VERSION 2.8.12 FATAL_ERROR)

find_package(ECM 0.0.11 REQUIRED NO_MODULE)
set (CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${ECM_KDE_MODULE_DIR})

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings)

add_subdirectory(chat)
# add_subdirectory(contact)
add_subdirectory(contactlist)
# add_subdirectory(presence)

install(FILES 50-renameKTpApplets.js DESTINATION ${DATA_INSTALL_DIR}/plasma-desktop/updates)
