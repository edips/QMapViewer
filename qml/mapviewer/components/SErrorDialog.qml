import QtQuick 2.0
import Fluid.Controls 1.0 as FluidControls

FluidControls.AlertDialog {
    property string error;
    property string dialog_title;
    id: error_dialog
    title: "Error"
    modal: true
    x: (parent.width - width) / 2
    y: (parent.height - height) / 2
    text: error
    width: parent.width
    height: 120 + dialog_text.implicitHeight
}
