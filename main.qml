import QtQuick 2.9
import QtQuick.Controls 2.9
import QtQuick.Window 2.2
import QtQuick.Layouts 1.15
import QtQuick.Extras 1.4

Window {
    width: 640
    height: 480
    minimumHeight: 480
    maximumHeight: 480
    minimumWidth: 640
    maximumWidth: 640
    visible: true
    title: qsTr("Hello World")

    Button {
        id: btnLoadConfig
        x: 30
        y: 18
        width: 107
        height: 40
        text: "Load Settings"
        onClicked: model.revert()
    }

    Rectangle {
        id: rectangle
        x: 30
        y: 275
        width: 580
        height: 189
        color: "#000000"

        TextEdit {
            id: textEdit
            anchors.fill: parent
            color: "#ffffff"
            selectByMouse: true
            readOnly: true
            font.pixelSize: 12
            selectionColor: "#ffffff"
            font.family: "Courier"
            anchors.rightMargin: 5
            anchors.leftMargin: 5
            anchors.bottomMargin: 5
            anchors.topMargin: 5
            selectedTextColor: "#000000"
        }

    }

    StatusIndicator {
        id: statusIndicator
        x: 154
        y: 26
    }

    GroupBox {
        id: groupBox
        x: 30
        y: 70
        width: 580
        height: 191
        font.bold: true

        GridLayout {
            id: gridLayout
            anchors.fill: parent
            rows: 5
            columns: 4

            Text {
                id: text6
                x: 18
                y: 47
                text: qsTr("Display Out")
                font.pixelSize: 12
            }

            ComboBox {
                id: cbDisplayOut
                x: 101
                y: 40
                width: 92
                height: 20
                model: ["DSI 2CH", "DSI 1CH", "HDMI"]
            }

            Text {
                id: text4
                x: 216
                y: 79
                text: qsTr("RemoteUpgrade")
                font.pixelSize: 12
            }

            ComboBox {
                id: cbRemoteUpgrade
                x: 321
                y: 40
                width: 92
                height: 20
            }

            Text {
                id: txtCurrentDispOut
                x: 216
                y: 79
                text: qsTr("")
                font.pixelSize: 10
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.columnSpan: 2
            }

            Text {
                id: txtCurrentRemoteUpgrade
                x: 216
                y: 79
                text: qsTr("")
                font.pixelSize: 10
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.columnSpan: 2
            }

            Text {
                id: text3
                x: 18
                y: 79
                width: 11
                height: 12
                text: qsTr("IP")
                font.pixelSize: 12
            }

            ComboBox {
                id: cbIp
                x: 101
                y: 10
                width: 92
                height: 20
                model: ["192.168.20.192", "192.168.20.193"]
            }

            Text {
                id: text2
                x: 216
                y: 47
                text: qsTr("DISP_CTRL")
                font.pixelSize: 12
            }

            ComboBox {
                id: cbDispCtrl
                x: 321
                y: 72
                width: 92
                height: 20
            }

            Text {
                id: txtCurrentIp
                x: 216
                y: 79
                text: qsTr("")
                font.pixelSize: 10
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.columnSpan: 2
            }

            Text {
                id: txtCurrentDispCtrl
                x: 216
                y: 79
                text: qsTr("")
                font.pixelSize: 10
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.columnSpan: 2
            }

            RowLayout {
                id: rowLayout
                x: 438
                y: 296
                width: 100
                height: 100
                Layout.rightMargin: 15
                Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                Layout.columnSpan: 4

                Button {
                    id: btnLoadLastSetting
                    x: 384
                    y: 185
                    text: qsTr("Load")
                }
                Button {
                    id: btnSetConfig
                    x: 384
                    y: 185
                    text: qsTr("Flashing")
                }
            }


        }

    }

}
