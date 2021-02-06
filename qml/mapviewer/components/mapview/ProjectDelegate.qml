import QtQuick 2.7
import QtQuick.Controls 2.2
import QtQuick.Controls.Universal 2.3
import Fluid.Controls 1.0 as FluidControls

Component {
    id: projectDelegate
    FluidControls.ListItem {
        id: listItem
        // We have to use this instead of just text becahse when using text only, there is a problem about vertical center for text
        Label {
            text: (projectNamespace && projectName) ? (projectNamespace + "/" + projectName) : folderName
            verticalAlignment: Text.AlignVCenter
            height: parent.height;
            font.pixelSize: 16
            leftPadding: 50
            width: parent.width - 60
            clip: true
            font.bold: listItem.highlighted
            anchors.verticalCenter: parent.verticalCenter
            color: Universal.foreground
        }
        // use question mark for unrecognized projects
        icon.source: {
            if(!isValid) return "qrc:/assets/icons/help_outline.svg"
            return "qrc:/assets/icons/map.svg"
        }
        width: parent.width
        visible: height ? true : false
        enabled: isValid
        highlighted: activeProjectPath !== "" && path === activeProjectPath ? true : false
        onClicked: {
            activeProjectIndex = index
            openPanel.close()
        }
    }
}
