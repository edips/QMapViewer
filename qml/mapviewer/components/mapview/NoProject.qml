import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Universal 2.3

Label {
    id: noProject
    anchors.fill: parent
    horizontalAlignment: Qt.AlignHCenter
    verticalAlignment: Qt.AlignVCenter
    visible: false
    text: qsTr("No project found.")
    font.pixelSize: 18
    font.bold: true
    wrapMode: Label.WordWrap
}
