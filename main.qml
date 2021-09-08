import QtQuick 2.9
import QtQuick.Controls 2.9
import QtQuick.Window 2.2
import QtQuick.Layouts 1.15
import QtQuick.Extras 1.4
import QtQuick.Dialogs 1.3
import JetsonTx2FlashingInfo 1.0
import "."


Window {
    width: 640
    height: 480
    minimumHeight: 480
    maximumHeight: 480
    minimumWidth: 640
    maximumWidth: 640
    visible: true
    title: qsTr("Jetson TX2 flashing")

    signal loadSetting();
    signal saveSetting();

    signal projectChanged(int index);
    signal displayChanged(int index);
    signal ipChanged(int index);
    signal remoteupgradeChanged(int index);
    signal dispctrlChanged(int index);

    //qml에 클래스를 선언해 주는 곳
    JetsonTx2FlashingInfo
    {
        //클래스를 선언해주고 id를 등록
        id:jetson_flashing_info;
    }

    Component.onCompleted: {

    }

    onWindowStateChanged: {
        console.log("state: " + windowState);
    }

    Rectangle {
        id: rectangle
        x: 30
        y: 312
        width: 580
        height: 152
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
            Keys.onPressed: {
                event.key;
            }
        }

    }

    StatusIndicator {
        id: statusSetting
        x: 538
        y: 42
    }

    GroupBox {
        id: groupBox
        x: 30
        y: 24
        width: 580
        height: 271
        font.bold: true

        GridLayout {
            id: gridLayout
            anchors.fill: parent
            rows: 7
            columns: 4

            ComboBox {
                id: cbProject
                x: 0
                y: 9
                width: 126
                height: 36
                Layout.columnSpan: 4
                model: jetson_flashing_info.projectList
                onCurrentIndexChanged: {
                    jetson_flashing_info.projectChanged(currentIndex);
                }
            }

            ToolSeparator {
                id: toolSeparator
                x: 207
                y: 11
                width: 540
                clip: false
                Layout.columnSpan: 4
                orientation: Qt.Horizontal
            }

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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: jetson_flashing_info.displayList
                onCurrentIndexChanged: {
                    jetson_flashing_info.displayChanged(currentIndex);
                }
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: jetson_flashing_info.upgradeAppList
                onCurrentIndexChanged: {
                    jetson_flashing_info.remoteupgradeChanged(currentIndex);
                }
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: jetson_flashing_info.ipList
                onCurrentIndexChanged: {
                    jetson_flashing_info.ipChanged(currentIndex);
                }
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
                Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                model: jetson_flashing_info.dispAppList
                onCurrentIndexChanged: {
                    jetson_flashing_info.dispctrlChanged(currentIndex);
                }
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
                    onClicked: {
//                        jetson_flashing_info.button_test("button1");
                        jetson_flashing_info.loadSettingInfo("D:\\Projects\\JetsonTX2\\Software\\build-SystemFlashing-Desktop_Qt_5_15_1_MSVC2019_32bit-Debug\\debug\\init_test.xml");
                    }
                }
                Button {
                    id: btnSetConfig
                    x: 384
                    y: 185
                    text: qsTr("Flashing")
                    onClicked: {
                        jetson_flashing_info.button_test("button2");
                    }
                }
            }


        }

    }


}
