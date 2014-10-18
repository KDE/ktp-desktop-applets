project(ktp-desktop-applets)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
    ${CMAKE_MODULE_PATH}
)
cmake_policy(SET CMP0002 OLD)

set(IS_KTP_INTERNAL_MODULE TRUE)
set(KDE_MIN_VERSION "4.7.0")
set(KTP_DESKTOP_APPLETS_VERSION "0.9.0")

find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package(TelepathyQt4 0.8.90 REQUIRED)
find_package(KTp REQUIRED)

include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS}
                ${KDE4_DEFINITIONS}
)

include_directories(${CMAKE_SOURCE_DIR}
		    ${CMAKE_BINARY_DIR}
		    ${KDE4_INCLUDES}
		    ${TELEPATHY_QT4_INCLUDE_DIR}
		    ${KTP_INCLUDE_DIR}
)

add_subdirectory(chat)
add_subdirectory(contact)
add_subdirectory(contactlist)
add_subdirectory(presence)

install(FILES 50-renameKTpApplets.js DESTINATION ${DATA_INSTALL_DIR}/plasma-desktop/updates)
