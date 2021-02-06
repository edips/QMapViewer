import QtQuick 2.7
import QtQuick.Controls 2.2
import QgsQuick 0.1 as QgsQuick

Item {
    id: scaleBar
    property alias mapSettings: scaleBarKit.mapSettings
    property alias preferredWidth: scaleBarKit.preferredWidth



    QgsQuick.ScaleBarKit {
        id: scaleBarKit
    }

    property alias systemOfMeasurement: scaleBarKit.systemOfMeasurement
    property color barColor: "#36454f"
    property string barText: scaleBarKit.distance + " " + scaleBarKit.units
    property int barWidth: scaleBarKit.width
    property int lineWidth: 2 * QgsQuick.Utils.dp

    width: barWidth

    Rectangle {
        id: background
        color: "white"
        opacity: 0.75
        width: parent.width
        height: parent.height
        radius: 7
    }

    Item {
        anchors.fill: parent
        anchors.leftMargin: 5 * QgsQuick.Utils.dp
        anchors.rightMargin: anchors.leftMargin

        STextTop {
            id: text
            text: barText
            color: barColor
            font.bold: false
            font.pixelSize: 17 * 0.75
            anchors.fill: parent
            horizontalAlignment: SText.AlignHCenter
            verticalAlignment: SText.AlignTop
        }

        Rectangle {
            id: leftBar
            width: scaleBar.lineWidth
            height: scaleBar.height/3
            y: (scaleBar.height - leftBar.height) / 2
            color: barColor
            opacity: 1
            anchors.right: parent.right
            anchors.rightMargin: 0
        }

        Rectangle {
            id: bar
            anchors.left: parent.left
            anchors.right: parent.right
            height: scaleBar.lineWidth
            y: (scaleBar.height - scaleBar.lineWidth) / 2
            color: barColor
        }

        Rectangle {
            id: rightBar
            width: scaleBar.lineWidth
            height: scaleBar.height/3
            y: (scaleBar.height - leftBar.height) / 2
            color: barColor
        }
    }
}
