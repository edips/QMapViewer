import QtQuick 2.10
import QtQuick.Window 2.2
import QtQuick.Controls 2.3
import QtQuick.Controls.Universal 2.3
import QtGraphicalEffects 1.0

RoundButton {
    id: control
    property color background_color;
    implicitWidth: Math.max(background ? background.implicitWidth : 0, contentItem.implicitWidth) + leftPadding + rightPadding
    implicitHeight: Math.max(background ? background.implicitHeight : 0, contentItem.implicitHeight) + topPadding + bottomPadding
    leftPadding: 6
    rightPadding: 6
    topPadding: 6
    bottomPadding: 6
    property bool mini: Screen.width < 460
    background: Rectangle {
        id: background_rect
        implicitWidth: control.mini ? 40 : 56
        implicitHeight: implicitWidth
        anchors.centerIn: parent
        radius: control.radius
        color: background_color
    }
}
