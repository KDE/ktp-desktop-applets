project(ktp-contact-applet)

set(CMAKE_MODULE_PATH
    "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules"
    ${CMAKE_MODULE_PATH}
)

set(KDE_MIN_VERSION "4.6.0")

set(IS_KTP_INTERNAL_MODULE TRUE)
find_package(KDE4 ${KDE_MIN_VERSION} REQUIRED)
#find_package(KDE4Workspace REQUIRED)
find_package(TelepathyQt4 0.8.9 REQUIRED)
find_package(KTp REQUIRED)

include(KDE4Defaults)

add_definitions(${QT_DEFINITIONS}
                ${KDE4_DEFINITIONS}
)

include_directories(${CMAKE_SOURCE_DIR}
                    ${CMAKE_BINARY_DIR}
                    ${KDE4_INCLUDES}
                    ${TELEPATHY_QT4_INCLUDE_DIR}
                    ${KTP_INCLUDE_DIR}
)


set(plasma_applet_ktp_contact_SRCS
    src/abstract-contact-delegate.cpp
    src/config.cpp
    src/contact-delegate.cpp
    src/contact-wrapper.cpp
    src/telepathy-contact.cpp
)

kde4_add_ui_files(plasma_applet_ktp_contact_SRCS
                    src/config.ui
)

kde4_add_plugin(plasma_applet_ktp_contact ${plasma_applet_ktp_contact_SRCS})
target_link_libraries(plasma_applet_ktp_contact
                        ${KDE4_PLASMA_LIBS}
                        ${KDE4_KDEUI_LIBS}
                        ${TELEPATHY_QT4_LIBRARIES}
                        ${KTP_LIBRARIES}
                        ${KTP_MODELS_LIBRARIES}
                        ${QT_QTDECLARATIVE_LIBRARY})

install(DIRECTORY src/declarative/ DESTINATION ${DATA_INSTALL_DIR}/plasma/plasmoids/org.kde.ktp-contact)
install(TARGETS plasma_applet_ktp_contact DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma_applet_ktp_contact.desktop DESTINATION ${SERVICES_INSTALL_DIR})
