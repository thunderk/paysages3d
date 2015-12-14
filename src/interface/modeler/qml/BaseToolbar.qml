import QtQuick 2.0
import QtQuick.Controls 1.2

BaseRectangle {

    id: toolbar
    default property alias children : inner_space.children
    property bool horizontal: false
    property list<BasePanel> panels
    property var current
    width: horizontal ? parent.width : 60
    height: horizontal ? 60 : parent.height
    color: "#50888888"

    ExclusiveGroup {
        id: exclusive_checked
    }

    Grid {
        id: inner_space
        columns: parent.horizontal ? children.length : 1
        rows: parent.horizontal ? 1 : children.length
        spacing: (parent.horizontal ? (parent.width - children.length * tool_terrain.width) : (parent.height - children.length * tool_terrain.height)) / (children.length + 1)
        anchors.centerIn: parent

        onChildrenChanged: {
            for (var i = 0; i < children.length; ++i)
            {
                children[i].toolbar = toolbar;
                if (!children[i].toggle)
                {
                    children[i].exclusiveGroup = exclusive_checked;
                }
            }
        }
    }
}
