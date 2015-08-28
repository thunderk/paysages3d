import QtQuick 2.0

// Water control
BaseSection {
    id: section

    BaseToolbar {
        id: toolbar

        ToolbarButton {
            id: tool_water_level
            checked: true
            picture: "images/icon_water_level.png"
            hovertext: qsTr("Change the water altitude")
        }
    }

    PanelSimpleFloat {
        id: panel_water_level
        enabled: false
        anchors.left: toolbar.right
        minimumValue: -1
        maximumValue: 1
        objectName: "water_height"
    }

    states: [
        State {
            name: "WaterHeight"
            when: tool_water_level.checked
            PropertyChanges {
                target: panel_water_level
                enabled: true
            }
        }
    ]
}

