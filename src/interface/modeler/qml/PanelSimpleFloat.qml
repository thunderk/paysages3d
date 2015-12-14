import QtQuick 2.0

// Simple panel with a single float slider
BasePanel {
    id: panel
    property alias objectName: slider.objectName
    property alias minimumValue: slider.minimumValue
    property alias maximumValue: slider.maximumValue
    width: 40
    
    BaseSlider {
        id: slider
        minimumValue: 0
        maximumValue: 1
        orientation: Qt.Vertical
        anchors.fill: parent
        anchors.margins: 10
    }
}
