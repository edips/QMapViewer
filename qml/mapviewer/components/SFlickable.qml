import QtQuick 2.0
import QtQuick.Controls 2.3
Flickable {
    id:optionsPage
    anchors {
        left: parent.left
        right: parent.right
        top: parent.top
        bottom: parent.bottom
    }
    //interactive: false
    //maximumFlickVelocity: 1000
    boundsBehavior: Flickable.StopAtBounds
    width: parent.width
    clip: true
    ScrollIndicator.vertical: ScrollIndicator { }
}
