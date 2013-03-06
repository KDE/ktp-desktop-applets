include_directories(${CMAKE_CURRENT_BINARY_DIR})

set(plasma_applet_ktp_presence_SRCS
    src/presence-applet.cpp
)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/plasma-applet-ktp-presence.desktop.cmake
               ${CMAKE_CURRENT_BINARY_DIR}/plasma-applet-ktp-presence.desktop
               @ONLY)

kde4_add_plugin(plasma_applet_ktp_presence ${plasma_applet_ktp_presence_SRCS})
target_link_libraries(plasma_applet_ktp_presence
                        ${KDE4_PLASMA_LIBS}
                        ${KDE4_KDEUI_LIBS}
                        ${TELEPATHY_QT4_LIBRARIES}
                        ${KTP_LIBRARIES}
                        ${KTP_MODELS_LIBRARIES}
                        ${KTP_WIDGETS_LIBRARIES}
                     )

install(TARGETS plasma_applet_ktp_presence DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES ${CMAKE_CURRENT_BINARY_DIR}/plasma-applet-ktp-presence.desktop DESTINATION ${SERVICES_INSTALL_DIR})
