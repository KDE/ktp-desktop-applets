project(plasma-applet-presence)

cmake_minimum_required(VERSION 2.8)

find_package(KDE4 REQUIRED)

include(KDE4Defaults)

add_definitions(${KDE4_DEFINITIONS})

include_directories(
    ${KDE4_INCLUDES}
)

install(DIRECTORY package/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.telepathy-presence PATTERN .git EXCLUDE PATTERN CMakeLists.txt EXCLUDE)

install(FILES package/metadata.desktop DESTINATION ${SERVICES_INSTALL_DIR} RENAME plasma-applet-telepathy-presence.desktop)

