configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.kde.ktp-contactlist/metadata.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-contactlist/metadata.desktop
               @ONLY)

install(DIRECTORY org.kde.ktp-contactlist DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids PATTERN metadata.desktop.cmake EXCLUDE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-contactlist/metadata.desktop DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.ktp-contactlist)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-contactlist/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-org.kde.ktp-contactlist.desktop)

add_subdirectory(qmlplugin)