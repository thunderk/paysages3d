import QtQuick 2.0

BaseRectangle {

    default property alias children : inner_space.children
    width: 70
    height: parent.height
    color: "#50888888"

    Column {
        id: inner_space
        spacing: (parent.height - children.length * tool_terrain.height) / (children.length + 1)
        anchors.centerIn: parent
    }

    onEnabledChanged: {
        if (!enabled)
        {
            for (var i = 0; i < children.length; i++)
            {
                children[i].selected = false;
            }
        }
    }
}
