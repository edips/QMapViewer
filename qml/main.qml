import QtQuick 2.12 as QQ
import QtQuick.Controls 2.12 as QQC2
import QtQuick.Controls.Universal 2.12
import Fluid.Controls 1.0 as FluidControls
import QtQuick.Layouts 1.12
import QtQuick.Window 2.12
import "mapviewer"

FluidControls.ApplicationWindow {
    // Settings loader visibility
    property bool loaderVisible: false
    property int count;

    id: windoww
    visible:true
    title: "Surveying Calculator"
    visibility: Window.AutomaticVisibility
    // font loader
    font.family : "Lato"
    width: 1080
    height: 1920
    // Count of the app action icons
    appBar.maxActionCount: 1
    Universal.theme: Universal.Light
    Universal.accent: "#0050EF"

    // snack bar
    FluidControls.SnackBar { id: snack }


    // Initial Main Page
    initialPage: QMapViewer {}


    // Close the app ToolTip
    property bool alreadyCloseRequested: false
    onClosing: {
        if(__androidUtils.isAndroid){
            if( !alreadyCloseRequested )
            {
                __androidUtils.showToast( "Press back again to exit" )
                close.accepted = false
                alreadyCloseRequested = true
                closingTimer.start()
            }
            else
            {
                close.accepted = true
            }
        }else{
            close.accepted = true
        }
    }

    QQ.Timer {
        id: closingTimer
        interval: 2000
        onTriggered: {
            alreadyCloseRequested = false
        }
    }
}
