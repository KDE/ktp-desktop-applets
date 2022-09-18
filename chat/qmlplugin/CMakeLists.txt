add_library (ktpchatplugin SHARED hide-window-component.cpp htmlhelper.cpp qmlplugin.cpp)

target_link_libraries (ktpchatplugin
    Qt5::Qml
    
    KF5::WindowSystem
)

install (TARGETS ktpchatplugin DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/ktpchat)
install (FILES qmldir DESTINATION ${KDE_INSTALL_QMLDIR}/org/kde/ktpchat)
