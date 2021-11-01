import QtQuick 2.9
import QtQuick.Controls 2.2
import QtQuick.Window 2.2
import QtQuick.Layouts 1.1
import QtQuick.Extras 1.4
import QtQuick.Dialogs 1.2
import JetsonTx2FlashingInfo 1.0
import TerminalProcess 1.0
import "."


Window {
    id: root
    width: 550
    height: 500
    minimumWidth: 550
    minimumHeight: 500
    visible: true
    title: qsTr("Jetson TX2 flashing")

    signal flashImage();
    signal flashImageNoMake();
    signal flashDtb();

    signal loadConfig(string path);
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
//        textArea.(data)
        textArea.insert(textArea.length, data);
        textArea.cursorPosition = textArea.length-1;
//        cmdWindow.update();
    }

    function qmlRemoveChar(){
        textArea.remove(textArea.length-1, textArea.length);
    }

    function qmlConnected() {
        statusSetting.active = true;
        btnFlashImage.enabled = true;
        btnFlashDTB.enabled = true;
    }

    function qmlDisconnected() {
        statusSetting.active = false;
        btnFlashImage.enabled = false;
        btnFlashDTB.enabled = false;
    }

    function qmlInit() {
        btnFlashImage.enabled = true;
        btnFlashDTB.enabled = true;
    }

    function qmlProject(data) {
        txtLastProject.text = data;
    }

    function qmlLastUpdate(data) {
        txtLastUpdate.text = data;
    }

    function qmlDispOut(data){
        txtLastDispOut.text = data;
    }

    function qmlIp(data){
        txtLastIp.text = data;
    }

    function qmlRemoteUpgrade(data){
        txtLastRemoteUpgrade.text = data;
    }

    function qmlDispCtrl(data){
        txtLastDispCtrl.text = data;
    }

    function qmlCurrentProject(data){
        if(cbProject.count > data)
        {
            cbProject.currentIndex = data;
        }
    }

    function qmlCurrentDispOut(data){
        if(cbDisplayOut.count > data)
        {
            cbDisplayOut.currentIndex = data;
        }
    }

    function qmlCurrentIp(data){
        if(cbIp.count > data)
        {
            cbIp.currentIndex = data;
        }
    }

    function qmlCurrentRemoteUpgrade(data){
        if(cbRemoteUpgrade.count > data)
        {
            cbRemoteUpgrade.currentIndex = data;
        }
    }

    function qmlCurrentDispCtrl(data){
        if(cbDispCtrl.count > data)
        {
            cbDispCtrl.currentIndex = data;
        }
    }

    Connections{
        target: jetson_obj
    }

    Connections{
        target: terminal_obj
    }

    Component.onCompleted: {
        console.log("window completed");
        btnFlashImage.enabled = false;
        btnFlashDTB.enabled = false;
    }

    onWindowStateChanged: {
        console.log("windowState: " + windowState);
    }

    FileDialog {
        id: fileDialog
        title: "Please choose a file"
        nameFilters: ["Config files (*.xml)"]
        folder: shortcuts.home
        onAccepted: {
            console.log("You chose: " + fileDialog.fileUrl)
            loadConfig(fileDialog.fileUrl);
        }
        onRejected: {
            console.log("Canceled")
        }
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
                rows: 8
                columns: 4

                ComboBox {
                    id: cbProject
                    Layout.columnSpan: 3
                    model: jetson_obj.projectList
                    onCurrentIndexChanged: {
                        projectChanged(currentIndex);
                    }
                    onCurrentTextChanged: {
                        if(txtLastProject.text != cbProject.currentText)
                        {
                            txtLastProject.color = "red";
                        }
                        else
                        {
                            txtLastProject.color = "black";
                        }
                    }
                }

                StatusIndicator {
                    id: statusSetting
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 1
                    color: "green"
                }

                Text {
                    id: txtLastProject
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 1
                }

                Text {
                    id: txtLastUpdate
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 3
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
                    onCurrentTextChanged: {
                        if(txtLastDispOut.text != cbDisplayOut.currentText)
                        {
                            txtLastDispOut.color = "red";
                        }
                        else
                        {
                            txtLastDispOut.color = "black";
                        }
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
                    onCurrentTextChanged: {
                        if(txtLastRemoteUpgrade.text != cbRemoteUpgrade.currentText)
                        {
                            txtLastRemoteUpgrade.color = "red";
                        }
                        else
                        {
                            txtLastRemoteUpgrade.color = "black";
                        }
                    }
                    onModelChanged: {
                        var _maxWidth = 0
                        for(var i = 0; i < model.length; i++){
                            // TextMetrics does not work with Material Style
                            _maxWidth = Math.max((model[i].length+1)*cbRemoteUpgrade.font.pixelSize, _maxWidth)
                        }
                        Layout.minimumWidth = _maxWidth

                        if(_maxWidth > 140)
                        {
                            root.width = root.minimumWidth + (_maxWidth-140);
                        }
                    }
                }

                Text {
                    id: txtLastDispOut
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 2
                }

                Text {
                    id: txtLastRemoteUpgrade
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
                    onCurrentTextChanged: {
                        if(txtLastIp.text != cbIp.currentText)
                        {
                            txtLastIp.color = "red";
                        }
                        else
                        {
                            txtLastIp.color = "black";
                        }
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
                    onCurrentTextChanged: {
                        if(txtLastDispCtrl.text != cbDispCtrl.currentText)
                        {
                            txtLastDispCtrl.color = "red";
                        }
                        else
                        {
                            txtLastDispCtrl.color = "black";
                        }
                    }
                    onModelChanged: {
                        var _maxWidth = 0
                        for(var i = 0; i < model.length; i++){
                            // TextMetrics does not work with Material Style
                            _maxWidth = Math.max((model[i].length+1)*cbDispCtrl.font.pixelSize, _maxWidth)
                        }
                        Layout.minimumWidth = _maxWidth

                        if(_maxWidth > 140)
                        {
                            root.width = root.minimumWidth + (_maxWidth-140);
                        }
                    }
                }

                Text {
                    id: txtLastIp
                    text: qsTr("")
                    font.pixelSize: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 2
                }

                Text {
                    id: txtLastDispCtrl
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
                    Layout.topMargin: 10
                    Layout.alignment: Qt.AlignRight | Qt.AlignVCenter
                    Layout.columnSpan: 4

                    Button {
                        id: btnLoadLastSetting
                        text: qsTr("Load")
                        onClicked: {
                            fileDialog.open();
//                            jetson_obj.loadSettingInfo("D:\\Projects\\JetsonTX2\\Software\\SystemFlashing\\init_test.xml");
                        }
                    }
                    Button {
                        id: btnFlashImage
                        enabled: false
                        text: qsTr("Flash Image")
                        onClicked: {
                            /*
                            if(txtLastProject.text != cbProject.currentText)
                            {
                                flashImage()
                                return
                            }
                            if(txtLastDispOut.text != cbDisplayOut.currentText)
                            {
                                flashImage()
                                return
                            }
                            */
                            if(txtLastIp.text != cbIp.currentText)
                            {
                                flashImage()
                                return
                            }
                            if(txtLastRemoteUpgrade.text != cbRemoteUpgrade.currentText)
                            {
                                flashImage()
                                return
                            }
                            if(txtLastDispCtrl.text != cbDispCtrl.currentText)
                            {
                                flashImage()
                                return
                            }

                            flashImageNoMake()
                        }
                    }
                    Button {
                        id: btnFlashDTB
                        enabled: false
                        text: qsTr("Flash Dtb")
                        onClicked: {
                            flashDtb();
                        }
                    }
                }


            }

        }

        Flickable {
            id: cmdWindow
            Layout.alignment: Qt.AlignCenter
            Layout.fillHeight: true
            Layout.fillWidth: true
            Layout.leftMargin: 20
            Layout.rightMargin: 20
            Layout.bottomMargin: 20

            TextArea.flickable: TextArea {
                id: textArea
                color: "#ffffff"
                selectionColor: "#ffffff"
                selectedTextColor: "#000000"

                anchors.rightMargin: 5
                anchors.leftMargin: 5
                anchors.bottomMargin: 0
                anchors.topMargin: 0
                clip: true

                font.pixelSize: 12
                font.family: "Courier"

                selectByMouse: true
                textFormat: Text.AutoText
                wrapMode: Text.NoWrap
                readOnly: true

                background: Rectangle {
                    border.color: "blue"
                    color: "#000000"
                }

                Keys.onPressed: {
                    if((event.modifiers & Qt.ControlModifier) &&
                            (event.key == Qt.Key_W))
                    {
                        console.log("clear");
                        textArea.clear();
                        flickable.update();
                    }
                    else if((event.modifiers == Qt.NoModifier) ||
                            (event.modifiers == Qt.ShiftModifier))
                    {
                        //textArea.insert(textArea.length, event.text);
                        //textArea.cursorPosition = textArea.length-1;
                        keyPressed(event.text);
                    }
                }

            }
            ScrollBar.vertical: ScrollBar {}
        }
    }

}
