add_library (ktpcontactlistplugin SHARED register-contact-applet.cpp qmlplugin.cpp)

target_link_libraries (ktpcontactlistplugin
    Qt5::Qml

    Qt5::DBus
)

install (TARGETS ktpcontactlistplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/ktpcontactlist)
install (FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/ktpcontactlist)
