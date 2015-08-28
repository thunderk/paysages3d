import QtQuick 2.2
import Paysages 1.0

OpenGLView {
    id: main_ui
    state: "Init"
    property string previous_state

    width: 1000
    height: 700

    Tooltip {
        id: tooltip_widget

        anchors.top: primary_toolbar.bottom
        anchors.right: primary_toolbar.right
    }

    BaseToolbar {
        id: primary_toolbar
        horizontal:  true
        color: "#90888888"
        property var current

        anchors.top: parent.top
        anchors.left: parent.left

        ToolbarButton {
            id: tool_terrain
            picture: "images/tab_terrain.png"
            hovertext: qsTr("Terrain edition")
        }
        ToolbarButton {
            id: tool_textures
            picture: "images/tab_textures.png"
            hovertext: qsTr("Terrain textures")
        }
        ToolbarButton {
            id: tool_water
            picture: "images/icon_water.png"
            hovertext: qsTr("Water tools")
        }
        ToolbarButton {
            id: tool_atmosphere
            picture: "images/icon_atmosphere.png"
            hovertext: qsTr("Atmosphere/weather control")
        }
        ToolbarButton {
            id: tool_clouds
            picture: "images/tab_clouds.png"
            hovertext: qsTr("Cloud layers")
        }
        ToolbarButton {
            id: tool_render
            picture: "images/tab_render.png"
            hovertext: qsTr("Rendering")
        }
        ToolbarButton {
            id: tool_file
            picture: "images/tab_file.png"
            hovertext: qsTr("File")
        }
    }

    WaterSection {
        id: water_section
        enabled: false
    }

    AtmosphereSection {
        id: atmosphere_section
        enabled: false
    }

    BaseSecondaryToolbar {
        id: render_toolbar

        ToolbarButton {
            id: tool_render_quick
            objectName: "tool_render_quick"
            picture: "images/icon_render_quick.png"
            hovertext: qsTr("Start a quick render")
            shortcut: "F5"
        }

        ToolbarButton {
            id: tool_render_final
            objectName: "tool_render_final"
            picture: "images/tab_render.png"
            hovertext: qsTr("Start a final render")
            shortcut: "Ctrl+F5"
        }

        ToolbarButton {
            id: tool_render_show
            objectName: "tool_render_show"
            picture: "images/icon_render_show.png"
            hovertext: qsTr("Show last rendered image")
            shortcut: "F6"
        }
    }

    BaseSecondaryToolbar {
        id: file_toolbar

        ToolbarButton {
            id: tool_file_new
            objectName: "tool_file_new"
            picture: "images/icon_file_new.png"
            hovertext: qsTr("Generate a new scene")
            shortcut: "Ctrl+N"
        }

        ToolbarButton {
            id: tool_file_save
            objectName: "tool_file_save"
            picture: "images/icon_file_save.png"
            hovertext: qsTr("Save the current scene to a file")
            shortcut: "Ctrl+S"
        }

        ToolbarButton {
            id: tool_file_load
            objectName: "tool_file_load"
            picture: "images/icon_file_load.png"
            hovertext: qsTr("Load a scene from a file")
            shortcut: "Ctrl+L"
        }

        ToolbarButton {
            id: tool_file_exit
            objectName: "tool_file_exit"
            picture: "images/icon_exit.png"
            hovertext: qsTr("Exit the program")
            shortcut: "Ctrl+Q"
        }
    }

    CameraChoice {
        id: camera_choice
        height: 150
        anchors.right: main_ui.right
        anchors.verticalCenter: main_ui.verticalCenter
    }

    RenderDialog {
        id: render_dialog
        enabled: false
        anchors.fill: parent
    }

    onStateChanged: {
        if (state != "Render Dialog")
        {
            previous_state = state;
        }
    }

    states: [
        State {
            name: "Water Mode"
            when: primary_toolbar.current === tool_water

            PropertyChanges {
                target: water_section
                enabled: true
            }
        },
        State {
            name: "Atmosphere Mode"
            when: primary_toolbar.current === tool_atmosphere

            PropertyChanges {
                target: atmosphere_section
                enabled: true
            }
        },
        State {
            name: "Render Mode"
            when: primary_toolbar.current === tool_render

            PropertyChanges {
                target: render_toolbar
                enabled: true
            }
        },
        State {
            name: "File Mode"
            when: primary_toolbar.current === tool_file

            PropertyChanges {
                target: file_toolbar
                enabled: true
            }
        },
        State {
            name: "Render Dialog"

            PropertyChanges {
                target: render_dialog
                enabled: true
            }
        }
    ]

}
