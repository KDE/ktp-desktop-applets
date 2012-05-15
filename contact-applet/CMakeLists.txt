include_directories(
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${CMAKE_CURRENT_BINARY_DIR}
        ${KDE4_INCLUDES}
        ${TELEPATHY_QT4_INCLUDE_DIR}
)


set(plasma_applet_ktp_contact_SRCS
    src/abstract-contact-delegate.cpp
    src/applet_config.cpp
    src/contact-delegate.cpp
    src/contact-wrapper.cpp
    src/telepathy-contact.cpp
)

kde4_add_ui_files(plasma_applet_ktp_contact_SRCS
                    src/applet_config.ui
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
