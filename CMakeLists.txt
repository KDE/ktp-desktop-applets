project(plasma_applet_telepathy_contact_list)

cmake_minimum_required(VERSION 2.8)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
    ${CMAKE_MODULE_PATH}
)
set(IS_TELEPATHY_KDE_INTERNAL_MODULE TRUE)
set(KDE_MIN_VERSION "4.7.0")

find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)
find_package(KDE4Workspace REQUIRED)
find_package(TelepathyQt4 0.7.3 REQUIRED)
find_package (KTelepathy REQUIRED)


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

set(telepathy_contact_list_applet_SRCS
    src/telepathyContactList.cpp
    src/flat-model-proxy.cpp
)

kde4_add_plugin(plasma_applet_telepathy_contact_list ${telepathy_contact_list_applet_SRCS})

target_link_libraries(plasma_applet_telepathy_contact_list
                        ${KDE4_PLASMA_LIBS}
                        ${KDE4_KDEUI_LIBS}
                        ${TELEPATHY_QT4_LIBRARIES}
                        ${QT_QTDECLARATIVE_LIBRARY}
                        ${KTELEPATHY_MODELS_LIBRARIES})

install(DIRECTORY src/declarative/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.telepathy-contact-list)
install(TARGETS plasma_applet_telepathy_contact_list DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES telepathy-contact-list-applet.desktop DESTINATION ${SERVICES_INSTALL_DIR})
