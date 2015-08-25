import QtQuick 2.0

// Atmosphere control
BaseSection {
    id: section

    BaseToolbar {
        id: toolbar

        ToolbarButton {
            id: tool_daytime
            selected: true
            picture: "images/icon_atmosphere_daytime.png"
            hovertext: qsTr("Change the time of day")
        }

        ToolbarButton {
            id: tool_humidity
            picture: "images/icon_atmosphere.png"
            hovertext: qsTr("Humidity factor in the air")
        }
    }

    PanelAtmosphereDaytime {
        id: panel_daytime
        anchors.left: toolbar.right
        enabled: false
    }

    PanelSimpleFloat {
        id: panel_humidity
        anchors.left: toolbar.right
        enabled: false
        objectName: "atmosphere_humidity"
    }

    states: [
        State {
            name: "DayTime"
            when: tool_daytime.selected
            PropertyChanges {
                target: panel_daytime
                enabled: true
            }
        },
        State {
            name: "Humidity"
            when: tool_humidity.selected
            PropertyChanges {
                target: panel_humidity
                enabled: true
            }
        }
    ]
}

