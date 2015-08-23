import QtQuick 2.0

BaseRectangle {

    default property alias children : inner_space.children
    property bool horizontal: false
    property list<BasePanel> panels
    width: horizontal ? parent.width : 60
    height: horizontal ? 60 : parent.height
    color: "#50888888"

    Grid {
        id: inner_space
        columns: parent.horizontal ? children.length : 1
        rows: parent.horizontal ? 1 : children.length
        spacing: (parent.horizontal ? (parent.width - children.length * tool_terrain.width) : (parent.height - children.length * tool_terrain.height)) / (children.length + 1)
        anchors.centerIn: parent
    }
}
