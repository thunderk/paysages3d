import QtQuick 2.0

BaseRectangle {
    id: panel
    width: 200
    height: parent.height
    property string title: ""

    anchors.top: parent.top

    color: "#40909090"

    Text {
        text: title
        visible: title != ""
        font.bold: true
        font.pixelSize: 12
        anchors.top: parent.top
        anchors.horizontalCenter: parent.horizontalCenter
    }
}
