import QtQuick 2.0

Rectangle {
    id: camera_choice
    width: 200
    height: 50
    color: "#90888888"
    objectName: "camera_choice"

    Row {
        id: inner_space
        anchors.centerIn: parent
        spacing: 15

        ToolbarButton {
            id: camera_choice_render
            picture: "images/tab_display.png"
            hovertext: qsTr("Switch to the final camera")
            selected: true
        }

        ToolbarButton {
            id: camera_choice_topdown
            picture: "images/display_topdown.png"
            hovertext: qsTr("Switch to the top-down camera")
        }
    }

    states: [
        State {
            name: "Render camera"
            when: camera_choice_render.selected
        },
        State {
            name: "Top-down camera"
            when: camera_choice_topdown.selected
        }
    ]
}
