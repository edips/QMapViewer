import QtQuick 2.10
import QtQuick.Controls 2.3
import QtQuick.Controls.Universal 2.3
import Fluid.Controls 1.0 as FluidControls
import QtQuick.Layouts 1.3
TopSheet {
    id: alertt
    title: "QMapViewer"
    onClosed: {
        loaderAboutVisible = false
    }
    SFlickable {
        id:settings_flickable
        anchors.margins: 15
        interactive: true
        anchors.top: alertt.toolbar.bottom
        contentHeight: column.implicitHeight
        Column {
            id: column
            anchors.fill: parent
            spacing: 15

            Label {
                onLinkActivated: Qt.openUrlExternally(link)
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                textFormat: Label.RichText
                font.pixelSize:14
                wrapMode: Label.WordWrap
                id:mylabel
                text:qsTr("Copyright © 2021 Edip Ahmet Taşkın, Geomatics Engineer\n<br><br>
You can mail to contact here <a href='mailto:geosoft66@gmail.com'>geosoft66@gmail.com</a><br>
You can also install another QGIS based app, <a href=\"https://play.google.com/store/apps/details?id=org.project.geoclass\">Surveying Calculator</a>.
<br><br>
QMapViewer is an open source app. You can access the source code here: <a href=\"https://github.com/edips/QMapViewer\">github.com/edips/QMapViewer</a>

"
                          )
            }
            Button {
                id:delegate
                anchors.horizontalCenter: parent.horizontalCenter
                text: qsTr("Rate QMapViewer")
                width: parent.width - 30
                icon.source: "qrc:/assets/icons/star_border.svg"
                flat: true
                onClicked: {
                    Qt.openUrlExternally("https://play.google.com/store/apps/details?id=org.project.qmapviewer")
                }
            }
            Label {
                onLinkActivated: Qt.openUrlExternally(link)
                width: parent.width
                anchors.horizontalCenter: parent.horizontalCenter
                textFormat: Label.RichText
                font.pixelSize:14
                wrapMode: Label.WordWrap
                id:mylabel2
                text:qsTr("
<a href=\"https://edips.github.io/privacypolicy.html\">Privacy policy</a><br><br>
<a href=\"https://edips.github.io\">Developer Website</a><br>
")

            }
        }

    }
}
