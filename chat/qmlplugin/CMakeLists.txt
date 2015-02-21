add_library (ktpchatplugin SHARED hide-window-component.cpp qmlplugin.cpp)

target_link_libraries (ktpchatplugin
    Qt5::Qml
    
    KF5::WindowSystem
)

install (TARGETS ktpchatplugin DESTINATION ${QML_INSTALL_DIR}/org/kde/ktpchat)
install (FILES qmldir DESTINATION ${QML_INSTALL_DIR}/org/kde/ktpchat)
