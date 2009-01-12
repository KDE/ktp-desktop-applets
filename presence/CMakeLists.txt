SET(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules" ${CMAKE_MODULE_PATH})
project(plasma-presence)

find_package(KDE4 REQUIRED)
find_package (TelepathyQt4 REQUIRED)
include(KDE4Defaults)

add_definitions(${KDE4_DEFINITIONS})

include_directories(
	${CMAKE_CURRENT_SOURCE_DIR}
	${CMAKE_CURRENT_BINARY_DIR}
	${KDE4_INCLUDES}
	${TELEPATHY_QT4_INCLUDE_DIR}
)

set(presence_SRCS
    presence.cpp
    presenceitemdelegate.cpp)

kde4_add_plugin(plasma_applet_presence ${presence_SRCS})
target_link_libraries(plasma_applet_presence 
	${KDE4_PLASMA_LIBS} 
	${KDE4_KDEUI_LIBS}
	${TELEPATHY_QT4_LIBRARIES}
)

install(TARGETS plasma_applet_presence DESTINATION ${PLUGIN_INSTALL_DIR})
install(FILES plasma-applet-presence.desktop DESTINATION ${SERVICES_INSTALL_DIR})
