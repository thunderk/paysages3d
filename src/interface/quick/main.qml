import QtQuick 2.2
import Paysages 1.0

OpenGLView {

    width: 640
    height: 480

    Item {
        id: primary_toolbar
        x: 0
        y: 0
        width: 70
        height: parent.height

        Rectangle {
            width: parent.height
            height: parent.width
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            transformOrigin: Item.Center
            rotation: -90
            gradient: Gradient {
                GradientStop { position: 0.0; color: "#605055FF" }
                GradientStop { position: 1.0; color: "#005055FF" }
            }
        }

        Column {

            anchors.horizontalCenter: parent.horizontalCenter
            anchors.verticalCenter: parent.verticalCenter
            spacing: (parent.height - 6 * tool_terrain.height) / 7

            ToolbarButton {
                id: tool_terrain
                code: "terrain"
            }
            ToolbarButton {
                id: tool_textures
                code: "textures"
            }
            ToolbarButton {
                id: tool_water
                code: "water"
            }
            ToolbarButton {
                id: tool_atmosphere
                code: "atmosphere"
            }
            ToolbarButton {
                id: tool_clouds
                code: "clouds"
            }
            ToolbarButton {
                id: tool_render
                code: "render"
            }
        }
    }

    Rectangle {
        id: toolbar_render
        anchors.top: primary_toolbar.top
        anchors.left: primary_toolbar.right
        width: primary_toolbar.width
        height: primary_toolbar.height
        color: "#8800FF00"

        SequentialAnimation on width {
            loops: Animation.Infinite
            PropertyAnimation { to: 0; duration: 1000 }
            PropertyAnimation { to: primary_toolbar.width; duration: 1000 }
        }
    }

}
