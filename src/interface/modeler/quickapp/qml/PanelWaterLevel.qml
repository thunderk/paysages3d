import QtQuick 2.2

BasePanel {
    width: 40

    BaseSlider {
        objectName: "water_height"
        minimumValue: -1
        maximumValue: 1
        orientation: Qt.Vertical
        anchors.fill: parent
        anchors.margins: 10
    }
}
