import QtQuick 2.9
import QtQuick.Controls 2.2

TextField {
    property var txtvalidator: RegExpValidator { regExp: /^-?(\d+(?:[\.\,]\d{15})?)$/ }
    height: 38
    implicitWidth: 120
    leftPadding: 5
    font.pixelSize: 16
    selectByMouse: true
    validator: txtvalidator
    EnterKey.type: Qt.EnterKeyNext
    Keys.onReturnPressed:  nextItemInFocusChain().forceActiveFocus() 
    horizontalAlignment: Text.AlignHCenter
}
