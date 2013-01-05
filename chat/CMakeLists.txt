install(DIRECTORY org.kde.ktp-chat
        DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/)


install(FILES org.kde.ktp-chat/metadata.desktop
        DESTINATION ${SERVICES_INSTALL_DIR}
        RENAME plasma-applet-ktp-chat.desktop)
