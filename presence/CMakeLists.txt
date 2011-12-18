project(telepathy-kde-presence-applet)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
    ${CMAKE_MODULE_PATH}
)

set(KDE_MIN_VERSION "4.6.0")

set(IS_TELEPATHY_KDE_INTERNAL_MODULE TRUE)

find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package(TelepathyQt4 0.8.90 REQUIRED)
find_package(KTelepathy REQUIRED)

include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS}
                ${KDE4_DEFINITIONS}
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${KDE4_INCLUDES}
                    ${TELEPATHY_QT4_INCLUDE_DIR}
                    ${KTELEPATHY_INCLUDE_DIR}
)


set(telepathy-kde-presence-applet_SRCS
    src/generalsettings.cpp
    src/presenceapplet.cpp
)

kde4_add_ui_files(telepathy-kde-presence-applet_SRCS
                    src/generalsettings.ui
)

kde4_add_plugin(telepathy-kde-presence-applet ${telepathy-kde-presence-applet_SRCS})
target_link_libraries(telepathy-kde-presence-applet
                        ${KDE4_PLASMA_LIBS}
                        ${KDE4_KDEUI_LIBS}
                        ${TELEPATHY_QT4_LIBRARIES}
                        ${KTELEPATHY_LIBRARIES}
                     )

install(TARGETS telepathy-kde-presence-applet DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES telepathy-kde-presence-applet.desktop DESTINATION ${SERVICES_INSTALL_DIR})
