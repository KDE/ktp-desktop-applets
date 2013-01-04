project(ktp_plasma)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
    ${CMAKE_MODULE_PATH}
)
set(IS_KTP_INTERNAL_MODULE TRUE)
set(KDE_MIN_VERSION "4.7.0")

find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package(TelepathyQt4 0.8.90 REQUIRED)
find_package(KTp REQUIRED)

add_subdirectory(contact-applet)

install(DIRECTORY org.kde.ktp.contactlist
        DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/)


install(FILES org.kde.ktp.contactlist/metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}
        RENAME plasma-applet-ktp-contactlist.desktop)
