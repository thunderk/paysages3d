import QtQuick 2.0

// Water control
BaseSection {
    id: section

    BaseToolbar {
        id: toolbar

        ToolbarButton {
            id: tool_water_preset
            picture: "images/icon_preset.png"
            hovertext: qsTr("Choose a water model")
        }

        ToolbarButton {
            id: tool_water_level
            checked: true
            picture: "images/icon_water_level.png"
            hovertext: qsTr("Change the water altitude")
        }

        ToolbarButton {
            id: tool_water_reflection
            picture: "images/icon_water_reflection.png"
            hovertext: qsTr("Reflection of the water surface")
        }
    }

    PanelPictureChoice {
        id: panel_water_preset
        enabled: false
        anchors.left: toolbar.right
        objectName: "water_model"
        items: [
            PictureChoiceItem {
                picture: "images/preset_water_lake.jpg"
                hovertext: "Water model : Lake"
            },
            PictureChoiceItem {
                picture: "images/preset_water_sea.jpg"
                hovertext: "Water model : Sea"
            }
        ]
    }

    PanelSimpleFloat {
        id: panel_water_level
        enabled: false
        anchors.left: toolbar.right
        minimumValue: -1
        maximumValue: 1
        objectName: "water_height"
    }

    PanelSimpleFloat {
        id: panel_water_reflection
        enabled: false
        anchors.left: toolbar.right
        objectName: "water_reflection"
    }

    states: [
        State {
            name: "WaterPreset"
            when: tool_water_preset.checked
            PropertyChanges {
                target: panel_water_preset
                enabled: true
            }
        },
        State {
            name: "WaterHeight"
            when: tool_water_level.checked
            PropertyChanges {
                target: panel_water_level
                enabled: true
            }
        },
        State {
            name: "WaterReflection"
            when: tool_water_reflection.checked
            PropertyChanges {
                target: panel_water_reflection
                enabled: true
            }
        }
    ]
}

