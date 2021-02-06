import QtQuick 2.10
import QtQuick.Window 2.12

RoundBtn{
    z: mapCanvas.z + 1
    id:fullMapBtn
    background_color: "#80d9d9d9"
    icon.height: 35
    icon.width: 35
    anchors{
        right: parent.right
        top: parent.top
        rightMargin: 5

    }
    onClicked: {
        count_full++
        if (count_full %2 === 0){
            windoww.visibility = Window.AutomaticVisibility
            windoww.footer.visible = true
        }
        else if(count_full %2 === 1){
            windoww.visibility = Window.FullScreen
            windoww.footer.visible = false
        }
    }
    icon.source:{
        if (count_full %2 === 0){
            return "qrc:/assets/icons/fullscreen.svg"
        }
        else if(count_full %2 === 1){
            return "qrc:/assets/icons/fullscreen_exit.svg"
        }
    }
}
