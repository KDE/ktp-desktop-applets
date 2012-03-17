include_directories(
        ${CMAKE_SOURCE_DIR}
        ${CMAKE_BINARY_DIR}
        ${KDE4_INCLUDES}
        ${TELEPATHY_QT4_INCLUDE_DIR}
)

set(ktpcontactlistqmlplugin_SRCS
        qml-plugin.cpp
        contact-list.cpp
    )

kde4_add_library(ktpcontactlistqmlplugin SHARED ${ktpcontactlistqmlplugin_SRCS})

target_link_libraries(ktpcontactlistqmlplugin
        ${KDE4_KDECORE_LIBS}
        ${QT_QTSCRIPT_LIBRARY}
        ${QT_QTDECLARATIVE_LIBRARY}
        ${TELEPATHY_QT4_LIBRARIES}
        ${KTP_MODELS_LIBRARIES}
        ${KTP_LIBRARIES}
        ${KDE4_KDEUI_LIBS}
)

install(TARGETS ktpcontactlistqmlplugin DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/telepathy/contactlist)
install(FILES qmldir DESTINATION ${IMPORTS_INSTALL_DIR}/org/kde/telepathy/contactlist)
