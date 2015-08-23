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
    }

    PanelAtmosphereDaytime {
        id: panel_daytime
        anchors.left: toolbar.right
        enabled: false
    }

    states: [
        State {
            name: "DayTime"
            when: tool_daytime.selected
            PropertyChanges {
                target: panel_daytime
                enabled: true
            }
        }
    ]
}

