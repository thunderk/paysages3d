import QtQuick 2.0

Item {
    property string picture
    property bool selected: false
    property bool hovered: false
    property string helptext
    property string hovertext

    width: image.width + 10
    height: image.height + 10

    Rectangle {
        id: glow
        anchors.fill: parent
        color: "white"
        radius: 8

        opacity: parent.selected ? 1.0 : (parent.hovered ? 0.5 : 0.0)
        Behavior on opacity {
            PropertyAnimation {
                duration: 200
            }
        }
    }

    Image {
        id: image
        source: parent.picture
        anchors.centerIn: parent
        width: 32
        height: 32
    }

    MouseArea {
        anchors.fill: parent
        hoverEnabled: true
        cursorShape: Qt.PointingHandCursor

        onEntered: {
            parent.hovered = true;
            tooltip_widget.hovertext = hovertext;
        }
        onExited: {
            parent.hovered = false;
            tooltip_widget.hovertext = "";
        }
        onClicked: {
            parent.selected = !parent.selected;
            if (parent.selected)
            {
                var toolbar = parent.parent;
                for (var i = 0; i < toolbar.children.length; ++i)
                {
                    var child = toolbar.children[i]
                    if (child !== parent)
                    {
                        child.selected = false;
                    }
                }
                tooltip_widget.helptext = helptext;
            }
            else
            {
                tooltip_widget.helptext = "";
            }
        }
    }
}
