import QtQuick 2.2
import Paysages 1.0

OpenGLView {
    id: main_ui
    state: "Init"

    width: 800
    height: 600

    Tooltip {
        id: tooltip_widget

        anchors.top: parent.top
        anchors.right: parent.right
    }

    Toolbar {
        id: primary_toolbar
        color: "#90000000"

        anchors.left: parent.left

        ToolbarButton {
            id: tool_display
            picture: "images/tab_display.png"
            hovertext: qsTr("Display options")
        }
        ToolbarButton {
            id: tool_terrain
            picture: "images/tab_terrain.png"
        }
        ToolbarButton {
            id: tool_textures
            picture: "images/tab_textures.png"
        }
        ToolbarButton {
            id: tool_water
            picture: "images/tab_water.png"
        }
        ToolbarButton {
            id: tool_atmosphere
            picture: "images/tab_atmosphere.png"
        }
        ToolbarButton {
            id: tool_clouds
            picture: "images/tab_clouds.png"
        }
        ToolbarButton {
            id: tool_render
            picture: "images/tab_render.png"
        }
    }

    Toolbar {
        id: display_toolbar
        opacity: 0
        anchors.left: primary_toolbar.right

        ToolbarButton {
            id: tool_display_topdown
            picture: "images/display_topdown.png"
            hovertext: qsTr("Top-down view")
            helptext: qsTr("Drag the mouse on the map to change the viewpoint.")
        }
    }

    states: [
        State {
            name: "Display Mode"
            when: tool_display.selected

            PropertyChanges {
                target: display_toolbar
                opacity: 1
            }
        }
    ]

}
