install(DIRECTORY org.kde.ktp-contactlist
        DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/)

install(FILES org.kde.ktp-contactlist/metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}
        RENAME plasma-applet-ktp-contactlist.desktop)
