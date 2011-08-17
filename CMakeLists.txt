project(plasma_applet_telepathy_contact)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
    ${CMAKE_MODULE_PATH}
)

set(KDE_MIN_VERSION "4.7.0")

find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package(KDE4Workspace REQUIRED)
find_package(TelepathyQt4 0.7.1 REQUIRED)

include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS}
                ${KDE4_DEFINITIONS}
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${KDE4_INCLUDES}
                    ${TELEPATHY_QT4_INCLUDE_DIR}
)

set(telepathy_contact_applet_SRCS
    src/telepathyContact.cpp
    src/config.cpp
)

kde4_add_ui_files(telepathy_contact_applet_SRCS
                    src/config.ui
)

kde4_add_plugin(plasma_applet_telepathy_contact ${telepathy_contact_applet_SRCS})
target_link_libraries(plasma_applet_telepathy_contact ${KDE4_PLASMA_LIBS} ${KDE4_KDEUI_LIBS} ktelepathy)

install(TARGETS plasma_applet_telepathy_contact DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES telepathy-contact-applet.desktop DESTINATION ${SERVICES_INSTALL_DIR})