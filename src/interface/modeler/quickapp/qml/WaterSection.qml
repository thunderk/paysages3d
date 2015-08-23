import QtQuick 2.0

// Water control
BaseSection {
    id: section

    BaseToolbar {
        id: toolbar

        ToolbarButton {
            id: tool_water_level
            selected: true
            picture: "images/icon_water_level.png"
            hovertext: qsTr("Change the water altitude")
        }
    }

    BasePanel {
        id: panel_water_level
        width: 40
        anchors.left: toolbar.right
        enabled: false

        BaseSlider {
            objectName: "water_height"
            minimumValue: -1
            maximumValue: 1
            orientation: Qt.Vertical
            anchors.fill: parent
            anchors.margins: 10
        }
    }

    states: [
        State {
            name: "WaterHeight"
            when: tool_water_level.selected
            PropertyChanges {
                target: panel_water_level
                enabled: true
            }
        }
    ]
}

