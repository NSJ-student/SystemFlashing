import QtQuick 2.9
import QtQuick.Controls 2.9
import QtQuick.Window 2.2
import QtQuick.Layouts 1.15
import QtQuick.Extras 1.4
import QtQuick.Dialogs 1.3
import JetsonTx2FlashingInfo 1.0
import TerminalProcess 1.0
import "."


Window {
    width: 550
    height: 500
    minimumWidth: 550
    minimumHeight: 500
    visible: true
    title: qsTr("Jetson TX2 flashing")

    signal flashImage();

    signal projectChanged(int index);
    signal displayChanged(int index);
    signal ipChanged(int index);
    signal remoteupgradeChanged(int index);
    signal dispctrlChanged(int index);

    signal keyPressed(string key);

    function qmlDisplayOut(data) {
        if(cbDisplayOut.count > 0)
        {
            cbDisplayOut.currentIndex = data;
        }
    }

    function qmlProcessRecv(data){
        textEdit.insert(textEdit.length, data);
    }

    Connections{
        target: jetson_obj
    }

    Connections{
        target: terminal_obj
    }

    Component.onCompleted: {
        console.log("window completed");
    }

    onWindowStateChanged: {
        console.log("windowState: " + windowState);
    }

    ColumnLayout {
        id: column
        anchors.fill: parent
        spacing: 20

        GroupBox {
            id: controlBox
            Layout.alignment: Qt.AlignLeft
            Layout.fillWidth: false
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.topMargin: 20
            font.bold: true

            GridLayout {
                id: gridLayout
                anchors.fill: parent
                rows: 7
                columns: 4

                ComboBox {
                    id: cbProject
                    Layout.columnSpan: 3
                    model: jetson_obj.projectList
                    onCurrentIndexChanged: {
                        projectChanged(currentIndex);
                    }
                }

                StatusIndicator {
                    id: statusSetting
                    Layout.columnSpan: 1
                }

                ToolSeparator {
                    id: toolSeparator
                    clip: false
                    Layout.columnSpan: 4
                    Layout.fillWidth: true
                    orientation: Qt.Horizontal
                }

                Text {
                    id: text6
                    text: qsTr("Display Out")
                    font.pixelSize: 12
                }

                ComboBox {
                    id: cbDisplayOut
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: jetson_obj.displayList
                    onCurrentIndexChanged: {
                        displayChanged(currentIndex);
                    }
                }

                Text {
                    id: text4
                    Layout.leftMargin: 20
                    text: qsTr("RemoteUpgrade")
                    font.pixelSize: 12
                }

                ComboBox {
                    id: cbRemoteUpgrade
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: jetson_obj.upgradeAppList
                    onCurrentIndexChanged: {
                        remoteupgradeChanged(currentIndex);
                    }
                }

                Text {
                    id: txtCurrentDispOut
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 2
                }

                Text {
                    id: txtCurrentRemoteUpgrade
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 2
                }

                Text {
                    id: text3
                    width: 11
                    height: 12
                    text: qsTr("IP")
                    font.pixelSize: 12
                }

                ComboBox {
                    id: cbIp
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: jetson_obj.ipList
                    onCurrentIndexChanged: {
                        ipChanged(currentIndex);
                    }
                }

                Text {
                    id: text2
                    Layout.leftMargin: 20
                    text: qsTr("DISP_CTRL")
                    font.pixelSize: 12
                }

                ComboBox {
                    id: cbDispCtrl
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignHCenter | Qt.AlignVCenter
                    model: jetson_obj.dispAppList
                    onCurrentIndexChanged: {
                        dispctrlChanged(currentIndex);
                    }
                }

                Text {
                    id: txtCurrentIp
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 2
                }

                Text {
                    id: txtCurrentDispCtrl
                    x: 216
                    y: 79
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 2
                }

                RowLayout {
                    id: rowLayout
                    Layout.fillWidth: true
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 4

                    Button {
                        id: btnLoadLastSetting
                        text: qsTr("Load")
                        onClicked: {
                            jetson_obj.loadSettingInfo("D:\\Projects\\JetsonTX2\\Software\\SystemFlashing\\init_test.xml");
                        }
                    }
                    Button {
                        id: btnSetConfig
                        text: qsTr("Flashing")
                        onClicked: {
                            flashImage();
                        }
                    }
                }


            }

        }

        Rectangle {
            id: cmdWindow
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.bottomMargin: 20
            color: "#000000"

            ScrollView {
                id: textScroll
                anchors.fill: cmdWindow
                clip: true

                TextEdit {
                    id: textEdit
                    anchors.fill: parent
                    color: "#ffffff"
                    textFormat: Text.PlainText
                    selectByMouse: true
                    readOnly: true
                    font.pixelSize: 12
                    selectionColor: "#ffffff"
                    font.family: "Courier"
                    anchors.rightMargin: 5
                    anchors.leftMargin: 5
                    anchors.bottomMargin: 0
                    anchors.topMargin: 0
                    selectedTextColor: "#000000"
                    Keys.onPressed: {
                        keyPressed(event.text);
//                        textEdit.insert(textEdit.length, event.text);
                    }
                }

                Keys.onPressed: {
//                    console.log("scrollview", textScroll.height);
                }
            }

            Keys.onPressed: {
//                console.log("cmdWindow ", cmdWindow.height);
            }
        }

    }


}
