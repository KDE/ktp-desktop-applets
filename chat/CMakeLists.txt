configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.kde.ktp-chat/metadata.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-chat/metadata.desktop
               @ONLY)

install(DIRECTORY org.kde.ktp-chat DESTINATION ${PLASMA_DATA_INSTALL_DIR}/plasmoids PATTERN metadata.desktop.cmake EXCLUDE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-chat/metadata.desktop DESTINATION ${PLASMA_DATA_INSTALL_DIR}/plasmoids/org.kde.ktp-chat/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-chat/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-org.kde.ktp-chat.desktop)

add_subdirectory(qmlplugin)
