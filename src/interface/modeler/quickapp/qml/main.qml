import QtQuick 2.2
import Paysages 1.0

OpenGLView {
    id: main_ui
    state: "Init"

    width: 800
    height: 600

    Tooltip {
        id: tooltip_widget

        anchors.top: primary_toolbar.bottom
        anchors.right: primary_toolbar.right
    }

    Toolbar {
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

    BaseSecondaryToolbar {
        id: water_toolbar

        ToolbarButton {
            id: tool_water_level
            picture: "images/icon_water_level.png"
            hovertext: qsTr("Change the water altitude")
        }
    }

    BaseSecondaryToolbar {
        id: atmosphere_toolbar

        ToolbarButton {
            id: tool_atmosphere_daytime
            picture: "images/icon_atmosphere_daytime.png"
            hovertext: qsTr("Change the time of day")
        }
    }

    BaseSecondaryToolbar {
        id: render_toolbar

        ToolbarButton {
            id: tool_render_quick
            objectName: "tool_render_quick"
            picture: "images/tab_render.png"
            hovertext: qsTr("Start a quick render")
        }
    }

    BaseSecondaryToolbar {
        id: file_toolbar

        ToolbarButton {
            id: tool_file_new
            objectName: "tool_file_new"
            picture: "images/icon_file_new.png"
            hovertext: qsTr("Generate a new scene")
        }

        ToolbarButton {
            id: tool_file_save
            objectName: "tool_file_save"
            picture: "images/icon_file_save.png"
            hovertext: qsTr("Save the current scene to a file")
        }

        ToolbarButton {
            id: tool_file_load
            objectName: "tool_file_load"
            picture: "images/icon_file_load.png"
            hovertext: qsTr("Load a scene from a file")
        }

        ToolbarButton {
            id: tool_file_exit
            objectName: "tool_file_exit"
            picture: "images/icon_exit.png"
            hovertext: qsTr("Exit the program")
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

    PanelWaterLevel {
        id: panel_water_level
        tool: tool_water_level
    }
    PanelAtmosphereDaytime {
        id: panel_atmosphere_daytime
        tool: tool_atmosphere_daytime
    }

    states: [
        State {
            name: "Water Mode"
            when: tool_water.selected

            PropertyChanges {
                target: water_toolbar
                enabled: true
            }
        },
        State {
            name: "Atmosphere Mode"
            when: tool_atmosphere.selected

            PropertyChanges {
                target: atmosphere_toolbar
                enabled: true
            }
        },
        State {
            name: "Render Mode"
            when: tool_render.selected

            PropertyChanges {
                target: render_toolbar
                enabled: true
            }
        },
        State {
            name: "File Mode"
            when: tool_file.selected

            PropertyChanges {
                target: file_toolbar
                enabled: true
            }
        },
        State {
            name: "Render Dialog"

            PropertyChanges {
                target: primary_toolbar
                enabled: false
            }
            PropertyChanges {
                target: camera_choice
                enabled: false
            }
            PropertyChanges {
                target: render_dialog
                enabled: true
            }
        }
    ]

}
