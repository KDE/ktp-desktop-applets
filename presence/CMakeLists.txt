project(plasma-applet-presence)

find_package(KDE4 REQUIRED)
include(KDE4Defaults)

add_definitions(${KDE4_DEFINITIONS})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${KDE4_INCLUDES}
)

set(presence_SRCS
	presence.cpp
    accountimage.cpp
    accountwidget.cpp
    presencewidget.cpp
    globalpresencewidget.cpp)

kde4_add_plugin(plasma_applet_presence ${presence_SRCS})
target_link_libraries(plasma_applet_presence
	${KDE4_PLASMA_LIBS}
	${KDE4_KDEUI_LIBS}
)

install(TARGETS plasma_applet_presence DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-presence.desktop DESTINATION ${SERVICES_INSTALL_DIR})
