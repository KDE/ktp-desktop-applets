configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.kde.ktp-chat/metadata.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-chat/metadata.desktop
               @ONLY)

install(DIRECTORY org.kde.ktp-chat DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/ PATTERN metadata.desktop.cmake EXCLUDE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-chat/metadata.desktop DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.ktp-chat/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-chat/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-ktp-chat.desktop)

