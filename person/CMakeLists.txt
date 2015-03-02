configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.kde.person/metadata.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.person/metadata.desktop
               @ONLY)

install(DIRECTORY org.kde.person DESTINATION ${PLASMA_DATA_INSTALL_DIR}/plasmoids PATTERN metadata.desktop.cmake EXCLUDE)

install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.person/metadata.desktop DESTINATION ${PLASMA_DATA_INSTALL_DIR}/plasmoids/org.kde.person/)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.person/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-org.kde.person.desktop)
