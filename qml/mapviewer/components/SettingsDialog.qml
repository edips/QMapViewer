import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.3
import QtQuick.Layouts 1.3
import Fluid.Controls 1.0 as FluidControls
import Qt.labs.settings 1.1
import lc 1.0

FluidControls.AlertDialog {
    property bool scaleVisible: scale_visible.checked
    Settings {
        id: survSettings
        property alias scalebarVisible: scale_visible.checked
    }
    id: settingsDialog
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    width: parent.width
    height: 200
    title: qsTr("Settings")
    Flickable{
        id:settings_flickable
        Column{
            id:settings_column
            spacing: 15
            // Scalebar Units
            RowLayout{
                id: scaleRow
                SText {
                    text:qsTr("Scale bar unit: ")
                    font.bold: false
                }
                CustomComboBox {
                    id: scale_unit
                    implicitWidth: 200
                    currentIndex: __appSettings.scaleUnit
                    model: ListModel {
                        ListElement { text: qsTr("Metric") }
                        ListElement { text: qsTr("Imperial") }
                    }
                    onCurrentIndexChanged: __appSettings.scaleUnit = currentIndex
                }
            }
            Switch {
                id: scale_visible
                checked: true
                text: "Display scale bar"
            }
        }
        
    }
}
