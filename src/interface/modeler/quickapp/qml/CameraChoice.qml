import QtQuick 2.0

Toolbar {
    id: camera_choice
    horizontal: false
    color: "#90888888"
    objectName: "camera_choice"

    ToolbarButton {
        id: camera_choice_render
        picture: "images/tab_display.png"
        hovertext: qsTr("Final render camera")
        selected: true
    }

    ToolbarButton {
        id: camera_choice_topdown
        picture: "images/display_topdown.png"
        hovertext: qsTr("Top-down camera")
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
