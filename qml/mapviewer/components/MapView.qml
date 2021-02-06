import QtQuick 2.7
import QtQuick.Controls 2.2

import QtQml.Models 2.2
import QtPositioning 5.8
import QtQuick.Layouts 1.12
import QtQuick.Controls.Universal 2.3
import Fluid.Controls 1.0 as FluidControls
import Fluid.Core 1.0 as FluidCore
import QtQuick.Dialogs 1.1
import QgsQuick 0.1 as QgsQuick
import lc 1.0
import "mapview"

Item {
    // No project visible
    property bool noprojectVisible: projectList.count === 0
    // opens project panel to choose a project
    property alias openPanel: openProjectPanel
    // Map canvas
    property alias mapCanvas: mapCanvas
    // map settings of map canvas
    property alias canvasMapSettings: mapCanvas.mapSettings
    // identify kit to see attribute information form of an object
    //property alias identify: identifyKit
    // use it to set project panel permanently
    property bool alwaysOpenPanel: false
    // Enabled when it is used for project panel, disable it when using Map canvas related pages like data collector and map viewer
    property bool projectPanelEnabled: true

    // Active index of qgs project
    property int activeProjectIndex: -1
    // Active path of qgs project
    property string activeProjectPath: __projectsModel.data(__projectsModel.index(activeProjectIndex),
                                                            ProjectModel.Path)
    // Project index of right button click
    property int menuIndex: -1
    // Project path of right button click
    property string menuProjectPath: ""
    // When selecting another project on the list..
    onActiveProjectIndexChanged: {
        activeProjectPath = __projectsModel.data(__projectsModel.index(activeProjectIndex), ProjectModel.Path)
        __appSettings.defaultProject = activeProjectPath
        __appSettings.activeProject = activeProjectPath
        __loader.load(activeProjectPath)
    }

    id: window
    visible: true
    //anchors.fill: parent
    // BusyIndicator when loading map
    SBusyIndicator { id: busyIndicator; visible: mapView.mapCanvas.isRendering; z: mapCanvas.z + 1 }
    // initializa project parameters
    Component.onCompleted: {
        if (__appSettings.defaultProject) {
            var path = __appSettings.defaultProject
            var defaultIndex = __projectsModel.rowAccordingPath(path)
            var isValid = __projectsModel.data(__projectsModel.index(defaultIndex), ProjectModel.IsValid)
            if (isValid && __loader.load(path)) {
                activeProjectIndex = defaultIndex !== -1 ? defaultIndex : 0
                __appSettings.activeProject = path
            } else {
                // if default project load failed, delete default setting
                __appSettings.defaultProject = ""
                openPanel.open()
            }
        } else {
            openPanel.open()
        }
        __loader.mapSettings = mapCanvas.mapSettings
    }
    // Map Canvas
    QgsQuick.MapCanvas {
        id: mapCanvas
        height: parent.height
        width: parent.width
        z: 0
        mapSettings.project: __loader.project
        onIsRenderingChanged: busyIndicator.busy.running = isRendering
    }
    // Transform project CRS with Map Canvas CRS
    Item {
        transform: QgsQuick.MapTransform { mapSettings: mapCanvas.mapSettings }
        anchors.fill: mapCanvas
        z: 1  // make sure items from here are on top of the Z-order
    }
    // Project List Panel
    TopSheet {
        property real rowHeight: 50
        title: "Select QGIS Project"
        id: openProjectPanel
        // Project panel opens when map canvas used but active project is invalid.
        // Project panel always opens when it is on Project Manager page
        // initialize __projectsmodel for list model
        Component.onCompleted: projectList.model = __projectsModel
        // Project List View
        SFlickable {
            //implicitHeight: projectList.implicitHeight + exHeight.implicitHeight
            anchors.top: openProjectPanel.toolbar.bottom
            width: parent.width
            focus: true
            interactive: true
            contentHeight: Math.max(projectList.contentHeight + 100, height)
            Rectangle {
                width: parent.width
                height: parent.height
                color: "transparent"
                // Project ListView
                ListView {
                    id: projectList
                    interactive: false
                    anchors.fill: parent
                    contentWidth: projectList.width
                    clip: true
                    visible: true
                    delegate: projectDelegate
                    // No project found label
                    NoProject { id: noProject; visible: noprojectVisible }
                }
            }
        }

        // Project list component
        ProjectDelegate { id: projectDelegate }
    }
}
