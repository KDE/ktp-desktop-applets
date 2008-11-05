project(plasma-presence)

include_directories(${CMAKE_CURRENT_BINARY_DIR}
                    ${CMAKE_CURRENT_SOURCE_DIR}
)

set(presence_SRCS
    presence.cpp
    presenceitemdelegate.cpp)

kde4_add_plugin(plasma_applet_presence ${presence_SRCS})
target_link_libraries(plasma_applet_presence plasmaappletdialog ${KDE4_PLASMA_LIBS} decibel ${KDE4_KDEUI_LIBS} QtTapioca)

install(TARGETS plasma_applet_presence DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-presence.desktop DESTINATION ${SERVICES_INSTALL_DIR})
