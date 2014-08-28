import QtQuick 2.0

Rectangle {

    default property alias children : inner_space.children
    width: 70
    height: parent.height
    color: "#50888888"
    enabled: opacity > 0

    Column {
        id: inner_space
        spacing: (parent.height - children.length * tool_terrain.height) / (children.length + 1)
        anchors.centerIn: parent
    }

    Behavior on opacity {
        PropertyAnimation {
            duration: 200
        }
    }
}
