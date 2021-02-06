import QtQuick 2.0
import QtQuick.Controls 2.5
import QtQuick.Controls.Universal 2.12
Rectangle{
    property string stext;
    color: "transparent"
    anchors.horizontalCenter: parent.horizontalCenter
    height: helplabel.height
    width: parent.width - 30
    Label{
        id:helplabel
        text: stext
        font.pixelSize:15
        textFormat: Label.RichText
        //anchors.horizontalCenter: parent.horizontalCenter
        wrapMode: Label.WordWrap
        width: parent.width
        topPadding: 10
    }
}
