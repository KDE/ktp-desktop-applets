project(plasma_applet_telepathy_contact)

cmake_minimum_required(VERSION 2.8)

find_package(KDE4 REQUIRED)
find_package(KDE4Workspace REQUIRED)

include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS} ${KDE4_DEFINITIONS})
include_directories(${CMAKE_SOURCE_DIR} ${CMAKE_BINARY_DIR} ${KDE4_INCLUDES})

set(telepathy_contact_applet_SRCS
    src/telepathyContact.cpp
    src/config.cpp
)

kde4_add_ui_files(telepathy_contact_applet_SRCS
                    src/config.ui
)

kde4_add_plugin(plasma_applet_telepathy_contact ${telepathy_contact_applet_SRCS})
target_link_libraries(plasma_applet_telepathy_contact ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS})

install(TARGETS plasma_applet_telepathy_contact DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES telepathy-contact-applet.desktop DESTINATION ${SERVICES_INSTALL_DIR})