configure_file(${CMAKE_CURRENT_SOURCE_DIR}/org.kde.ktp-contactlist/metadata.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-contactlist/metadata.desktop
               @ONLY)

install(DIRECTORY org.kde.ktp-contactlist DESTINATION ${KDE_INSTALL_DATADIR}/plasma/plasmoids PATTERN metadata.desktop.cmake EXCLUDE)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-contactlist/metadata.desktop DESTINATION ${KDE_INSTALL_DATADIR}/plasma/plasmoids/org.kde.ktp-contactlist)
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/org.kde.ktp-contactlist/metadata.desktop DESTINATION ${KDE_INSTALL_KSERVICES5DIR} RENAME plasma-applet-org.kde.ktp-contactlist.desktop)

add_subdirectory(qmlplugin)