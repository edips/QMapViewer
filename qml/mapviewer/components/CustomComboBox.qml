import QtQuick 2.14
import QtQuick.Window 2.14
import QtQuick.Controls 2.14
import QtQuick.Controls.impl 2.14
import QtQuick.Templates 2.14 as T
import QtQuick.Controls.Universal 2.14

T.ComboBox {
    id: control

    implicitWidth: Math.max(implicitBackgroundWidth + leftInset + rightInset,
                            implicitContentWidth + leftPadding + rightPadding)
    implicitHeight: Math.max(implicitBackgroundHeight + topInset + bottomInset,
                             implicitContentHeight + topPadding + bottomPadding,
                             implicitIndicatorHeight + topPadding + bottomPadding)

    leftPadding: padding + (!control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)
    rightPadding: padding + (control.mirrored || !indicator || !indicator.visible ? 0 : indicator.width + spacing)

    Universal.theme: editable && activeFocus ? Universal.Light : undefined

    delegate: ItemDelegate {
        width: parent.width
        text: control.textRole ? (Array.isArray(control.model) ? modelData[control.textRole] : model[control.textRole]) : modelData
        font.weight: control.currentIndex === index ? Font.DemiBold : Font.Normal
        highlighted: control.highlightedIndex === index
        hoverEnabled: control.hoverEnabled
    }

    indicator: ColorImage {
        x: control.mirrored ? control.padding : control.width - width - control.padding
        y: control.topPadding + (control.availableHeight - height) / 2
        color: !control.enabled ? control.Universal.baseLowColor : control.Universal.baseMediumHighColor
        source: "qrc:/qt-project.org/imports/QtQuick/Controls.2/Universal/images/downarrow.png"

        Rectangle {
            z: -1
            width: parent.width
            height: parent.height
            color: control.activeFocus ? control.Universal.accent :
                                         control.pressed ? control.Universal.baseMediumLowColor :
                                                           control.hovered ? control.Universal.baseLowColor : "transparent"
            visible: control.editable && !contentItem.hovered && (control.pressed || control.hovered)
            opacity: control.activeFocus && !control.pressed ? 0.4 : 1.0
        }
    }

    contentItem: T.TextField {
        leftPadding: control.mirrored ? 1 : 12
        rightPadding: control.mirrored ? 10 : 1
        topPadding: 5 - control.topPadding
        bottomPadding: 7 - control.bottomPadding

        text: control.editable ? control.editText : control.displayText

        enabled: control.editable
        autoScroll: control.editable
        readOnly: control.down
        inputMethodHints: control.inputMethodHints
        validator: control.validator

        font: control.font
        color: !control.enabled ? control.Universal.chromeDisabledLowColor :
                                  control.editable && control.activeFocus ? control.Universal.chromeBlackHighColor : control.Universal.foreground
        selectionColor: control.Universal.accent
        selectedTextColor: control.Universal.chromeWhiteColor
        verticalAlignment: Text.AlignVCenter
    }

    background: Rectangle {
        implicitWidth: 120
        implicitHeight: 32

        border.width: control.flat ? 0 : 2 // ComboBoxBorderThemeThickness
        border.color: !control.enabled ? control.Universal.baseLowColor :
                                         control.editable && control.activeFocus ? control.Universal.accent :
                                                                                   control.down ? control.Universal.baseMediumLowColor :
                                                                                                  control.hovered ? control.Universal.baseMediumColor : control.Universal.baseMediumLowColor
        color: !control.enabled ? control.Universal.baseLowColor :
                                  control.down ? control.Universal.listMediumColor :
                                                 control.flat && control.hovered ? control.Universal.listLowColor :
                                                                                   control.editable && control.activeFocus ? control.Universal.background : control.Universal.altMediumLowColor
        visible: !control.flat || control.pressed || control.hovered || control.visualFocus

        Rectangle {
            x: 2
            y: 2
            width: parent.width - 4
            height: parent.height - 4

            visible: control.visualFocus && !control.editable
            color: control.Universal.accent
            opacity: control.Universal.theme === Universal.Light ? 0.4 : 0.6
        }
    }

    popup: T.Popup {
        width: control.width
        height: Math.min(contentItem.implicitHeight, control.Window.height - topMargin - bottomMargin)
        topMargin: 60
        bottomMargin: 8
        modal: true
        dim: false

        Universal.theme: control.Universal.theme
        Universal.accent: control.Universal.accent

        contentItem: ListView {
            clip: true
            implicitHeight: contentHeight
            model: control.delegateModel
            currentIndex: control.highlightedIndex
            highlightMoveDuration: 0
            boundsBehavior: Flickable.StopAtBounds

            T.ScrollIndicator.vertical: ScrollIndicator { }
        }

        background: Rectangle {
            color: control.Universal.chromeMediumLowColor
            border.color: control.Universal.chromeHighColor
            border.width: 1 // FlyoutBorderThemeThickness
        }
    }
}
