import QtQuick 2.1
import org.kde.plasma.plasmoid 2.0

QtObject
{
    Plasmoid.switchWidth: 0
    Plasmoid.switchHeight: 0
    Plasmoid.preferredRepresentation: Plasmoid.fullRepresentation
    Plasmoid.fullRepresentation: FullChatList {}
    Plasmoid.onExpandedChanged: Plasmoid.fullRepresentationItem.activate()
}
