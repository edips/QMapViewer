// Autohor: Edip Ahmet Taşkın
// 12.2019

import QtQuick 2.9
import QtQuick.Controls 2.3
import QtQuick.Controls.Universal 2.2
import Fluid.Controls 1.0 as FluidControls
import Fluid.Core 1.0 as FluidCore
import QtQuick.Layouts 1.3

/* Documentation

 User should enter title
 For its children (Flickable), type anchors.top: toolbar.bottom

*/

Popup {
    property string title;
    property alias toolbar: tool_bar
    property alias toolbar_group: toolbarGroup
    property color universalcolor: Universal.accent

    focus: true
    modal: true

    onClosed: {
        loaderVisible =  false
    }
    Component.onCompleted: forceActiveFocus()

    id: topSheet
    height: parent.height + FluidCore.Device.gridUnit
    width: parent.width
    visible: false
    closePolicy: Popup.CloseOnEscape // prevents the drawer closing while moving canvas

    padding: 0


    FluidControls.AppToolBar {
        id:tool_bar
        height: FluidCore.Device.gridUnit * 0.8


        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }

        RowLayout {
            id: toolbarGroup
            anchors.fill: parent
            Layout.margins: 0
            spacing: 15
            Label {
                id: titleLabel
                text: title
                font.bold: true
                font.pixelSize: 14
                elide: Label.ElideRight
                horizontalAlignment: Qt.AlignHCenter
                verticalAlignment: Qt.AlignVCenter
                Layout.fillWidth: true
            }

            FluidControls.ToolButton {
                id: closeButton
                Layout.alignment: Qt.AlignRight
                flat: true
                icon.source: "qrc:/assets/icons/close.svg"
                onClicked: {
                    topSheet.close()
                }
            }
        }

    }
}
