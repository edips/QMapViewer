// Author: Edip AHmet Taşkın
// Copy Right Edip Ahmet Taşkın
import QtQuick 2.10 as QQ
import QtLocation 5.6
import QtPositioning 5.6
import QtQuick.Controls 2.12
import QtQuick.Controls.Universal 2.12
import QtQuick.Layouts 1.3
import QtQuick.Window 2.12
import Fluid.Controls 1.0 as FluidControls
import QgsQuick 0.1 as QgsQuick
import Qt.labs.settings 1.1
import lc 1.0
import QtQml.Models 2.12
import "components"
import "components/mapviewer.js" as JS

FluidControls.Page {
    // counter for full screen round button for mapcanvas
    property int count_full: 0;
    // for help when the app initializes first time
    property int help_count: 0;
    // // Project's EPSG ID for transforming from project crs to active layer's crs cursor coordinate system
    // Project's EPSG ID
    property int epsgID;
    // Project's Ccoordinate System Name
    property string epsgName;
    // is geographic
    property bool isGeographic;
    // screen point coordinates when clicking on map canvas
    property var screenPoint;
    property var projectedPosition;
    property bool projectPositionValid: false
    // You can change UI based on the display mode
    //property bool portaitMode: Screen.desktopAvailableHeight > Screen.desktopAvailableWidth
    title:qsTr("")
    appBar.maxActionCount: 5
    anchors.fill: parent
    id: dataCollector
    visible: true

    Settings {
        id: mapsettings
        property alias count_help: dataCollector.help_count
    }

    QQ.Component.onCompleted: {
        if( help_count === 0 )
            maphelp.open()
        help_count = 1;
    }

    // PositionSource to get currrent GPS coordinates
    PositionSource {
        id: src
        updateInterval: 1000
        // Active when the button is pressed
        active: __appSettings.autoCenterMapChecked
        QQ.Component.onCompleted: epsgID = __loader.epsg_code()
        onPositionChanged: {
            if( epsgID !== 0 && src.valid ) {
                // Converting Latitude and Longitude to projet coordinate system

                // (array) Get GPS's lat and long
                var currentPosition = src.position.coordinate

                // (QgsPoint) convert GPS coordinate string to QgsPoint
                var coord_qgs = __layersModel.addFeatureSurvey( currentPosition.latitude, currentPosition.longitude )

                // update EPSG code of QGIS project
                epsgID = __loader.epsg_code()

                // (array) Convert GPS coordinate to Project's CRS for marker location
                var coords_pr = __loader.coordTransformer( coord_qgs,
                                                          mapView.canvasMapSettings.transformContext(), 4326, epsgID )

                // (QgsPoint) Convert coordinate of Project's CRS for marker's location on map canvas
                var newpoint = __layersModel.addFeatureSurvey( coords_pr[1], coords_pr[0] )
                mapView.mapCanvas.mapSettings.setCenter( newpoint );
                projectPositionValid = true
                projectedPosition = newpoint
            }
        }
    }

    // Cursor coordinate connection
    QQ.Connections {
        target: mapView.canvasMapSettings
        onExtentChanged: {
            if( projectPositionValid === true ) {
                // set marker to center of the position when moving the map canvas
                mapView.mapCanvas.mapSettings.setCenter( projectedPosition );
            }
        }
    }
    // Map View component with map canvas and basic project browser
    /*
    TODO: Highlight fix: when user click on another place highlighted feature must turn off
    */
    MapView {
        id: mapView
        z: activeProjectIndex === -1 ? 10 : 0
        anchors.fill: parent
        mapCanvas.onClicked: {
            if(count_full %2 === 0) {
                mapCanvas.forceActiveFocus()
                screenPoint = Qt.point( mouse.x, mouse.y );
                var res = identifyKit.identifyOne(screenPoint);
                if (res.valid) {
                    highlightMap.visible = true
                    highlightMap.featureLayerPair = res

                    // Add Menu Items

                    if( __surveyingUtils.featureIsPoint(res) ) {
                        JS.editAttributeItem()
                    }
                    else if( __surveyingUtils.featureIsLine(res) ) {
                        JS.editAttributeItem()
                        JS.lengthItem()
                    }
                    else if( __surveyingUtils.featureIsPolygon(res) ) {
                        JS.editAttributeItem()
                        JS.areaItem()
                    }
                    featureMenu.popup(mouse.x, mouse.y)
                }
            }
        }
        // MapView's initialization
        QQ.Component.onCompleted: {
            isGeographic = __loader.isGeographic()
            __loader.mapSettings = mapView.canvasMapSettings
            //__loader.positionKit = positionKit
            __loader.recording = digitizing.recording
            epsgID = __loader.epsg_code()
            if( __activeLayer.layerName != "" ) {
                epsgName = __loader.epsg_name()
            }
        }
    }

    // map canvas menu
    Menu {
        id: featureMenu
        modal: true
        dim: false
        onClosed: {
            highlightMap.visible = false
            // remove all menu items when closing the menu
            while(featureMenu.count > 0) {
                featureMenu.removeItem(featureMenu.itemAt(0));
            }
        }

        // For Line

        QQ.Component {
            id: length
            MenuItem {
                onTriggered: {
                    lengthcombo.currentIndex = 0
                    var res = identifyKit.identifyOne(screenPoint);
                    var length_val = __surveyingUtils.getLength( res )
                    length_metric = length_val
                    length_dialog.title = "Length"
                    length_dialog.lengthValue.text = length_val
                    length_dialog.open()
                }
            }
        }


        QQ.Component {
            id: area
            MenuItem {
                onTriggered: {
                    areacombo.currentIndex = 0
                    var res = identifyKit.identifyOne(screenPoint);
                    var area_val = __surveyingUtils.getArea( res )
                    area_metric = area_val
                    area_dialog.areaValue.text = area_val
                    area_dialog.title = "Area"
                    area_dialog.open()
                }
            }
        }

        // Edit attribute menu item

        QQ.Component {
            id: edit_attribute
            MenuItem {
                onTriggered: {
                    var res = identifyKit.identifyOne(screenPoint);
                    if (res.valid) {
                        highlightMap.featureLayerPair = res
                        featurePanel.show_panel(res, "Edit" )
                    }
                    else if (featurePanel.visible) {
                        // closes feature/preview panel when there is nothing to show
                        featurePanel.visible = false
                    }
                }
            }
        }
    }


    // Area unit conversion
    property double area_metric;



    // area dialog
    SErrorDialog {
        id: area_dialog
        property alias areaValue: area_value
        QQ.Row{
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            STextField{id: area_value; width: 150; readOnly:true}
            CustomComboBox {
                id: areacombo
                height: area_value.height
                currentIndex: 0
                implicitWidth:80
                model: ListModel {
                    ListElement { text: qsTr("m²") }
                    ListElement { text: qsTr("km²") }
                    ListElement { text: qsTr("ha") }
                    ListElement { text: qsTr("acre") }
                    ListElement { text: qsTr("mi²") }
                    ListElement { text: qsTr("yd²") }
                    ListElement { text: qsTr("ft²") }
                }
                onCurrentIndexChanged: JS.areaUnits( area_value, area_metric );
            }
        }
    }

    // length unit conversion
    property double length_metric;


    // length dialog
    SErrorDialog {
        id: length_dialog
        property alias lengthValue: length_value
        QQ.Row{
            anchors.horizontalCenter: parent.horizontalCenter
            spacing: 10
            STextField{id: length_value; width: 150; readOnly:true}
            CustomComboBox {
                id: lengthcombo
                height: length_value.height
                currentIndex: 0
                implicitWidth:80
                model: ListModel {
                    ListElement { text: qsTr("m") }
                    ListElement { text: qsTr("km") }
                    ListElement { text: qsTr("mi") }
                    ListElement { text: qsTr("nmi") }
                    ListElement { text: qsTr("yds") }
                    ListElement { text: qsTr("ft") }
                }
                onCurrentIndexChanged: JS.lengthUnits( length_value, length_metric );
            }
        }
    }
    // FeaturePanel to edit features
    QgsQuick.IdentifyKit {
        id: identifyKit
        //parent: mapView.mapCanvas
        mapSettings: mapView.canvasMapSettings
        identifyMode: QgsQuick.IdentifyKit.TopDownAll
    }
    // feature panel to edit features
    FeaturePanel {
        id: featurePanel
        width: dataCollector.width
        mapSettings: mapView.canvasMapSettings
        project: __loader.project
        visible: false
    }
    // Feature Heighlight
    QgsQuick.FeatureHighlight {
        anchors.fill: mapView
        id: highlightMap
        mapSettings: mapView.canvasMapSettings
        z: 2
    }

    // Digitizing Controller
    DigitizingController {
        id: digitizing

        lineRecordingInterval: __appSettings.lineRecordingInterval
        mapSettings: mapView.canvasMapSettings

        onRecordingChanged: {
            __loader.recording = digitizing.recording
        }
    }

    // Settings dialog
    SettingsDialog {
        id: settingsDialog
    }

    // About loader visible
    property bool loaderAboutVisible: false
    QQ.Loader {
        id: about
        anchors.fill: parent
        source: loaderAboutVisible ? "components/DialogAbout.qml" : ""
    }

    // Menu
    Menu {
        id: moreMenu
        x: parent.width - width
        y: windoww.height
        transformOrigin: Menu.TopRight
        modal: true
        // Settings
        MenuItem{
            onTriggered: settingsDialog.open()
            text: qsTr("Settings")
            icon.source: "qrc:/assets/icons/settings.svg"
        }
        // Help
        MenuItem{
            onTriggered: maphelp.open()
            text: qsTr("Help")
            icon.source: "qrc:/assets/icons/help_outline.svg"
        }
        // About
        MenuItem {
            text: qsTr("About")
            icon.source: "qrc:/assets/icons/info_outline.svg"
            onTriggered: {
                loaderAboutVisible = true
                about.item.open()
            }
        }
    }
    // Actions on toolbar
    actions: [
        // Open project button
        FluidControls.Action {
            onTriggered: {
                mapView.openPanel.open()
            }
            toolTip: qsTr("Open Project")
            icon.source: "qrc:/assets/icons/folder.svg"
        },
        // GPS button
        FluidControls.Action {
            id:gpsAction
            icon.color: {
                if( __appSettings.autoCenterMapChecked ) {
                    return Universal.color( Universal.Orange )
                }
                else{
                    return "black"
                }
            }
            onTriggered: {
                __appSettings.autoCenterMapChecked = !__appSettings.autoCenterMapChecked
                projectPositionValid = false

            }
            toolTip: qsTr("Enable GPS")
            icon.source: "qrc:/assets/icons/gps_fixed.svg"
        },
        // Layers button
        FluidControls.Action {
            icon.source: "qrc:/assets/icons/layers.svg"
            toolTip: qsTr("Layers")
            //visible: navDrawer.modal
            onTriggered: {
                navDrawer.open()
            }
        },
        // Zoom to Project
        FluidControls.Action {
            id:mya
            toolTip: qsTr("Zoom to Project")
            icon.source: "qrc:/assets/icons/zoom_out_map.svg"
            onTriggered:{
                if( __appSettings.autoCenterMapChecked ){
                    __appSettings.autoCenterMapChecked =! __appSettings.autoCenterMapChecked
                    projectPositionValid = false
                }
                __loader.zoomToProject( mapView.canvasMapSettings )
            }
        },
        // Menu
        FluidControls.Action {
            id:menuBar
            toolTip: "Menu"
            icon.source: "qrc:/assets/icons/more_vert.svg"
            onTriggered: {
                moreMenu.open()
                //fileDialog.open()
            }
        }
    ]

    // Help Page
    QMapViewerHelp {
        id: maphelp
        z: mapView.openPanel.z + 1
        visible: false
    }



    // ScaleBar in metric or imperial units
    ScaleBar {
        id: scaleBar
        visible: settingsDialog.scaleVisible
        height: 30
        anchors.bottom: parent.bottom
        anchors.bottomMargin: 10
        mapSettings: mapView.canvasMapSettings
        preferredWidth: Math.min(dataCollector.width, 180 * QgsQuick.Utils.dp)
        z: 1
        systemOfMeasurement: JS.scalebar_settings()
        anchors.horizontalCenter: parent.horizontalCenter
    }
    // Record Crosshair
    RecordCrosshair {
        id: crosshair
        width: mapView.width
        height: mapView.height
        visible: true
        z: 2
    }
    // Layers
    LayerPanel{ id: navDrawer }
    // Full screen and normal screen map
    FullMapBtn {
        id: fullMapBtn
        z: highlightMap.z + 1
    }
}
