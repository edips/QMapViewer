import QtQuick 2.7
import QtGraphicalEffects 1.0
import QgsQuick 0.1 as QgsQuick
import QtSensors 5.12
import lc 1.0

Item {    
    property int rotation: 0;
    property bool nav_visible: parseFloat( (src.position.horizontalAccuracy) ) < 20 && __appSettings.autoCenterMapChecked

    id: crosshair
    property real size: {
        if (__appSettings.autoCenterMapChecked){
            return 20
        }else{
            return 45
        }
    }


    // todo: enable the compass when compass azimuth is active
    Compass {
        id: compass
        active: direction.visible
        skipDuplicates: true
        onReadingChanged: {
            //calib_level.text = calib_status(reading)
            crosshair.rotation = (reading.azimuth).toFixed(2)
        }
    }

    Image {
        id: direction
        visible: nav_visible
        source: "qrc:/assets/icons/navigation.svg"
        anchors.centerIn: parent
        fillMode: Image.PreserveAspectFit
        rotation: -crosshair.rotation
        transformOrigin: Item.Center
        sourceSize.width: 40
        sourceSize.height: 40
        smooth: true
        opacity: 0.5
        Behavior on rotation { RotationAnimation { properties: "rotation"; direction: RotationAnimation.Shortest; duration: 500 }}
    }

    ColorOverlay {
        anchors.fill: direction
        visible: nav_visible
        source: direction
        color: "#009900"
        rotation: direction.rotation
        transformOrigin: Item.Center
    }
}
