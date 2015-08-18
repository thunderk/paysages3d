import QtQuick 2.2

BasePanel {
    width: 20

    BaseSlider {
        objectName: "water_height"
        minimumValue: -1
        maximumValue: 1
        orientation: Qt.Vertical
        anchors.fill: parent
    }
}
