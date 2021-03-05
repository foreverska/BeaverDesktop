import QtQuick 2.15
import QtQuick.Window 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtQuick.Controls.Material 2.0

Window {
    width: 640
    height: 540
    minimumWidth: width
    maximumWidth: width
    minimumHeight: height
    maximumHeight: height
    visible: true
    color: "#c0d4d8"
    title: qsTr("Beaver En Route")

    ProgressBar {
        id: updateProgressBar
        x: 47
        y: 303
        width: 546
        height: 8
        value: guiBackend.progress
    }

    Text {
        id: targetLabel
        x: 47
        y: 127
        text: qsTr("Target CAN-ID:")
        font.pixelSize: 12
    }

    TextField {
        id: targetField
        x: 158
        y: 115
        placeholderText: qsTr("")
    }

    Text {
        id: firmwareLabel
        x: 52
        y: 194
        text: qsTr("Firmware File:")
        font.pixelSize: 12
    }

    TextField {
        id: firmwareField
        x: 158
        y: 181
        placeholderText: qsTr("")
    }

    Button {
        id: browseButton
        x: 364
        y: 181
        text: qsTr("Browse")
        onPressed: {
            fileDialog.visible = true
        }
    }

    Button {
        id: updateButton
        x: 493
        y: 250
        text: qsTr("Update")
        enabled: guiBackend.running == false
        onClicked: {
            guiBackend.filename = firmwareField.text
            guiBackend.target = targetField.text
            guiBackend.device = canInteraceSelection.currentText
            guiBackend.fullErase = eraseBox.checkState == Qt.Checked
            guiBackend.dryRun = dryRunBox.checkState == Qt.Checked
            guiBackend.reboot = rebootBox.checkState == Qt.Checked

            guiBackend.beginUpdate()
        }
    }

    CheckBox {
        id: eraseBox
        x: 264
        y: 250
        text: qsTr("Full Erase")
    }

    FileDialog{
        id: fileDialog
        title: "Choose firmware .out file"
        folder: shortcuts.home + "/workspace_v10/"
        selectMultiple: false
        onAccepted: {
            firmwareField.text = fileDialog.fileUrl
            fileDialog.close()
        }
        onRejected: {
            fileDialog.close()
        }
    }

    CheckBox {
        id: rebootBox
        x: 159
        y: 250
        text: qsTr("Reboot")
        checked: true
    }

    CheckBox {
        id: dryRunBox
        x: 47
        y: 250
        text: qsTr("Dry Run")
    }

    Rectangle{
        x: 47
        y: 317
        width: 546
        height: 193
        ScrollView{
            anchors.fill: parent
            anchors.rightMargin: 0
            anchors.bottomMargin: 2
            anchors.leftMargin: 0
            anchors.topMargin: -2
            TextArea {
                id: statusText
                x: -10
                y: -6
                text: logger.log
                font.pixelSize: 12
                horizontalAlignment: Text.AlignLeft
                verticalAlignment: Text.AlignTop
                wrapMode: Text.WordWrap
                textFormat: Text.PlainText
            }
        }
    }

    Image {
        id: image
        x: 493
        y: 51
        width: 100
        height: 100
        source: "beaver.png"
        fillMode: Image.PreserveAspectFit
    }

    Text {
        id: canInterfaceLabel
        x: 48
        y: 64
        text: qsTr("CAN Interface:")
        font.pixelSize: 12
    }

    ComboBox {
        id: canInteraceSelection
        x: 158
        y: 51
        width: 200
        height: 40
        model: guiBackend.canDevices
    }
}


