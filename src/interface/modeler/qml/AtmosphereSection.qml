import QtQuick 2.0

// Atmosphere control
BaseSection {
    id: section

    BaseToolbar {
        id: toolbar

        ToolbarButton {
            id: tool_daytime
            checked: true
            picture: "images/icon_atmosphere_daytime.png"
            hovertext: qsTr("Control the sun and moon")
        }

        ToolbarButton {
            id: tool_sun_radius
            picture: "images/icon_sun_radius.png"
            hovertext: qsTr("Radius of the sun")
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

    PanelSimpleFloat {
        id: panel_sun_radius
        anchors.left: toolbar.right
        minimumValue: 0
        maximumValue: 3
        enabled: false
        objectName: "atmosphere_sun_radius"
    }

    states: [
        State {
            name: "DayTime"
            when: tool_daytime.checked
            PropertyChanges {
                target: panel_daytime
                enabled: true
            }
        },
        State {
            name: "Sun radius"
            when: tool_sun_radius.checked
            PropertyChanges {
                target: panel_sun_radius
                enabled: true
            }
            PropertyChanges {
                target: section
                tool: "sun"
            }
        },
        State {
            name: "Humidity"
            when: tool_humidity.checked
            PropertyChanges {
                target: panel_humidity
                enabled: true
            }
        }
    ]
}

