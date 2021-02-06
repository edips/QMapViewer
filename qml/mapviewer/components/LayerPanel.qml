// Author: Edip AHmet Taşkın
// Copy Right Edip Ahmet Taşkın
import QtQuick 2.10
import QtQuick.Controls 2.12
import QtQuick.Layouts 1.3
import QtGraphicalEffects 1.0
import QtQuick.Controls.Universal 2.12
import Fluid.Controls 1.0 as FluidControls
import QgsQuick 0.1 as QgsQuick
import lc 1.0

FluidControls.NavigationListView {
    id: navDrawer
    width: parent.width - 100
    height: parent.height
    readonly property bool mobileAspect: dataCollector.width < 500
    modal: true
    interactive: true
    visible: false
    ColumnLayout{
        width: parent.width
        ListView {
            id:my_layerlist
            implicitHeight: navDrawer.height
            implicitWidth: parent.width
            clip: true
            boundsBehavior: Flickable.StopAtBounds
            model:__layersModel
            spacing: 1
            delegate: ItemDelegate {
                height: 60
                width: parent.width
                onClicked: {
                    __loader.layerChecked(layerId)
                    icon2.source = __loader.layerVisibility( layerId ) ? "qrc:/assets/icons/check_box.svg" : "qrc:/assets/icons/check_box_outline_blank.svg"
                }
                Row{
                    spacing: 10
                    anchors.verticalCenter: parent.verticalCenter

                    Image {
                        id: icon2
                        visible: false
                        anchors.verticalCenter: parent.verticalCenter
                        source: __loader.layerVisibility( layerId ) ? "qrc:/assets/icons/check_box.svg" : "qrc:/assets/icons/check_box_outline_blank.svg"
                        sourceSize.width: 25
                        sourceSize.height: 25

                    }
                    ColorOverlay{
                        width: icon2.width
                        height: icon2.height
                        source: icon2
                        color: Universal.accent
                        transformOrigin: Item.Center
                    }


                    Image {
                        id: icon
                        anchors.verticalCenter: parent.verticalCenter
                        source: iconSource
                        sourceSize.width: 20
                        sourceSize.height: 20
                    }

                    SText{
                        font.pixelSize: 16
                        text: layerName
                    }

                }
            }
        }
    }
}
