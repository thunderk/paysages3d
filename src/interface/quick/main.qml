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

            Image {
                id: tool_terrain
                opacity: 0.5
                source: "qrc:///toolbar/primary/tab_terrain.png"
            }
            Image {
                id: tool_textures
                opacity: 0.5
                source: "qrc:///toolbar/primary/tab_textures.png"
            }
            Image {
                id: tool_water
                opacity: 0.5
                source: "qrc:///toolbar/primary/tab_water.png"
            }
            Image {
                id: tool_atmosphere
                opacity: 0.5
                source: "qrc:///toolbar/primary/tab_atmosphere.png"
            }
            Image {
                id: tool_clouds
                opacity: 0.5
                source: "qrc:///toolbar/primary/tab_clouds.png"
            }
            Image {
                id: tool_render
                opacity: 0.5
                source: "qrc:///toolbar/primary/tab_render.png"
            }
        }
    }

    Rectangle {
        id: toolbar_render
        anchors.top: primary_toolbar.top
        anchors.left: primary_toolbar.right
        width: primary_toolbar.width
        height: primary_toolbar.height
        color: "green"

        SequentialAnimation on width {
            loops: Animation.Infinite
            PropertyAnimation { to: 0; duration: 1000 }
            PropertyAnimation { to: primary_toolbar.width; duration: 1000 }
        }
    }

}
